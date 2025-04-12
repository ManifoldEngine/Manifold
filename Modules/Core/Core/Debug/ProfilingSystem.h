#pragma once

#include <Core/CoreFwd.h>

namespace Mani
{
    namespace _impl
    {
        struct ScopedTimer;
    }

    class ProfilingSystem : public Mani::SystemBase
    {
    public:
        virtual std::string_view getName() const override { return "ProfilingSystem"; }
        virtual bool shouldTick(Mani::ECS::Registry& registry) const override { return false; }

        static void onTimerDestroyed(const Mani::_impl::ScopedTimer& namedScopeTimer);

    protected:
        virtual void onInitialize(Mani::ECS::Registry& registry, World& world) override;
        virtual void onDeinitialize(Mani::ECS::Registry& registry) override;
    };

}