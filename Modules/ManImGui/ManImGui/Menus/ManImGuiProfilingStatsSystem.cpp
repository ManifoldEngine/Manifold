#include "ManImGuiProfilingStatsSystem.h"

#include <ManImGui/ManImGui.h>
#include <ManImGui/ManImGuiSystem.h>
#include <ManImGui/ManImGuiManifoldMenuSystem.h>

#include <Core/TimeSystem.h>
#include <Core/Profiling/Profiling.h>
#include "imgui.h"

using namespace Mani;

constexpr std::string_view PROFILER_NAME = "Profiler";

void ManImGuiProfilingStatsSystem::onInitialize(ECS::Registry& registry, World& world)
{
	world.initializeDependency<TimeSystem>();
	world.initializeDependency<ManImGuiSystem>();
	world.initializeDependency<ManImGuiManifoldMenuSystem>();
	
	{
		Ref<ManImGuiMenu> menu = ManImGuiStatics::Manifold::getMenu(registry);
		menu->subMenu.addItem(PROFILER_NAME);
	}
}

bool Mani::ManImGuiProfilingStatsSystem::shouldTick(const ECS::Registry& registry) const
{
	if (!ManImGuiStatics::isShowing(registry))
	{
		return false;
	}

	Ref<const ManImGuiMenu> menu = ManImGuiStatics::Manifold::getMenu(registry);
	return menu->subMenu.getSelected(PROFILER_NAME);
}

void ManImGuiProfilingStatsSystem::tick(Mani::ECS::Registry& registry)
{
	bool isOpened = true;
	if (!ImGui::Begin("Profiling Stats", &isOpened, ImGuiWindowFlags_MenuBar))
	{
		ImGui::End();
		return;
	}

	if (!isOpened)
	{
		Ref<ManImGuiMenu> menu = ManImGuiStatics::Manifold::getMenu(registry);
		menu->subMenu.setSelected(PROFILER_NAME, false);
		ImGui::End();
		return;
	}

	Ref<Time> time = registry.getSingle<Time>();
	const float fps = Math::isEqual(time->delta, 0.f) ? 0.f : 1.f / time->delta;
	ImGui::Text(std::format("{:.3}fps, entity count {}", fps, registry.count()).c_str());
	ImGui::Separator();
	
	List<Profiling::Record> records = Application::get().getProfiler().getLastFrameRecords();
	records.sort([](const Profiling::Record& lhs, const Profiling::Record& rhs)
	{
		return lhs.name.compare(rhs.name) < 0;
	});

	ImGui::Indent();
	for (const auto& record : records)
	{
		ImGui::Text(std::format("{}: {:.3}ms", record.name, record.duration).c_str());
	}
	ImGui::Unindent();
	ImGui::End();
}
