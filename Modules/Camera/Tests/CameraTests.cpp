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
			const BoundingSphere boundingSphere = { .radius = Math::sqrt(2.f) };

			Camera camera;
			camera.height = 128.f;
			camera.width = 128.f;
			camera.mode = ECameraMode::Orthographic;
			camera.pixelsPerUnit = 32;
			camera.frustrum = FrustumStatics::create(camera, position, rotation);

			{
				constexpr Position entityPosition = Position{ VEC3F::ZERO };
				constexpr Rotation entityRotation = Rotation{ QUATF::IDENTITY };
				constexpr Scale entityScale = Scale{ VEC3F::ONE };
				
				const bool expected = true;
				const bool result = CameraStatics::isInView(camera, entityPosition, entityRotation, entityScale, boundingSphere);
				MANI_TEST_ASSERT(result == expected, "Should be in view");
			}

			{
				constexpr Position entityPosition = Position{ { 1000.f, 1000.f, 0.f} };
				constexpr Rotation entityRotation = Rotation{ QUATF::IDENTITY };
				constexpr Scale entityScale = Scale{ VEC3F::ONE };

				const bool expected = false;
				const bool result = CameraStatics::isInView(camera, entityPosition, entityRotation, entityScale, boundingSphere);
				MANI_TEST_ASSERT(result == expected, "Should not be in view");
			}

			{
				// screen should be 4 times larger than the diameter, so this offset should be barely out of view
				const Position entityPosition = Position{ { 2.0f, 0.f, 0.f } };
				constexpr Rotation entityRotation = Rotation{ QUATF::IDENTITY };
				constexpr Scale entityScale = Scale{ VEC3F::ONE };

				const bool expected = true;
				const bool result = CameraStatics::isInView(camera, entityPosition, entityRotation, entityScale, boundingSphere);
				MANI_TEST_ASSERT(result == expected, "Should be in view");
			}

			{
				// screen should be 4 times larger than the diameter, so this offset should be barely out of view
				const Position entityPosition = Position{ { 3.50f, 0.f, 0.f } };
				constexpr Rotation entityRotation = Rotation{ QUATF::IDENTITY };
				constexpr Scale entityScale = Scale{ VEC3F::ONE };

				const bool expected = false;
				const bool result = CameraStatics::isInView(camera, entityPosition, entityRotation, entityScale, boundingSphere);
				MANI_TEST_ASSERT(result == expected, "Should not be in view");
			}
		}
	}
	MANI_SECTION_END(Orthographic)
}
MANI_SECTION_END(CameraTests)