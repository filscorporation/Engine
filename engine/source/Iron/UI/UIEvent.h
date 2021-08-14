#pragma once

#include "glm/vec2.hpp"
#include <string>

#include "../Core/ButtonStates.h"

namespace UIEventTypes
{
    enum UIEventType
    {
        Any                         = 1 << 0,
        MouseEnter                  = 1 << 1,
        MouseExit                   = 1 << 2,
        MouseOver                   = 1 << 3,
        MouseJustPressed            = 1 << 4,
        MousePressed                = 1 << 5,
        MouseJustReleased           = 1 << 6,
        MouseJustPressedAnywhere    = 1 << 7,
        MousePressedAnywhere        = 1 << 8,
        MouseJustReleasedAnywhere   = 1 << 9,
        MouseDragBegin              = 1 << 10,
        MouseDrag                   = 1 << 11,
        MouseDragEnd                = 1 << 12,
        ScrollDelta                 = 1 << 13,
        KeyInput                    = 1 << 14,
        TextInput                   = 1 << 15,
    };

    inline UIEventType operator|(UIEventType a, UIEventType b)
    {
        return static_cast<UIEventType>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }

    inline UIEventType operator&(UIEventType a, UIEventType b)
    {
        return static_cast<UIEventType>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
    }
}

struct UIEvent
{
    bool Used = false;
    // If not 0, then event got 'clipped' by UI clipping planes
    int ClippingDepth = 0;

    glm::vec2 MousePosition;
    glm::vec2 MouseDelta;
    glm::vec2 ScrollDelta;
    ButtonStates::ButtonState LeftMouseButtonState;
    ButtonStates::ButtonState RightMouseButtonState;
    bool AnyKey = false;
    std::string InputString;
};
