#include "ManImGuiECSSystem.h"

#include <ManImGui/ManImGui.h>
#include <ManImGui/ManImGuiSystem.h>
#include <ManImGui/ManImGuiManifoldMenuSystem.h>

#include <sstream>
#include <string>

#include "imgui.h"

constexpr std::string_view ECS_NAME = "ECS";

using namespace Mani;

#if MANI_DEBUG
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

bool queryMatches(const List<std::string>& tokens, const ECS::Registry& registry, const ECS::Entity& entity, bool shouldMatchAll)
{
    bool result = shouldMatchAll;
    for (const std::string_view& token : tokens)
    {
        const ECS::ComponentId componentId = registry.reflect(token);
        if (shouldMatchAll)
        {
            result &= entity.hasComponent(componentId);
            if (!result)
            {
                return false;
            }
        }
        else
        {
            result |= entity.hasComponent(componentId);
            if (result)
            {
                return true;
            }
        }
    }

    return result;
}
#endif

void Mani::ManImGuiECSSystem::onInitialize(ECS::Registry& registry, World& world)
{
	world.initializeDependency<ManImGuiSystem>();
	world.initializeDependency<ManImGuiManifoldMenuSystem>();

	{
		const ECS::EntityId debugMenuId = ManImGuiStatics::ManifoldMenu::getEntityId(registry);
		ManImGuiStatics::Menu::addItem(registry, debugMenuId, ECS_NAME);
	}
}

bool Mani::ManImGuiECSSystem::shouldTick(const ECS::Registry& registry) const
{
    if (!ManImGuiStatics::isShowing(registry))
    {
        return false;
    }

    const ECS::EntityId debugMenuId = ManImGuiStatics::ManifoldMenu::getEntityId(registry);
    return ManImGuiStatics::Menu::isOpened(registry, debugMenuId, ECS_NAME);
}

void Mani::ManImGuiECSSystem::tick(ECS::Registry& registry)
{
	bool isOpened = true;
	if (!ImGui::Begin("ECS", &isOpened, ImGuiWindowFlags_MenuBar))
	{
		ImGui::End();
		return;
	}

	if (!isOpened)
	{
        const ECS::EntityId debugMenuId = ManImGuiStatics::ManifoldMenu::getEntityId(registry);
		ManImGuiStatics::Menu::close(registry, debugMenuId, ECS_NAME);
		ImGui::End();
		return;
	}

    static char searchBuffer[512] = "";
    static bool allowLooseMatch = true;
    static bool matchesAll = false;
    ImGui::Checkbox("Allow loose type", &allowLooseMatch);
    if (!allowLooseMatch)
    {
        ImGui::SameLine();
        ImGui::Checkbox("Matches all", &matchesAll);
    }

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

    constexpr ImGuiTableFlags flags = ImGuiTableFlags_::ImGuiTableFlags_SizingFixedSame | ImGuiTableFlags_::ImGuiTableFlags_Borders;
    // Optionally: make columns for readability
    if (ImGui::BeginTable("ECS_Entities", 3, flags))
    {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.2f, 0.3f, 0.4f, 0.4f));

        ImGui::TableSetupColumn("Entity");
        ImGui::TableSetupColumn("Index");
        ImGui::TableSetupColumn("Version", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableHeadersRow();

#if MANI_DEBUG
        ECS::View<> view(registry);
        for (const auto entityId : view)
        {
            const ECS::Entity& entity = *registry.getEntity(entityId);
            const List<ECS::TypeInfo> typeInfo = registry.getTypeInfo(entity);

            const bool isEmpty = searchTokens.isEmpty();
            const bool matches = isEmpty || (allowLooseMatch
                ? isLooseMatch(searchTokens, typeInfo)
                : queryMatches(searchTokens, registry, entity, matchesAll));
            
            if (!matches)
            {
                // didn't pass the search filter.
                continue;
            }

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);

            // Collapsing header per entity
            std::string header = std::format("Entity {}", entityId);
            bool open = ImGui::TreeNodeEx(header.c_str(), ImGuiTreeNodeFlags_SpanFullWidth);

            ImGui::TableSetColumnIndex(1);
            ImGui::Text("index[%u]", entity.getIndex());
            ImGui::TableSetColumnIndex(2);
            ImGui::Text("version[%u]", entity.getVersion());

            if (open)
            {
                if (ImGui::BeginTable("ECS_Entities_Components", 2, ImGuiTableFlags_BordersInnerV))
                {
                    ImGui::TableSetupColumn("Component Name");
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
        }
#endif

        ImGui::PopStyleColor();
        ImGui::PopStyleVar();

        ImGui::EndTable();
    }

	ImGui::End();
}
