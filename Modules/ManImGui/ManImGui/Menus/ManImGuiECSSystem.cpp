#include "ManImGuiECSSystem.h"

#include <ManImGui/ManImGui.h>
#include <ManImGui/ManImGuiSystem.h>
#include <ManImGui/ManImGuiManifoldMenuSystem.h>

#include <sstream>
#include <string>

#include "imgui.h"

constexpr std::string_view ECS_NAME = "ECS";

using namespace Mani;

void getComponentsInfo(const ECS::Registry& registry, ECS::ComponentMask components, Mani::List<ECS::TypeInfo>& result)
{
    for (ECS::ComponentId componentId = 0; componentId < ECS::MAX_COMPONENTS; componentId++)
    {
        if (components.test(componentId))
        {
            result.add(registry.getMetadata().getInfo(componentId).getOr(ECS::TypeInfo{"UNKNOWN TYPE?", 0}));
        }
    }
}

bool isLooseMatch(const List<std::string>& tokens, const List<ECS::TypeInfo>& typeInfo)
{
    for (const std::string_view& token : tokens)
    {
        for (const ECS::TypeInfo& info : typeInfo)
        {
            if (info.name.find(token) != std::string::npos)
            {
                return true;
            }
        }
    }

    return false;
}

void DrawEntity(const Mani::ECS::Registry& registry, Mani::EntityId entityId, ECS::ComponentMask mask, const List<std::string>& searchTokens)
{
    constexpr ImGuiTableFlags TABLE_FLAGS = ImGuiTableFlags_::ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInnerV;
    constexpr int TABLE_COLUMS = 2;

    Mani::List<ECS::TypeInfo> typeInfo;
    getComponentsInfo(registry, mask, typeInfo);
    const bool isEmpty = searchTokens.isEmpty();
    const bool matches = isEmpty || isLooseMatch(searchTokens, typeInfo);

    if (!matches)
    {
        // didn't pass the search filter.
        return;
    }

    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);

    // Collapsing header per entity
    const std::string header = std::format("[{}:{}] {}", ECS::toIndex(entityId), ECS::toVersion(entityId), entityId);
    const bool open = ImGui::TreeNodeEx(header.c_str(), ImGuiTreeNodeFlags_SpanFullWidth);

    ImGui::TableSetColumnIndex(1);

    if (open)
    {
        if (ImGui::BeginTable("ECS_Entities_Components", TABLE_COLUMS, TABLE_FLAGS))
        {
            ImGui::TableSetupColumn("Component");
            ImGui::TableSetupColumn("Size");
            ImGui::TableHeadersRow();

            for (const ECS::TypeInfo& info : typeInfo)
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted(info.name.data(), info.name.data() + info.name.size());
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%u", info.size);
            }

            ImGui::EndTable();
        }
        ImGui::TreePop();
    }
#if MANI_DEBUG
    else
    {
        const ECS::Entity* entity = registry.getEntity(entityId);
        ImGui::Text(entity->debug_name.data());
    }
#endif
}

void ManImGuiECSSystem::onInitialize(ECS::Registry& registry, World& world)
{
	world.initializeDependency<ManImGuiSystem>();
	world.initializeDependency<ManImGuiManifoldMenuSystem>();

	{
		Ref<ManImGuiMenu> menu = ManImGui::Manifold::getMenu(registry);
        menu->subMenu.addItem(ECS_NAME);
	}
}

bool ManImGuiECSSystem::shouldTick(const ECS::Registry& registry) const
{
    if (!ManImGui::isShowing(registry))
    {
        return false;
    }

    Ref<const ManImGuiMenu> menu = ManImGui::Manifold::getMenu(registry);
    return menu->subMenu.getSelected(ECS_NAME);
}

void ManImGuiECSSystem::tick(ECS::Registry& registry)
{
	bool isOpened = true;
	if (!ImGui::Begin("ECS", &isOpened, ImGuiWindowFlags_MenuBar))
	{
		ImGui::End();
		return;
	}

	if (!isOpened)
	{
        Ref<ManImGuiMenu> menu = ManImGui::Manifold::getMenu(registry);
        menu->subMenu.setSelected(ECS_NAME, false);
		ImGui::End();
		return;
	}

    static char searchBuffer[512] = "";
    ImGui::InputTextWithHint("##search", "Search entity or component...", searchBuffer, sizeof(searchBuffer));
    
    List<std::string> searchTokens;
    {
        std::string s(searchBuffer);
        std::erase_if(s, [](unsigned char c) { return std::isspace(c); });
        std::stringstream ss(s);
        std::string token;
        while (std::getline(ss, token, ','))
        {
            searchTokens.add(token);
        }
    }

    constexpr ImGuiTableFlags TABLE_FLAGS= ImGuiTableFlags_::ImGuiTableFlags_Resizable | ImGuiTableFlags_::ImGuiTableFlags_Borders;
    constexpr int TABLE_COLUMS = 2;

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.2f, 0.3f, 0.4f, 0.4f));

    if (ImGui::CollapsingHeader("Archetypes"))
    {
        ImGui::BeginTable("ECS_ENTITIES_ARCHETYPES", TABLE_COLUMS, TABLE_FLAGS);
        ImGui::TableSetupColumn("Entities");
        ImGui::TableSetupColumn("Components");
        ImGui::TableHeadersRow();

        ECS::View<> view(registry);
        for (const auto [entityId] : view)
        {
            const ECS::Entity* entity = registry.getEntity(entityId);
            DrawEntity(registry, entityId, entity->components, searchTokens);
        }
        ImGui::EndTable();
    }

    if (ImGui::CollapsingHeader("Pinned Components"))
    {
        ImGui::BeginTable("ECS_ENTITIES_PINNED_COMPONENTS", TABLE_COLUMS, TABLE_FLAGS);
        ImGui::TableSetupColumn("Entities");
        ImGui::TableSetupColumn("Components");
        ImGui::TableHeadersRow();

        ECS::PinnedView<> view(registry);
        for (const auto [entityId] : view)
        {
            const ECS::Entity* entity = registry.getEntity(entityId);
            DrawEntity(registry, entityId, entity->pinned, searchTokens);
        }
        ImGui::EndTable();
    }

    ImGui::PopStyleColor();
    ImGui::PopStyleVar();

	ImGui::End();
}
