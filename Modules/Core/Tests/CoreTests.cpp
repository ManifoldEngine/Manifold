#include <ManiTests/ManiTests.h>
#include <Core/ManiTests.h>

#ifndef MANI_WEBGL
extern "C" __declspec(dllexport) void runTests()
{
	Mani::runTests();
}
#endif