#include "simpleTests.h"

extern "C" __declspec(dllexport) void runTests()
{
	SimpleTests::SimpleTestsRunner::runTests();
}

ST_SECTION_BEGIN(PhysX, "PhysX")
{
}
ST_SECTION_END(PhysX)