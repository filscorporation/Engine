#pragma once

#include "../EntityComponentSystem/Component.h"
#include "UIEvent.h"
#include "RectTransformation.h"

using EventCallbackFunction = void (*)(EntityID, UIEventTypes::UIEventType, UIEvent&);

namespace EventHandlerTypes
{
    enum EventHandlerType
    {
        Normal,
        Transparent,
        ClippingOpen,
        ClippingClose,
    };
}

class UIEventHandler : public Component
{
public:
    explicit UIEventHandler(EntityID ownerEntityID) : Component(ownerEntityID) { }

    void HandleEvent(const ComponentAccessor<RectTransformation>& rtAccessor, UIEvent& uiEvent);

    // Parameters, defined by UI element
    EventHandlerTypes::EventHandlerType Type = EventHandlerTypes::Normal;
    EntityID RectEntity = NULL_ENTITY;
    float SortingOrder = 0;
    UIEventTypes::UIEventType EventsMask = (UIEventTypes::UIEventType)0;
    EventCallbackFunction EventCallback = nullptr;

private:
    bool lastFrameContains = false;
    bool isDragged = false;
};
