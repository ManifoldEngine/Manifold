#pragma once

#include <Core/ManiTime.h> // fwd
#include <Core/CoreFwd.h>

namespace Mani
{
    class TimeSystem : public ECS::System
    {
    public:
        virtual std::string_view getName() const override { return "TimeSystem"; }
        virtual ETickGroup getTickGroup() const override { return ETickGroup::Begin;  }
        virtual bool shouldTick(ECS::Registry& registry) const override { return true; }

        virtual void tick(ECS::Registry& registry) override;

        static std::string getTimeFormatted();

    protected:
        virtual void onInitialize(ECS::Registry& registry, World& world) override;
        virtual void onDeinitialize(ECS::Registry& registry, World& world) override;

    private:
        struct Storage;
    };
}