#pragma once

#include <Core/CoreFwd.h>

namespace Mani
{
    class IOpenGLRenderExtension;
    class IOpenGLRenderer;

    class OpenGLRenderSystem : public ECS::System
    {
    public:
        struct Storage
        {
            ThreadPool renderThread{ 1 };

            std::vector<IOpenGLRenderer*> renderers;
            std::vector<IOpenGLRenderExtension*> extensions;
        };

        virtual std::string_view getName() const override { return "OpenGLRenderSystem"; }
        virtual ETickGroup getTickGroup() const override { return ETickGroup::Render; }
        virtual bool shouldTick(ECS::Registry& registry) const override { return true; }

        virtual void tick(float deltaTime, ECS::Registry& registry) override;

        template<typename TFunctor, typename... TArgs>
        static void enqueueRenderTask(ECS::Registry& registry, TFunctor&& f, TArgs&&... args);

        static void registerExtension(ECS::Registry& registry, IOpenGLRenderExtension* extension);
        static void unregisterExtension(ECS::Registry& registry, IOpenGLRenderExtension* extension);

        static void registerRenderer(ECS::Registry& registry, IOpenGLRenderer* renderer);
        static void unregisterRenderer(ECS::Registry& registry, IOpenGLRenderer* renderer);

    protected:
        virtual void onInitialize(ECS::Registry& registry, World& world) override;
        virtual void onDeinitialize(ECS::Registry& registry, World& world) override;
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