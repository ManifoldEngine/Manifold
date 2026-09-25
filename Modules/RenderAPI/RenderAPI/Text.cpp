#include "Text.h"

#include <RenderAPI/Components/TextRendering.h>
#include <RenderAPI/Components/MeshRendering.h>
#include <RenderAPI/RendererIds.h>

using namespace Mani;

void Text::addRendering(ECS::Registry& registry, EntityId entityId, EntityId fontId, EntityId materialId, float height, std::string_view text)
{
	Ref<TextRendering> textRendering = registry.add<TextRendering>(entityId);
	textRendering->fontId = fontId;
	textRendering->height = height;
	textRendering->text = text;

	Ref<MeshRendering> meshRendering = registry.add<MeshRendering>(entityId);
	meshRendering->rendererId = Mani::RendererIds::WORLD_TEXT;
	meshRendering->materialResourceId = materialId;
}
