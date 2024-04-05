#include <Core/CoreFwd.h>

#include <Assets/AssetSystem.h>

#include <Inputs/InputSystem.h>
#include <FloatingCamera/FloatingCameraControllerSystem.h>

#include <RenderAPI/Mesh.h>
#include <RenderAPI/Material.h>
#include <RenderAPI/MeshComponent.h>
#include <RenderAPI/Light/DirectionalLightComponent.h>

#include <OpenGL/OpenGLSystem.h>
#include <OpenGL/Render/OpenGLRenderSystem.h>

#include <PhysX/PhysXSystem.h>

using namespace Mani;

class TestSystem : public SystemBase
{
	std::string_view getName() const { return "TestSystem"; }

	void onInitialize(EntityRegistry& registry, SystemContainer& systemContainer)
	{
		// initialize inputs
		std::shared_ptr<OpenGLSystem> openGLSystem = Application::get().getSystemContainer().getSystem<OpenGLSystem>().lock();
		std::shared_ptr<InputSystem> inputSystem = systemContainer.initializeDependency<InputSystem>().lock();
		inputSystem->assignInputGenerator(LOCAL_USERID, openGLSystem->getInputGenerator());


		std::shared_ptr<AssetSystem> assetSystem = systemContainer.initializeDependency<AssetSystem>().lock();
		std::shared_ptr<PhysXSystem> physxSystem = systemContainer.initializeDependency<PhysXSystem>().lock();

		{
			// light
			EntityId sunEntityId = registry.create();

			MeshComponent* meshComponent = registry.addComponent<MeshComponent>(sunEntityId);
			meshComponent->mesh = assetSystem->loadJsonAsset<Mesh>("Sandbox/Assets/Meshes/Sphere.mesh");
			meshComponent->material = assetSystem->loadJsonAsset<Material>("Sandbox/Assets/Materials/light.material");

			Transform* transform = registry.addComponent<Transform>(sunEntityId);
			transform->position = glm::vec3(500.f, 500.0f, 500.0f);
			transform->scale = glm::vec3(50.0f, 50.0f, 50.0f);

			DirectionalLightComponent* light = registry.addComponent<DirectionalLightComponent>(sunEntityId);
			light->ambient = glm::vec3(.1f, .1f, .01f);
			light->diffuse = glm::vec3(.9f, .9f, .8f);
			light->specular = glm::vec3(.9f, .9f, .8f);
			light->direction = glm::normalize(-transform->position);
		}

		{
			// floor
			const EntityId floorEntityId = registry.create();

			Transform* transform = registry.addComponent<Transform>(floorEntityId);
			transform->scale = glm::vec3(50.f, .2f, 50.f);


			MeshComponent* meshComponent = registry.addComponent<MeshComponent>(floorEntityId);
			meshComponent->mesh = assetSystem->loadJsonAsset<Mesh>("Sandbox/Assets/Meshes/Cube.mesh");
			meshComponent->material = assetSystem->loadJsonAsset<Material>("Sandbox/Assets/Materials/floor.material");

			PhysXStaticBoxComponent* boxComponent = registry.addComponent<PhysXStaticBoxComponent>(floorEntityId);
			boxComponent->extent = glm::vec3(1.f);
			physxSystem->registerStaticBoxComponent(floorEntityId, *boxComponent, *transform);
		}

		{
			// cube
			const EntityId cubeEntityId = registry.create();

			Transform* transform = registry.addComponent<Transform>(cubeEntityId);
			transform->position = glm::vec3(0.f, 50.f, 0.f);
			transform->scale = glm::vec3(3.f, 3.f, 3.f);


			MeshComponent* meshComponent = registry.addComponent<MeshComponent>(cubeEntityId);
			meshComponent->mesh = assetSystem->loadJsonAsset<Mesh>("Sandbox/Assets/Meshes/Cube.mesh");
			meshComponent->material = assetSystem->loadJsonAsset<Material>("Sandbox/Assets/Materials/redLit.material");

			PhysXDynamicBoxComponent* boxComponent = registry.addComponent<PhysXDynamicBoxComponent>(cubeEntityId);
			boxComponent->extent = glm::vec3(1.f);
			physxSystem->registerDynamicBoxComponent(cubeEntityId, *boxComponent, *transform);
		}
	}

	void onDeinitialize(EntityRegistry& registry)
	{

	}
};

int main(int argc, char** argv)
{
	Application app;
	SystemContainer& appSystemContainer = app.getSystemContainer();
	appSystemContainer.createSystem<OpenGLSystem>();
	std::shared_ptr<WorldSystem> worldSystem = appSystemContainer.initializeDependency<WorldSystem>().lock();

	std::shared_ptr<World> world = worldSystem->createWorld();
	
	SystemContainer& worldSystemContainer = world->getSystemContainer();
	worldSystemContainer.createSystem<OpenGLRenderSystem>()
						.createSystem<PhysXSystem>()
						.createSystem<FloatingCameraControllerSystem>()
						.createSystem<TestSystem>();


	app.run();

	return EXIT_SUCCESS;
}