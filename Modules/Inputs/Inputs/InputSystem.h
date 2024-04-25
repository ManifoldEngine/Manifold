#pragma once

#include <Core/CoreFwd.h>

namespace Mani
{
    DECLARE_EVENT(ActionEvent, EntityId /*userId*/, const struct InputAction& /*inputAction*/)

    class InputSystem : public SystemBase
    {
    public:
        virtual std::string_view getName() const override { return "InputSystem"; }
        virtual bool shouldTick(EntityRegistry& registry) const override { return true; }
        virtual ETickGroup getTickGroup() const override { return ETickGroup::PreTick; }

        virtual void tick(float deltaTime, EntityRegistry& registry) override;

        ActionEvent onActionEvent;
    };
}