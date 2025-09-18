#include <ManiTests/ManiTests.h>
#include <Camera/Camera.h>

#ifndef MANI_WEBGL
extern "C" __declspec(dllexport) void runTests()
{
	ManiTests::ManiTestsRunner::runTests();
}
#endif

using namespace Mani;

MANI_SECTION_BEGIN(CameraTests, "Camera")
{
	MANI_SECTION_BEGIN(Orthographic, "Orthographic")
	{
		MANI_TEST(Frustum, "should create a frustum and detect if we're in view properly")
		{
			constexpr Vec3f position = VEC3F::FORWARD * -5.f;
			constexpr Quatf rotation = QUATF::IDENTITY;

			// assuming a sprite that's 32x32 with a tpu of 32
			BoundingSphere boundingSphere = { .radius = 1.f };

			Camera camera;
			camera.height = 100.f;
			camera.width = 100.f;
			camera.mode = ECameraMode::ORTHOGRAPHIC;
			camera.orthographicZoomFactor = 1.f;
			camera.pixelsPerUnit = 32;
			camera.frustrum = FrustumStatics::create(camera, position, rotation);

			{
				const bool expected = true;
				
				const bool result = CameraStatics::isInView(camera, VEC3F::ZERO, rotation, VEC3F::ONE, boundingSphere);
				MANI_TEST_ASSERT(result == expected, "Should be in view");
			}
		}
	}
	MANI_SECTION_END(Orthographic)
}
MANI_SECTION_END(CameraTests)