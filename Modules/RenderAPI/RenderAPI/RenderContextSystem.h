#pragma once

#include <Core/CoreFwd.h>
//forward
#include <RenderAPI/RenderContext.h>

namespace Mani
{
    class RenderContextSystem : public Mani::ECS::System
    {
    public:
        virtual std::string_view getName() const override { return "RendererContextSystem"; }
        virtual bool shouldTick(const Mani::ECS::Registry& registry) const override { return false; }

    protected:
        virtual void onInitialize(Mani::ECS::Registry& registry, Mani::World& world) override;
        virtual void onDeinitialize(Mani::ECS::Registry& registry, Mani::World& world) override;
    };
}
