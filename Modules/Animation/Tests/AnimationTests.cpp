#include <ManiTests/ManiTests.h>
#include <Camera/Camera.h>

#ifndef MANI_WEBGL
extern "C" __declspec(dllexport) void runTests()
{
	ManiTests::ManiTestsRunner::runTests();
}
#endif

using namespace Mani;

MANI_SECTION_BEGIN(AnimationTests, "Camera")
{
}
MANI_SECTION_END(CameraTests)