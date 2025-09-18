#pragma once

#include <Core/CoreFwd.h>

struct GLFWwindow;

namespace Mani
{
    class ManImGuiSystem : public ECS::System
    {
    public:
        virtual std::string_view getName() const override { return "ManImGuiSystem"; }
        virtual bool shouldTick(const ECS::Registry& registry) const override { return true; }
        // systems displaying imgui should be executing after this system
        virtual ETickGroup getTickGroup() const override { return ETickGroup::PreUpdate; }

        virtual void tick(ECS::Registry& registry) override;

    protected:
        virtual void onInitialize(ECS::Registry& registry, World& world) override;
        virtual void onDeinitialize(ECS::Registry& registry, World& world) override;
    };
}