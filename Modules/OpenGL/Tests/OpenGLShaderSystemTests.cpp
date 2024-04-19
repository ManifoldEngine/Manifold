#include "simpleTests.h"
#include <Core/World/World.h>
#include <filesystem>
#include <Core/Application.h>
#include <Utils/StringUtils.h>

#ifndef MANI_WEBGL
extern "C" __declspec(dllexport) void runTests()
{
    SimpleTests::SimpleTestsRunner::runTests();
}
#endif

using namespace Mani;

ST_SECTION_BEGIN(OpenGL, "OpenGL")
{
    //ST_TEST(ParseShaderFile, "Should parse a shader file")
    //{
    //    Application app;
    //    auto path = std::filesystem::path(std::filesystem::current_path().string() + "/Assets/shader.glsl");
    //    std::string fileName, vertexSource, fragmentSource;
    //    const bool result = parseShaderSourceFileFromPath(path, fileName, vertexSource, fragmentSource);

    //    ST_ASSERT(result, "Should have succeeded");
    //    ST_ASSERT(fileName == "shader.glsl", "Filename should be equal");

    //    // Expected vertex and fragment shader sources
    //    const std::string expectedVertexSource = StringUtils::removeWhiteSpace("#version 330 corelayout(location = 0) in vec3 aPos;void main(){gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);}");
    //    const std::string expectedFragmentSource = StringUtils::removeWhiteSpace("#version 330 coreout vec4 FragColor;void main(){FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);}");

    //    ST_ASSERT(StringUtils::removeWhiteSpace(vertexSource) == expectedVertexSource, "Vertex source should match expected");
    //    ST_ASSERT(StringUtils::removeWhiteSpace(fragmentSource) == expectedFragmentSource, "Fragment source should match expected");
    //}
}
ST_SECTION_END(OpenGL)
