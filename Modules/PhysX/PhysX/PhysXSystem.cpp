#include "PhysXSystem.h"

#include <PhysX/PhysXMaths.h>
#include <RenderAPI/Mesh.h>

#include <vector>
#include <utility>

using namespace Mani;
using namespace physx;

constexpr float PHYSX_FIXED_DELTA_TIME = 1.f / 60.f;

std::string_view PhysXSystem::getName() const
{
	return "PhysXSystem";
}

bool PhysXSystem::shouldTick(EntityRegistry& registry) const
{
	return true;
}

ETickGroup PhysXSystem::getTickGroup() const
{
	return ETickGroup::PostTick;
}

void PhysXSystem::onInitialize(EntityRegistry& registry, SystemContainer& systemContainer)
{
	m_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_allocator, m_errorCallback);

	m_pvd = PxCreatePvd(*m_foundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	m_pvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

	m_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_foundation, PxTolerancesScale(), true, m_pvd);

	PxSceneDesc sceneDesc(m_physics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	m_dispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = m_dispatcher;
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	m_scene = m_physics->createScene(sceneDesc);

	PxPvdSceneClient* pvdClient = m_scene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

	m_defaultMaterial = m_physics->createMaterial(0.5f, 0.5f, 0.6f);

	onComponentRemovedHandle = registry.onComponentRemoved.subscribe(std::bind(&PhysXSystem::onComponentRemoved, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	onEntityDestroyedHandle = registry.onEntityDestroyed.subscribe(std::bind(&PhysXSystem::onEntityDestroyed, this, std::placeholders::_1, std::placeholders::_2));
}

void PhysXSystem::onDeinitialize(EntityRegistry& registry)
{
	registry.onComponentRemoved.unsubscribe(onComponentRemovedHandle);
	registry.onEntityDestroyed.unsubscribe(onEntityDestroyedHandle);

	PX_RELEASE(m_scene);
	PX_RELEASE(m_dispatcher);
	PX_RELEASE(m_physics);
	if (m_pvd)
	{
		PxPvdTransport* transport = m_pvd->getTransport();
		m_pvd->release();
		m_pvd = NULL;
		PX_RELEASE(transport);
	}
	PX_RELEASE(m_foundation);
}

void PhysXSystem::tick(float deltaTime, EntityRegistry& registry)
{
	m_accumulatedTime += deltaTime;
	while (m_accumulatedTime >= PHYSX_FIXED_DELTA_TIME)
	{
		m_scene->simulate(PHYSX_FIXED_DELTA_TIME);
		m_scene->fetchResults(true);
		m_accumulatedTime -= PHYSX_FIXED_DELTA_TIME;
	}

	const auto updatePhysXEntity = [&](const EntityId entityId)
	{
		const auto it = m_actors.find(entityId);
		if (it == m_actors.end())
		{
			// body is not registered.
			return;
		}

		const PxRigidActor* rigidActor = it->second;
		if (rigidActor == nullptr)
		{
			return;
		}

		const PxTransform pxTransform = rigidActor->getGlobalPose();
		Transform* transform = registry.getComponent<Transform>(entityId);
		transform->position = PhysXMaths::toglmVec3(pxTransform.p);
		transform->rotation = PhysXMaths::toglmQuat(pxTransform.q);
	};

	RegistryView<Transform, PhysXDynamicBoxComponent> dynamicBoxView(registry);
	for (const EntityId entityId : dynamicBoxView)
	{
		updatePhysXEntity(entityId);
	}

	RegistryView<Transform, PhysXDynamicSphereComponent> dynamicSphereView(registry);
	for (const EntityId entityId : dynamicSphereView)
	{
		updatePhysXEntity(entityId);
	}

	RegistryView<Transform, PhysXDynamicMeshComponent> dynamicMeshView(registry);
	for (const EntityId entityId : dynamicMeshView)
	{
		updatePhysXEntity(entityId);
	}
}

bool PhysXSystem::registerStaticBoxComponent(EntityId entityId, const PhysXStaticBoxComponent& boxComponent, const Transform& transform)
{
	const glm::vec3 extent = boxComponent.extent * transform.scale;
	PxShape* boxShape = m_physics->createShape(PxBoxGeometry(extent.x, extent.y, extent.z), *m_defaultMaterial);
	if (boxShape == nullptr)
	{
		return false;
	}

	const PxTransform pxTransform = PhysXMaths::toPxTransform(transform);
	
	const bool result = createStaticRigidActor(entityId, pxTransform, *boxShape);
	boxShape->release();
	return result;
}

bool PhysXSystem::registerStaticSphereComponent(EntityId entityId, const PhysXStaticSphereComponent& sphereComponent, const Transform& transform)
{
	const float scaleFactor = std::max(std::max(transform.scale.x, transform.scale.y), transform.scale.z);
	const float radius = sphereComponent.radius * scaleFactor;
	PxShape* sphereShape = m_physics->createShape(PxSphereGeometry(radius), *m_defaultMaterial);
	if (sphereShape == nullptr)
	{
		return false;
	}

	const PxTransform pxTransform = PhysXMaths::toPxTransform(transform);

	const bool result = createStaticRigidActor(entityId, pxTransform, *sphereShape);
	sphereShape->release();
	return result;
}

bool PhysXSystem::registerStaticMeshComponent(EntityId entityId, const PhysXStaticMeshComponent& meshComponent, const Transform& transform)
{
	PxConvexMesh* pxMesh = createPxConvexMesh(meshComponent.mesh);
	if (pxMesh == nullptr)
	{
		return false;
	}

	PxShape* shape = m_physics->createShape(PxConvexMeshGeometry(pxMesh, PxMeshScale(PhysXMaths::toPxVec3(transform.scale))), *m_defaultMaterial);
	if (shape == nullptr)
	{
		pxMesh->release();
		return false;
	}

	const PxTransform pxTransform = PhysXMaths::toPxTransform(transform);

	const bool result = createStaticRigidActor(entityId, pxTransform, *shape);
	shape->release();
	pxMesh->release();
	return result;
}

bool PhysXSystem::registerDynamicBoxComponent(EntityId entityId, const PhysXDynamicBoxComponent& boxComponent, const Transform& transform)
{
	const glm::vec3 extent = boxComponent.extent * transform.scale;
	PxShape* boxShape = m_physics->createShape(PxBoxGeometry(extent.x, extent.y, extent.z), *m_defaultMaterial);
	if (boxShape == nullptr)
	{
		return false;
	}

	const PxTransform pxTransform = PhysXMaths::toPxTransform(transform);

	const bool result = createDynamicRigidActor(entityId, pxTransform, *boxShape, boxComponent);
	boxShape->release();
	return result;
}

bool PhysXSystem::registerDynamicSphereComponent(EntityId entityId, const PhysXDynamicSphereComponent& sphereComponent, const Transform& transform)
{
	const float scaleFactor = std::max(std::max(transform.scale.x, transform.scale.y), transform.scale.z);
	const float radius = sphereComponent.radius * scaleFactor;
	PxShape* sphereShape = m_physics->createShape(PxSphereGeometry(radius), *m_defaultMaterial);
	if (sphereShape == nullptr)
	{
		return false;
	}

	const PxTransform pxTransform = PhysXMaths::toPxTransform(transform);

	const bool result = createDynamicRigidActor(entityId, pxTransform, *sphereShape, sphereComponent);
	sphereShape->release();
	return result;
}

bool PhysXSystem::registerDynamicMeshComponent(EntityId entityId, const PhysXDynamicMeshComponent& meshComponent, const Transform& transform)
{
	PxConvexMesh* pxMesh = createPxConvexMesh(meshComponent.mesh);
	if (pxMesh == nullptr)
	{
		return false;
	}

	PxShape* shape = m_physics->createShape(PxConvexMeshGeometry(pxMesh, PxMeshScale(PhysXMaths::toPxVec3(transform.scale))), *m_defaultMaterial);
	if (shape == nullptr)
	{
		pxMesh->release();
		return false;
	}

	const PxTransform pxTransform = PhysXMaths::toPxTransform(transform);

	const bool result = createDynamicRigidActor(entityId, pxTransform, *shape, meshComponent);
	shape->release();
	pxMesh->release();
	return result;
}

bool PhysXSystem::createStaticRigidActor(EntityId entityId, const physx::PxTransform& transform, PxShape& shape)
{
	PxRigidStatic* body = m_physics->createRigidStatic(transform);
	if (body == nullptr)
	{
		return false;
	}

	if (!body->attachShape(shape))
	{
		return false;
	}

	m_scene->addActor(*body);

	m_actors[entityId] = body;
	return true;
}

bool PhysXSystem::createDynamicRigidActor(EntityId entityId, const physx::PxTransform& transform, PxShape& shape, const PhysXDynamicComponent& dynamicComponent)
{
	if (m_actors.contains(entityId))
	{
		// for now we only allow 1 collision per entity.
		return false;
	}

	PxRigidDynamic* body = m_physics->createRigidDynamic(transform);
	if (body == nullptr)
	{
		return false;
	}

	if (!body->attachShape(shape))
	{
		return false;
	}

	body->setAngularDamping(dynamicComponent.angularDamping);
	if (!PxRigidBodyExt::updateMassAndInertia(*body, dynamicComponent.mass))
	{
		return false;
	}

	m_scene->addActor(*body);
	m_actors[entityId] = body;
	return true;
}

PxConvexMesh* PhysXSystem::createPxConvexMesh(const std::shared_ptr<Mesh>& mesh)
{
	if (mesh == nullptr)
	{
		return nullptr;
	}

	PxTolerancesScale tolerances;
	PxCookingParams params(tolerances);
	params.convexMeshCookingType = PxConvexMeshCookingType::eQUICKHULL;
	params.gaussMapLimit = 16;

	PxConvexMeshDesc meshDescription;
	std::vector<PxVec3> pxVertices;
	for (const auto& vertex : mesh->vertices)
	{
		pxVertices.push_back(PhysXMaths::toPxVec3(vertex.position));
	}

	if (pxVertices.empty())
	{
		return nullptr;
	}

	meshDescription.points.data = &pxVertices[0];
	meshDescription.points.count = static_cast<PxU32>(pxVertices.size());
	meshDescription.points.stride = sizeof(PxVec3);
	meshDescription.flags = PxConvexFlag::eCOMPUTE_CONVEX;

	PxConvexMesh* pxMesh = PxCreateConvexMesh(params, meshDescription, m_physics->getPhysicsInsertionCallback());
	MANI_ASSERT(pxMesh != nullptr, "failed to create PxConvexMesh");
	return pxMesh;
}

void PhysXSystem::onComponentRemoved(EntityRegistry& registry, EntityId entityId, ComponentId componentId)
{
	checkForDestroyedActors(entityId);
}

void PhysXSystem::onEntityDestroyed(EntityRegistry& registry, EntityId entityId)
{
	checkForDestroyedActors(entityId);
}

void PhysXSystem::checkForDestroyedActors(EntityId entityId)
{
	auto it = m_actors.find(entityId);
	if (it == m_actors.end())
	{
		return;
	}

	PxRigidActor* actor = it->second;
	actor->release();
	m_actors.erase(entityId);
}
