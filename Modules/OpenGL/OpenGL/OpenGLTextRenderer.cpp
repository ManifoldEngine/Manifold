#include "OpenGLTextRenderer.h"

#include <Core/ManiAssert.h>
#include <Core/Profiling/Profiling.h>

#include <OpenGL/Resources/OpenGLVertexArray.h>
#include <OpenGL/Resources/OpenGLShader.h>

#include <OpenGL/Render/OpenGLCommand.h>
#include <OpenGL/Render/OpenGLRenderContext.h>
#include <OpenGL/Render/OpenGLRenderSystem.h>

#include <RenderAPI/Primitives.h>
#include <RenderAPI/Shader.h>
#include <RenderAPI/RendererIds.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>
#include <STB/Fonts/STBTruetype.h>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

using namespace Mani;

int OpenGLTextRenderer::getId() const
{
	return RendererIds::WORLD_TEXT;
}

void OpenGLTextRenderer::onBegin(OpenGLRenderContext& context)
{
	//glEnable(GL_DEPTH_TEST);
	//glEnable(GL_BLEND);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void OpenGLTextRenderer::render(const OpenGLCommand& command, OpenGLRenderContext& context)
{
	if (command.text.value.empty())
	{
		return;
	}

	MANI_ASSERT(command.text.font != nullptr, "Trying to render text with a nullptr font");

	MANI_ASSERT(command.shader != nullptr, "Resource was unloaded unexpectedly.");

	Mat4f model = command.model;
	const Vec3f direction = (model * Mani::VEC3F::RIGHT).normalize();
	const OpenGLShader& shader = *command.shader;

	const Mat3f normalMatrix = static_cast<Mat3f>(model).inverse().transpose();

	shader.use();
	// set vertex uniforms
	shader.setFloatMatrix4(ShaderNames::MANI_MODEL, &(model._00));
	shader.setFloatMatrix3(ShaderNames::MANI_NORMALMATRIX, &(normalMatrix._00));
	shader.setFloatMatrix4(ShaderNames::MANI_VIEW, &(context.view._00));
	shader.setFloatMatrix4(ShaderNames::MANI_PROJECTION, &(context.projection._00));

	int textureIndex = 0;

	// TODO: hurts my feelings to be calculating this on the CPU.
	const Mani::Vec3f position = context.projection * context.view * command.model * VEC3F::ZERO;
	float cursorX = 0.f;
	
	const stbtt_fontinfo* fontInfo = &command.text.font->info;
	for (const char c : command.text.value)
	//const char c = 'A';
	{
		int width = 0;
		int height = 0;
		int offsetX = 0;
		int offsetY = 0;

		const float scale = stbtt_ScaleForPixelHeight(fontInfo, command.text.fontHeight);

		// creates the glyph's texture
		unsigned char* bitmap = stbtt_GetCodepointBitmap(
			fontInfo, scale, scale, c,
			&width, &height, &offsetX, &offsetY
		);

		// temp create texture 
		// create texture data
		GLuint textureId;
		glCreateTextures(GL_TEXTURE_2D, 1, &textureId);
		glTextureStorage2D(textureId, 1, GL_R8, width, height);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		// generate texture from image data.
		glTextureSubImage2D(
			textureId,
			0, // level
			0,
			0,
			width,
			height,
			GL_RED,
			GL_UNSIGNED_BYTE,
			bitmap
		);

		glTextureParameteri(textureId, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(textureId, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTextureParameteri(textureId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(textureId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glBindTextureUnit(textureIndex, textureId);
		shader.setTextureSlot(Mani::ShaderNames::MANI_TEXTURE_0, textureIndex);

		//const Mani::Mesh mesh = Mani::Primitives::makeQuad({ static_cast<float>(width), static_cast<float>(height) });
		//const Mani::Mesh& mesh = command.text.quad;
		const Mani::Mesh mesh = Mani::Primitives::makeQuad();
		OpenGLVertexArray vao;

		constexpr size_t vertexSize = 3 + 3 + 2;
		OpenGLVertexBuffer vertexBuffer;
		vertexBuffer.create(&mesh.vertices[0].position.x, (int)(sizeof(float) * (mesh.vertices.count() * vertexSize)));;
		vertexBuffer.layout =
		{
			{ EShaderDataType::Float3, false },
			{ EShaderDataType::Float3, true  },
			{ EShaderDataType::Float2, false }
		};

		OpenGLIndexBuffer indexBuffer;
		indexBuffer.create(&mesh.indices[0], (int)sizeof(uint32_t) * mesh.indices.count());

		vao.create();
		vao.addVertexBuffer(std::move(vertexBuffer));
		vao.setIndexBuffer(std::move(indexBuffer));

		vao.bind();
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(vao.getIndexBuffer().getStrideCount()), GL_UNSIGNED_INT, nullptr);

		int advanceWidth = 0;
		int leftBearing = 0;
		stbtt_GetCodepointHMetrics(fontInfo, c, &advanceWidth, &leftBearing);
		cursorX += advanceWidth * scale;
		stbtt_FreeBitmap(bitmap, nullptr);

		glBindTextureUnit(textureIndex, NULL);
		glDeleteTextures(1, &textureId);
		vao.destroy();

		model.translate(Mani::VEC3F::RIGHT * 1.f);
		shader.setFloatMatrix4(ShaderNames::MANI_MODEL, &(model._00));
	}
}

void OpenGLTextRenderer::onEnd(OpenGLRenderContext& context)
{
	glDisable(GL_BLEND);
	//glDisable(GL_DEPTH_TEST);
}

void OpenGLTextRendererSystem::onInitialize(ECS::Registry& registry, World& world)
{
	world.initializeDependency<OpenGLRenderSystem>();
	OpenGL::registerRenderer(registry, &m_renderer);
}

void OpenGLTextRendererSystem::onDeinitialize(ECS::Registry& registry, World& world)
{
	OpenGL::unregisterRenderer(registry, &m_renderer);
}
