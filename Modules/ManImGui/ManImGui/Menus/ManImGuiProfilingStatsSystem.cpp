#include "ManImGuiProfilingStatsSystem.h"

#include <Core/Containers/Map.h>

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
		Ref<ManImGuiMenu> menu = ManImGui::Manifold::getMenu(registry);
		menu->subMenu.addItem(PROFILER_NAME);
	}
}

bool Mani::ManImGuiProfilingStatsSystem::shouldTick(const ECS::Registry& registry) const
{
	if (!ManImGui::isShowing(registry))
	{
		return false;
	}

	Ref<const ManImGuiMenu> menu = ManImGui::Manifold::getMenu(registry);
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
		Ref<ManImGuiMenu> menu = ManImGui::Manifold::getMenu(registry);
		menu->subMenu.setSelected(PROFILER_NAME, false);
		ImGui::End();
		return;
	}

	Ref<Time> time = registry.getSingle<Time>();
	const float fps = Math::isEqual(time->delta, 0.f) ? 0.f : 1.f / time->delta;
	ImGui::Text(std::format("{:.3}fps, entity count {}", fps, registry.count()).c_str());
	ImGui::Separator();
	
	const List<Profiling::Record>& records = Application::get().getProfiler().getLastFrameRecords();
	MANI_LOG_VERBOSE(ManImGuiLog, "{}: Showing {} records", PROFILER_NAME, records.count());
	
	Map<Profiling::RecordId, Profiling::Record> aggregatedRecords;
	aggregatedRecords.reserve(records.count());
	for (const auto& record : records)
	{
		Profiling::Record& aggregatedRecord = aggregatedRecords[record.id];
		aggregatedRecord.name = record.name;
		aggregatedRecord.duration += record.duration;
	}

	ImGui::Indent();
	for (const auto& [id, record] : aggregatedRecords)
	{
		ImGui::Text(std::format("{}: {:.3}ms", record.name, record.duration).c_str());
	}
	ImGui::Unindent();
	ImGui::End();
}
