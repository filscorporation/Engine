#pragma once

#include "Steel/Rendering/Window.h"

class AndroidWindow : public Window
{
public:
    AndroidWindow() = default;
    ~AndroidWindow() override = default;

    void Init(int width, int height, glm::vec3 color, bool fullscreen, bool doubleBuffer, bool vSync) override;
    void InitInputHandler() override;
    void Apply() override;

    int GetWidth() override;
    int GetHeight() override;
    bool GetIsMinimized() override;

    bool IsScreenSizeDirty() override;
    void SetScreenSizeDirty() override;

    void UpdateSize() override;
    void SwapBuffers() override;
    void ClearDirty() override;
    bool WindowShouldClose() override;
    void Terminate() override;

    float InvertY(float y) override;
};
