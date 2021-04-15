#pragma once

#include <glm/vec2.hpp>

#include "Sprite.h"
#include "RendererData.h"
#include "../EntityComponentSystem/Component.h"
#include "../Scene/Transformation.h"

class SpriteRenderer : public Component
{
public:
    explicit SpriteRenderer(EntityID ownerEntityID) : Component(ownerEntityID) { }

    uint32_t CurrentImageTileIndex = 0;

    void OnRender(Transformation& transformation);
    void SetImage(Sprite* image);
    Sprite* GetImage();
    glm::vec2 GetWorldSize();
    bool IsTransparent() const;

private:
    Sprite* _image = nullptr;
    glm::vec4 _color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    QuadCache quadCache;
    bool _isTransparent = false;
};
