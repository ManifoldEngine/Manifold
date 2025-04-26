#pragma once

#include <Core/CoreFwd.h>

namespace Mani
{
    class IOpenGLRenderExtension;

    class OpenGLRenderSystem : public ECS::System
    {
    public:

        virtual std::string_view getName() const override { return "OpenGLRenderSystem"; }
        virtual ETickGroup getTickGroup() const override { return ETickGroup::Render; }
        virtual bool shouldTick(ECS::Registry& registry) const override { return true; }

        virtual void tick(float deltaTime, ECS::Registry& registry) override;

        template<typename TFunctor, typename... TArgs>
        static void enqueueRenderTask(ECS::Registry& registry, TFunctor&& f, TArgs&&... args);

        static ECS::EntityId addExtension(ECS::Registry& registry, std::shared_ptr<IOpenGLRenderExtension> extension);
        static void removeExtension(ECS::Registry& registry, ECS::EntityId entityId);

    protected:
        virtual void onInitialize(ECS::Registry& registry, World& world) override;
        virtual void onDeinitialize(ECS::Registry& registry) override;
    
    private:
        struct Storage
        {
            ThreadPool renderThread{ 1 };
        };
    };

    template<typename TFunctor, typename ...TArgs>
    inline void OpenGLRenderSystem::enqueueRenderTask(ECS::Registry& registry, TFunctor&& f, TArgs && ...args)
    {
        OpenGLRenderSystem::Storage* storage = registry.getSingle<OpenGLRenderSystem::Storage>();
        if (storage == nullptr)
        {
            return;
        }

        storage->renderThread.enqueue(std::forward<TFunctor>(f), std::forward<TArgs>(args)...);
    }
}