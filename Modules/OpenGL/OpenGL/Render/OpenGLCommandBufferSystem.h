#pragma once

#include <Core/CoreFwd.h>
#include <Core/Containers/Array.h>

#include <OpenGL/Render/OpenGLCommand.h>

namespace Mani
{
    class OpenGLCommandBufferSystem : public ECS::System
    {
        virtual std::string_view getName() const override { return "OpenGLCommandBufferSystem"; }
        virtual ETickGroup getTickGroup() const override { return ETickGroup::PreRender; }
        virtual bool shouldTick(const ECS::Registry& registry) const override { return true; }

        virtual void tick(ECS::Registry& registry) override;

    protected:
        virtual void onInitialize(ECS::Registry& registry, World& systemContainer) override;
        virtual void onDeinitialize(ECS::Registry& registry, World& world) override;

    private:

        struct Semaphore
        {
            std::binary_semaphore* getPtr() 
            {
                return &m_value;
            }
        private:
            std::binary_semaphore m_value{ 0 };
        };

        Mani::Array<Semaphore, OpenGL::COMMAND_BUFFER_AMOUNT> m_semaphores;
    };
}