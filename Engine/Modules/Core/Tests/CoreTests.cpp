#include <simpleTests.h>

#ifndef MANI_WEBGL
extern "C" __declspec(dllexport) void runTests()
{
	SimpleTests::SimpleTestsRunner::runTests();
}
#endif