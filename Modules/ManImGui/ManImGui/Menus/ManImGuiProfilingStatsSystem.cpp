#include "ManImGuiProfilingStatsSystem.h"

#include <ManImGui/ManImGui.h>
#include <ManImGui/ManImGuiSystem.h>
#include <ManImGui/ManImGuiManifoldMenuSystem.h>

#include <Core/TimeSystem.h>
#include <Core/Debug/Profiling.h>
#include "imgui.h"

using namespace Mani;

constexpr std::string_view PROFILER_NAME = "Profiler";

void ManImGuiProfilingStatsSystem::onInitialize(ECS::Registry& registry, World& world)
{
	world.initializeDependency<TimeSystem>();
	world.initializeDependency<ManImGuiSystem>();
	world.initializeDependency<ManImGuiManifoldMenuSystem>();

	{
		const ECS::EntityId debugMenuId = ManImGuiStatics::ManifoldMenu::getEntityId(registry);
		ManImGuiStatics::Menu::addItem(registry, debugMenuId, PROFILER_NAME);
	}
}

bool Mani::ManImGuiProfilingStatsSystem::shouldTick(const ECS::Registry& registry) const
{
	return ManImGuiStatics::isShowing(registry);
}

void ManImGuiProfilingStatsSystem::tick(Mani::ECS::Registry& registry)
{
	const ECS::EntityId debugMenuId = ManImGuiStatics::ManifoldMenu::getEntityId(registry);
	if (!ManImGuiStatics::Menu::isOpened(registry, debugMenuId, PROFILER_NAME))
	{
		return;
	}

	// this system might not be in the application's registry, but below it.
	const ECS::Registry& appRegistry = Application::get().getWorld().getRegistry();
	const ScopedTimerDatabase* database = appRegistry.getSingle<ScopedTimerDatabase>();
	if (database == nullptr)
	{
		return;
	}

	const List<std::string> keys = database->scopedTimers.keys().sortCopy();

	bool isOpened = true;
	if (!ImGui::Begin("Profiling Stats", &isOpened, ImGuiWindowFlags_MenuBar))
	{
		ImGui::End();
		return;
	}

	if (!isOpened)
	{
		ManImGuiStatics::Menu::close(registry, debugMenuId, PROFILER_NAME);
		ImGui::End();
		return;
	}

	Time& time = *registry.getSingle<Time>();
	const float fps = Math::isEqual(time.delta, 0.f) ? 0.f : 1.f / time.delta;
	ImGui::Text(std::format("{:.3}fps, entity count {}", fps, registry.count()).c_str());
	for (const auto& name : keys)
	{
		const auto& stats = database->scopedTimers.get(name);

		ImGui::Indent();
		ImGui::Separator();
		ImGui::Text(std::format("{}: {:.3}ms", name, stats.lastTick).c_str());

		ImGui::SameLine();
		if (ImGui::TreeNode(std::format("##tree{}", name).c_str()))
		{
			ImGui::Text(std::format("count: {}", stats.count).c_str());
			ImGui::Text(std::format("min: {:.3}ms", stats.min).c_str());
			ImGui::Text(std::format("max: {:.3}ms", stats.max).c_str());

			double average = 0;
			if (stats.count > 0)
			{
				average = stats.accumulator / stats.count;
			}

			ImGui::Text(std::format("average: {:.3}ms", average).c_str());
			ImGui::Text(std::format("last value {:.3}ms", stats.lastValue).c_str());
			ImGui::Text(std::format("per tick: {:.3}ms", stats.lastTick).c_str());
			ImGui::TreePop();
		}
		ImGui::Unindent();
	}
	ImGui::End();
}
