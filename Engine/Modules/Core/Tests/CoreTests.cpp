#include <simpleTests.h>

extern "C" __declspec(dllexport) void runTests()
{
	SimpleTests::SimpleTestsRunner::runTests();
}