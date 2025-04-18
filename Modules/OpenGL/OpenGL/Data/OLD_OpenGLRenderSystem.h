//#pragma once
//
//#include <Core/CoreFwd.h>
//#include <ManiMaths/Fwd.h>
//
//namespace Mani
//{
//	class OpenGLResourceSystem;
//
//	class OpenGLRenderSystem : public ECS::System
//	{
//	public:
//		virtual std::string_view getName() const override;
//		virtual bool shouldTick(ECS::Registry& registry) const override;
//		virtual ETickGroup getTickGroup() const override { return ETickGroup::Render; }
//
//		virtual void tick(float deltaTime, ECS::Registry& registry) override;
//
//	protected:
//		virtual void onInitialize(ECS::Registry& registry, World& world) override;
//		virtual void onDeinitialize(ECS::Registry& registry) override;
//	};
//}
//
