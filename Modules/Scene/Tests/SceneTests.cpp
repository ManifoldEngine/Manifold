#include <ManiTests/ManiTests.h>

extern "C" __declspec(dllexport) void runTests()
{
	ManiTests::ManiTestsRunner::runTests();
}

MANI_SECTION_BEGIN(Scene, "Scene")
{
}
MANI_SECTION_END(Scene)