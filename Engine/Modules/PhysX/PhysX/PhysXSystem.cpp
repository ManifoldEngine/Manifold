#include "PhysXSystem.h"

#include <PhysX/PhysXMaths.h>
#include <utility>

using namespace Mani;
using namespace physx;

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
}

void PhysXSystem::onDeinitialize(EntityRegistry& registry)
{
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
	m_scene->simulate(deltaTime);
	m_scene->fetchResults(true);

	RegistryView<Transform, PhysXDynamicBoxComponent> dynamicBoxView(registry);
	for (const EntityId entityId : dynamicBoxView)
	{
		const auto it = m_actors.find(entityId);
		if (it == m_actors.end())
		{
			// body is not registered.
			continue;
		}
		
		const PxRigidActor* rigidActor = it->second;
		if (rigidActor == nullptr)
		{
			continue;
		}

		const PxTransform pxTransform = rigidActor->getGlobalPose();
		Transform* transform = registry.getComponent<Transform>(entityId);
		transform->position = PhysXMaths::toglmVec3(pxTransform.p);
		transform->rotation = PhysXMaths::toglmQuat(pxTransform.q);
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
	
	const bool result = createStaticRigidStatic(entityId, pxTransform, *boxShape);
	boxShape->release();
	return result;
}

bool PhysXSystem::registerStaticSphereComponent(EntityId entityId, const PhysXStaticSphereComponent& sphereComponent, const Transform& transform)
{
	// todo: this scale factor is lowkey sus.
	const float scaleFactor = std::max(std::max(transform.scale.x, transform.scale.y), transform.scale.z);
	const float radius = sphereComponent.radius * scaleFactor;
	PxShape* sphereShape = m_physics->createShape(PxSphereGeometry(radius), *m_defaultMaterial);
	if (sphereShape == nullptr)
	{
		return false;
	}

	const PxTransform pxTransform = PhysXMaths::toPxTransform(transform);

	const bool result = createStaticRigidStatic(entityId, pxTransform, *sphereShape);
	sphereShape->release();
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

	const bool result = createDynamicRigidStatic(entityId, pxTransform, *boxShape, boxComponent);
	boxShape->release();
	return result;
}

bool PhysXSystem::registerDynamicSphereComponent(EntityId entityId, const PhysXDynamicSphereComponent& sphereComponent, const Transform& transform)
{
	// todo: this scale factor is lowkey sus.
	const float scaleFactor = std::max(std::max(transform.scale.x, transform.scale.y), transform.scale.z);
	const float radius = sphereComponent.radius * scaleFactor;
	PxShape* sphereShape = m_physics->createShape(PxSphereGeometry(radius), *m_defaultMaterial);
	if (sphereShape == nullptr)
	{
		return false;
	}

	const PxTransform pxTransform = PhysXMaths::toPxTransform(transform);

	const bool result = createDynamicRigidStatic(entityId, pxTransform, *sphereShape, sphereComponent);
	sphereShape->release();
	return result;
}

bool PhysXSystem::createStaticRigidStatic(EntityId entityId, const physx::PxTransform& transform, PxShape& shape)
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

bool PhysXSystem::createDynamicRigidStatic(EntityId entityId, const physx::PxTransform& transform, PxShape& shape, const PhysXDynamicComponent& dynamicComponent)
{
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
