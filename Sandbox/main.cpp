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

//#include <PhysX/PhysXSystem.h>

using namespace Mani;

class TestSystem : public SystemBase
{
public:
	virtual std::string_view getName() const override { return "TestSystem"; }

protected:
	virtual void onInitialize(EntityRegistry& registry, SystemContainer& systemContainer) override
	{
		// initialize inputs
		std::shared_ptr<OpenGLSystem> openGLSystem = Application::get().getSystemContainer().getSystem<OpenGLSystem>().lock();
		std::shared_ptr<InputSystem> inputSystem = systemContainer.initializeDependency<InputSystem>().lock();
		inputSystem->assignInputGenerator(LOCAL_USERID, openGLSystem->getInputGenerator());


		std::shared_ptr<AssetSystem> assetSystem = systemContainer.initializeDependency<AssetSystem>().lock();
		//std::shared_ptr<PhysXSystem> physxSystem = systemContainer.initializeDependency<PhysXSystem>().lock();

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
			transform->rotation = glm::angleAxis(glm::radians(10.f), glm::vec3(1.f, 0.f, 1.f));
			transform->scale = glm::vec3(50.f, .2f, 50.f);


			MeshComponent* meshComponent = registry.addComponent<MeshComponent>(floorEntityId);
			meshComponent->mesh = assetSystem->loadJsonAsset<Mesh>("Sandbox/Assets/Meshes/Cube.mesh");
			meshComponent->material = assetSystem->loadJsonAsset<Material>("Sandbox/Assets/Materials/floor.material");

			/*PhysXStaticBoxComponent* boxComponent = registry.addComponent<PhysXStaticBoxComponent>(floorEntityId);
			boxComponent->extent = glm::vec3(1.f);
			physxSystem->registerStaticBoxComponent(floorEntityId, *boxComponent, *transform);*/
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

			/*PhysXDynamicBoxComponent* boxComponent = registry.addComponent<PhysXDynamicBoxComponent>(cubeEntityId);
			boxComponent->extent = glm::vec3(1.f);
			physxSystem->registerDynamicBoxComponent(cubeEntityId, *boxComponent, *transform);*/
		}

		{
			// sphere
			const EntityId sphereEntityId = registry.create();

			Transform* transform = registry.addComponent<Transform>(sphereEntityId);
			transform->position = glm::vec3(0.f, 60.f, 0.f);
			transform->scale = glm::vec3(3.f, 3.f, 3.f);

			MeshComponent* meshComponent = registry.addComponent<MeshComponent>(sphereEntityId);
			meshComponent->mesh = assetSystem->loadJsonAsset<Mesh>("Sandbox/Assets/Meshes/Sphere.mesh");
			meshComponent->material = assetSystem->loadJsonAsset<Material>("Sandbox/Assets/Materials/redLit.material");

			/*PhysXDynamicSphereComponent* sphereComponent = registry.addComponent<PhysXDynamicSphereComponent>(sphereEntityId);
			sphereComponent->radius = 1.f;
			physxSystem->registerDynamicSphereComponent(sphereEntityId, *sphereComponent, *transform);*/
		}

		{
			// cactus
			const EntityId cactusEntityId = registry.create();

			Transform* transform = registry.addComponent<Transform>(cactusEntityId);
			transform->position = glm::vec3(-10.f, 50.f, -10.f);
			transform->scale = glm::vec3(3.f, 3.f, 3.f);

			MeshComponent* meshComponent = registry.addComponent<MeshComponent>(cactusEntityId);
			meshComponent->mesh = assetSystem->loadJsonAsset<Mesh>("Sandbox/Assets/Meshes/Cactus.mesh");
			meshComponent->material = assetSystem->loadJsonAsset<Material>("Sandbox/Assets/Materials/cactus.material");

			/*PhysXDynamicMeshComponent* physxMeshComponent = registry.addComponent<PhysXDynamicMeshComponent>(cactusEntityId);
			physxMeshComponent->mesh = meshComponent->mesh;
			const bool result = physxSystem->registerDynamicMeshComponent(cactusEntityId, *physxMeshComponent, *transform);
			MANI_ASSERT(result, "could not register dynamic mesh");*/
		}
	}

	virtual void onDeinitialize(EntityRegistry& registry) override
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
						//.createSystem<PhysXSystem>()
						.createSystem<FloatingCameraControllerSystem>()
						.createSystem<TestSystem>();

	app.run();

	return EXIT_SUCCESS;
}