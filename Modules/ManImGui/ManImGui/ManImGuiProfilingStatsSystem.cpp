#include "ManImGuiProfilingStatsSystem.h"

#include "ManImGuiBeginFrameSystem.h"
#include "ManImGuiWindowContext.h"
#include "ManImGuiSystem.h"

#include <Core/Debug/Profiling.h>
#include "imgui.h"

using namespace Mani;

void ManImGuiProfilingStatsSystem::onInitialize(ECS::Registry& registry, World& world)
{
	world.initializeDependency<ManImGuiSystem>();
}

void ManImGuiProfilingStatsSystem::tick(float deltaTime, Mani::ECS::Registry& registry)
{
	ManImGuiWindowContext* context = registry.getSingle<ManImGuiWindowContext>();
	MANI_ASSERT(context != nullptr, "We expect the context to be accessible");
	if (context->mode < EManImGuiMode::Show)
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

	bool isActive = false;
	ImGui::Begin("Profiling Stats", &isActive, ImGuiWindowFlags_MenuBar);
	const float fps = Math::isEqual(deltaTime, 0.f) ? 0.f : 1.f / deltaTime;
	ImGui::Text(std::format("{:.3}fps", fps).c_str());
	for (const auto& [name, stats] : database->scopedTimers)
	{
		ImGui::Indent();
		ImGui::Separator();
		ImGui::Text(std::format("{}: {:.3}ms", name, stats.lastValue).c_str());

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
			ImGui::TreePop();
		}
		ImGui::Unindent();
	}
	ImGui::End();
}
