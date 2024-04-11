#include <simpleTests.h>

#ifndef __EMSCRIPTEN__
extern "C" __declspec(dllexport) void runTests()
{
	SimpleTests::SimpleTestsRunner::runTests();
}
#endif