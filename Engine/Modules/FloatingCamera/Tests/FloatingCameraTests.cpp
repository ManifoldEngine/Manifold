#include "simpleTests.h"

extern "C" __declspec(dllexport) void runTests()
{
	SimpleTests::SimpleTestsRunner::runTests();
}

ST_SECTION_BEGIN(FloatingCamera, "FloatingCamera")
{
}
ST_SECTION_END(FloatingCamera)