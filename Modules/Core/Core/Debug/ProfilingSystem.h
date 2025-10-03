#pragma once

#include <Core/CoreFwd.h>

namespace Mani
{
    namespace _impl
    {
        struct ScopedTimer;
    }

    class ProfilingSystem : public Mani::ECS::System
    {
    public:
        virtual std::string_view getName() const override { return "ProfilingSystem"; }
        virtual bool shouldTick(const Mani::ECS::Registry& registry) const override { return true; }
        virtual ETickGroup getTickGroup() const override { return ETickGroup::End; }

        static void onTimerDestroyed(const Mani::_impl::ScopedTimer& namedScopeTimer);

    protected:
        virtual void onInitialize(Mani::ECS::Registry& registry, World& world) override;
        virtual void onDeinitialize(Mani::ECS::Registry& registry, Mani::World& world) override;

    public:
        virtual void tick(Mani::ECS::Registry& registry) override;

    private:
        struct Storage;
    };
}