#pragma once

#include <Core/CoreFwd.h>
#include <PhysX/PhysXComponents.h>

#include <PxPhysicsAPI.h>

#define PVD_HOST "127.0.0.1"

namespace Mani
{
	struct Mesh;

	class PhysXSystem : public SystemBase
	{
	public:
		virtual std::string_view getName() const override;
		virtual bool shouldTick(ECS::Registry& registry) const override;
		virtual ETickGroup getTickGroup() const override;

		virtual void tick(float deltaTime, ECS::Registry& registry) override;

		bool registerStaticBoxComponent(ECS::EntityId entityId, const PhysXStaticBoxComponent& boxComponent, const Transform& transform);
		bool registerStaticSphereComponent(ECS::EntityId entityId, const PhysXStaticSphereComponent& sphereComponent, const Transform& transform);
		bool registerStaticMeshComponent(ECS::EntityId entityId, const PhysXStaticMeshComponent& meshComponent, const Transform& transform);

		bool registerDynamicBoxComponent(ECS::EntityId entityId, const PhysXDynamicBoxComponent& boxComponent, const Transform& transform);
		bool registerDynamicSphereComponent(ECS::EntityId entityId, const PhysXDynamicSphereComponent& sphereComponent, const Transform& transform);
		bool registerDynamicMeshComponent(ECS::EntityId entityId, const PhysXDynamicMeshComponent& meshComponent, const Transform& transform);

	protected:
		virtual void onInitialize(ECS::Registry& registry, SystemContainer& systemContainer) override;
		virtual void onDeinitialize(ECS::Registry& registry) override;

	private:
		void onComponentRemoved(ECS::Registry& registry, ECS::EntityId entityId, ECS::ComponentId componentId);
		void onEntityDestroyed(ECS::Registry& registry, ECS::EntityId entityId);
		void checkForDestroyedActors(ECS::EntityId entityId);

		bool createStaticRigidActor(ECS::EntityId entityId, const physx::PxTransform& transform, physx::PxShape& shape);
		bool createDynamicRigidActor(ECS::EntityId entityId, const physx::PxTransform& transform, physx::PxShape& shape, const PhysXDynamicComponent& dynamicComponent);
		physx::PxConvexMesh* createPxConvexMesh(const std::shared_ptr<Mesh>& mesh);

		std::unordered_map<ECS::EntityId, physx::PxRigidActor*> m_actors;

		float m_accumulatedTime = 0.f;

		EventHandle onComponentRemovedHandle;
		EventHandle onEntityDestroyedHandle;

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

