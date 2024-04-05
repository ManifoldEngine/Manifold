#pragma once

#include <Core/CoreFwd.h>
#include <PhysX/PhysXComponents.h>

#include <PxPhysicsAPI.h>

#define PVD_HOST "127.0.0.1"

namespace Mani
{
	class PhysXSystem : public SystemBase
	{
	public:
		virtual std::string_view getName() const override;
		virtual bool shouldTick(EntityRegistry& registry) const override;
		virtual ETickGroup getTickGroup() const override;

		virtual void tick(float deltaTime, EntityRegistry& registry) override;

		bool registerStaticBoxComponent(EntityId entityId, const PhysXStaticBoxComponent& boxComponent, const Transform& transform);
		bool registerStaticSphereComponent(EntityId entityId, const PhysXStaticSphereComponent& sphereComponent, const Transform& transform);

		bool registerDynamicBoxComponent(EntityId entityId, const PhysXDynamicBoxComponent& boxComponent, const Transform& transform);
		bool registerDynamicSphereComponent(EntityId entityId, const PhysXDynamicSphereComponent& sphereComponent, const Transform& transform);

	protected:
		virtual void onInitialize(EntityRegistry& registry, SystemContainer& systemContainer) override;
		virtual void onDeinitialize(EntityRegistry& registry) override;

	private:
		bool createStaticRigidStatic(EntityId entityId, const physx::PxTransform& transform, physx::PxShape& shape);
		bool createDynamicRigidStatic(EntityId entityId, const physx::PxTransform& transform, physx::PxShape& shape, const PhysXDynamicComponent& dynamicComponent);

		std::unordered_map<EntityId, physx::PxRigidActor*> m_actors;

		physx::PxDefaultAllocator m_allocator;
		physx::PxDefaultErrorCallback m_errorCallback;
		physx::PxFoundation* m_foundation = NULL;
		physx::PxPhysics* m_physics = NULL;
		physx::PxDefaultCpuDispatcher* m_dispatcher = NULL;
		physx::PxScene* m_scene = NULL;
		physx::PxMaterial* m_defaultMaterial = NULL;
		physx::PxPvd* m_pvd = NULL;
	};
}

