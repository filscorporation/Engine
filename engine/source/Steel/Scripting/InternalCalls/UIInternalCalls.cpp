#include "mono/include/mono/jit/jit.h"

#include "Steel/Scripting/ScriptingCore.h"
#include "UIInternalCalls.h"
#include "InternalCallsCommon.h"
#include "Steel/Scripting/InternalStructures/MaterialPropertyBlockInternal.h"
#include "Steel/Core/Log.h"
#include "Steel/Core/Application.h"
#include "Steel/Scene/SceneHelper.h"
#include "Steel/UI/UIEventHandler.h"
#include "Steel/UI/UIElements/UIButton.h"
#include "Steel/UI/UIElements/UICheckBox.h"
#include "Steel/UI/UIElements/UILayoutGroup.h"
#include "Steel/UI/UIElements/UIScrollableView.h"
#include "Steel/UI/UIElements/UIText.h"
#include "Steel/UI/UIElements/UITabs.h"

void UIInternalCalls::RectTransformation_GetAnchorMin(EntityID entityID, glm::vec2* anchor)
{
    GET_COMPONENT_OR_RETURN(RectTransformation, )

    glm::vec2 result = component.GetAnchorMin();
    anchor->x = result.x;
    anchor->y = result.y;
}

void UIInternalCalls::RectTransformation_SetAnchorMin(EntityID entityID, glm::vec2* anchor)
{
    GET_COMPONENT_OR_RETURN(RectTransformation, )

    component.SetAnchorMin(*anchor);
}

void UIInternalCalls::RectTransformation_GetAnchorMax(EntityID entityID, glm::vec2* anchor)
{
    GET_COMPONENT_OR_RETURN(RectTransformation, )

    glm::vec2 result = component.GetAnchorMax();
    anchor->x = result.x;
    anchor->y = result.y;
}

void UIInternalCalls::RectTransformation_SetAnchorMax(EntityID entityID, glm::vec2* anchor)
{
    GET_COMPONENT_OR_RETURN(RectTransformation, )

    component.SetAnchorMax(*anchor);
}

void UIInternalCalls::RectTransformation_GetAnchoredPosition(EntityID entityID, glm::vec2* position)
{
    GET_COMPONENT_OR_RETURN(RectTransformation, )

    glm::vec2 result = component.GetAnchoredPosition();
    position->x = result.x;
    position->y = result.y;
}

void UIInternalCalls::RectTransformation_SetAnchoredPosition(EntityID entityID, glm::vec2* position)
{
    GET_COMPONENT_OR_RETURN(RectTransformation, )

    component.SetAnchoredPosition(*position);
}

void UIInternalCalls::RectTransformation_GetOffsetMin(EntityID entityID, glm::vec2* offset)
{
    GET_COMPONENT_OR_RETURN(RectTransformation, )

    glm::vec2 result = component.GetOffsetMin();
    offset->x = result.x;
    offset->y = result.y;
}

void UIInternalCalls::RectTransformation_SetOffsetMin(EntityID entityID, glm::vec2* offset)
{
    GET_COMPONENT_OR_RETURN(RectTransformation, )

    component.SetOffsetMin(*offset);
}

void UIInternalCalls::RectTransformation_GetOffsetMax(EntityID entityID, glm::vec2* offset)
{
    GET_COMPONENT_OR_RETURN(RectTransformation, )

    glm::vec2 result = component.GetOffsetMax();
    offset->x = result.x;
    offset->y = result.y;
}

void UIInternalCalls::RectTransformation_SetOffsetMax(EntityID entityID, glm::vec2* offset)
{
    GET_COMPONENT_OR_RETURN(RectTransformation, )

    component.SetOffsetMax(*offset);
}

void UIInternalCalls::RectTransformation_GetPivot(EntityID entityID, glm::vec2* pivot)
{
    GET_COMPONENT_OR_RETURN(RectTransformation, )

    glm::vec2 result = component.GetPivot();
    pivot->x = result.x;
    pivot->y = result.y;
}

void UIInternalCalls::RectTransformation_SetPivot(EntityID entityID, glm::vec2* pivot)
{
    GET_COMPONENT_OR_RETURN(RectTransformation, )

    component.SetPivot(*pivot);
}

void UIInternalCalls::RectTransformation_GetSize(EntityID entityID, glm::vec2* size)
{
    GET_COMPONENT_OR_RETURN(RectTransformation, )

    glm::vec2 result = component.GetSize();
    size->x = result.x;
    size->y = result.y;
}

void UIInternalCalls::RectTransformation_SetSize(EntityID entityID, glm::vec2* size)
{
    GET_COMPONENT_OR_RETURN(RectTransformation, )

    component.SetSize(*size);
}

float UIInternalCalls::RectTransformation_GetSortingOrder(EntityID entityID)
{
    GET_COMPONENT_OR_RETURN(RectTransformation, 0.0f)

    return component.GetSortingOrder();
}

glm::vec3 UIInternalCalls::RectTransformation_GetRotation(EntityID entityID)
{
    GET_COMPONENT_OR_RETURN(RectTransformation, glm::vec3(0.0f))

    return component.GetRotation();
}

void UIInternalCalls::RectTransformation_SetRotation(EntityID entityID, glm::vec3 rotation)
{
    GET_COMPONENT_OR_RETURN(RectTransformation, )

    component.SetRotation(rotation);
}

ResourceID UIInternalCalls::UIImage_GetSprite(EntityID entityID)
{
    GET_COMPONENT_OR_RETURN(UIImage, 0)
    auto image = component.GetImage();

    return image == nullptr ? 0 : image->ID;
}

void UIInternalCalls::UIImage_SetSprite(EntityID entityID, ResourceID spriteID)
{
    GET_COMPONENT_OR_RETURN(UIImage, )

    component.SetImage(Application::Instance->GetResourcesManager()->GetSprite(spriteID));
}

ResourceID UIInternalCalls::UIImage_GetMaterial(EntityID entityID)
{
    GET_COMPONENT_OR_RETURN(UIImage, 0)
    auto material = component.GetMaterial();

    return material == nullptr ? NULL_RESOURCE : material->ID;
}

void UIInternalCalls::UIImage_SetMaterial(EntityID entityID, ResourceID materialID)
{
    GET_COMPONENT_OR_RETURN(UIImage, )

    component.SetMaterial(Application::Instance->GetResourcesManager()->GetMaterial(materialID));
}

glm::vec4 UIInternalCalls::UIImage_GetColor(EntityID entityID)
{
    GET_COMPONENT_OR_RETURN(UIImage, glm::vec4(0.0f))

    return component.GetColor();
}

void UIInternalCalls::UIImage_SetColor(EntityID entityID, glm::vec4 color)
{
    GET_COMPONENT_OR_RETURN(UIImage, )

    component.SetColor(color);
}

bool UIInternalCalls::UIImage_GetConsumeEvents(EntityID entityID)
{
    GET_COMPONENT_OR_RETURN(UIImage, 0)

    return component.GetConsumeEvents();
}

void UIInternalCalls::UIImage_SetConsumeEvents(EntityID entityID, bool consume)
{
    GET_COMPONENT_OR_RETURN(UIImage, )

    component.SetConsumeEvents(consume);
}

void UIInternalCalls::UIImage_GetCustomMaterialProperties(EntityID entityID, MaterialPropertyBlockInternal* properties)
{
    GET_COMPONENT_OR_RETURN(UIImage, )

    properties->FromMaterialPropertyBlock(component.GetCustomProperties());
}

void UIInternalCalls::UIImage_SetCustomMaterialProperties(EntityID entityID, MaterialPropertyBlockInternal properties)
{
    GET_COMPONENT_OR_RETURN(UIImage, )

    MaterialPropertyBlock propertiesOut;
    properties.ToMaterialPropertyBlock(propertiesOut);
    component.SetCustomProperties(propertiesOut);
}

EntityID UIInternalCalls::UIButton_GetTargetImage(EntityID entityID)
{
    GET_COMPONENT_OR_RETURN(UIButton, 0)

    return component.GetTargetImage();
}

void UIInternalCalls::UIButton_SetTargetImage(EntityID entityID, EntityID targetID)
{
    GET_COMPONENT_OR_RETURN(UIButton, )

    component.SetTargetImage(targetID);
}

bool UIInternalCalls::UIButton_GetInteractable(EntityID entityID)
{
    GET_COMPONENT_OR_RETURN(UIButton, false)

    return component.GetInteractable();
}

void UIInternalCalls::UIButton_SetInteractable(EntityID entityID, bool interactable)
{
    GET_COMPONENT_OR_RETURN(UIButton, )

    component.SetInteractable(interactable);
}

TransitionsInfo UIInternalCalls::UIButton_GetTransition(EntityID entityID)
{
    GET_COMPONENT_OR_RETURN(UIButton, TransitionsInfo())

    return component.GetTransitionsInfo();
}

void UIInternalCalls::UIButton_SetTransition(EntityID entityID, TransitionsInfo info)
{
    GET_COMPONENT_OR_RETURN(UIButton, )

    component.SetTransitionsInfo(info);
}

MonoString* UIInternalCalls::UIText_GetText(EntityID entityID)
{
    GET_COMPONENT_OR_RETURN(UIText, nullptr)

    return ScriptingCore::FromString(component.GetText().c_str());
}

void UIInternalCalls::UIText_SetText(EntityID entityID, MonoString* text)
{
    GET_COMPONENT_OR_RETURN(UIText, )

    component.SetText(ScriptingCore::ToString(text));
}

int UIInternalCalls::UIText_GetTextSize(EntityID entityID)
{
    GET_COMPONENT_OR_RETURN(UIText, 0)

    return component.GetTextSize();
}

void UIInternalCalls::UIText_SetTextSize(EntityID entityID, int textSize)
{
    GET_COMPONENT_OR_RETURN(UIText, )

    component.SetTextSize(textSize);
}

float UIInternalCalls::UIText_GetLineSpacing(EntityID entityID)
{
    GET_COMPONENT_OR_RETURN(UIText, 0.0f)

    return component.GetLineSpacing();
}

void UIInternalCalls::UIText_SetLineSpacing(EntityID entityID, float spacing)
{
    GET_COMPONENT_OR_RETURN(UIText, )

    component.SetLineSpacing(spacing);
}

glm::vec4 UIInternalCalls::UIText_GetColor(EntityID entityID)
{
    GET_COMPONENT_OR_RETURN(UIText, glm::vec4(0.0f))

    return component.GetColor();
}

void UIInternalCalls::UIText_SetColor(EntityID entityID, glm::vec4 color)
{
    GET_COMPONENT_OR_RETURN(UIText, )

    component.SetColor(color);
}

bool UIInternalCalls::UIButton_GetIsTextAutoSize(EntityID entityID)
{
    GET_COMPONENT_OR_RETURN(UIText, 0)

    return component.GetIsTextAutoSize();
}

void UIInternalCalls::UIButton_SetIsTextAutoSize(EntityID entityID, bool isAutoSize)
{
    GET_COMPONENT_OR_RETURN(UIText, )

    component.SetIsTextAutoSize(isAutoSize);
}

AlignmentTypes::AlignmentType UIInternalCalls::UIButton_GetTextAlignment(EntityID entityID)
{
    GET_COMPONENT_OR_RETURN(UIText, AlignmentTypes::CenterLeft)

    return component.GetTextAlignment();
}

void UIInternalCalls::UIButton_SetTextAlignment(EntityID entityID, AlignmentTypes::AlignmentType alignmentType)
{
    GET_COMPONENT_OR_RETURN(UIText, )

    component.SetTextAlignment(alignmentType);
}

OverflowModes::OverflowMode UIInternalCalls::UIButton_GetTextOverflowMode(EntityID entityID)
{
    GET_COMPONENT_OR_RETURN(UIText, OverflowModes::Overflow)

    return component.GetOverflowMode();
}

void UIInternalCalls::UIButton_SetTextOverflowMode(EntityID entityID, OverflowModes::OverflowMode overflowMode)
{
    GET_COMPONENT_OR_RETURN(UIText, )

    component.SetOverflowMode(overflowMode);
}

EntityID UIInternalCalls::UIInputField_GetTargetText(EntityID entityID)
{
    GET_COMPONENT_OR_RETURN(UIInputField, 0)

    return component.GetTargetText();
}

void UIInternalCalls::UIInputField_SetTargetText(EntityID entityID, EntityID targetID)
{
    GET_COMPONENT_OR_RETURN(UIInputField, )

    component.SetTargetText(targetID);
}

EntityID UIInternalCalls::UIInputField_GetTargetImage(EntityID entityID)
{
    GET_COMPONENT_OR_RETURN(UIInputField, 0)

    return component.GetTargetImage();
}

void UIInternalCalls::UIInputField_SetTargetImage(EntityID entityID, EntityID targetID)
{
    GET_COMPONENT_OR_RETURN(UIInputField, )

    component.SetTargetImage(targetID);
}

bool UIInternalCalls::UIInputField_GetInteractable(EntityID entityID)
{
    GET_COMPONENT_OR_RETURN(UIInputField, 0)

    return component.GetInteractable();
}

void UIInternalCalls::UIInputField_SetInteractable(EntityID entityID, bool interactable)
{
    GET_COMPONENT_OR_RETURN(UIInputField, )

    component.SetInteractable(interactable);
}

uint32_t UIInternalCalls::UIInputField_GetCursorWidth(EntityID entityID)
{
    GET_COMPONENT_OR_RETURN(UIInputField, 0)

    return component.GetCursorWidth();
}

void UIInternalCalls::UIInputField_SetCursorWidth(EntityID entityID, uint32_t width)
{
    GET_COMPONENT_OR_RETURN(UIInputField, )

    component.SetCursorWidth(width);
}

glm::vec4 UIInternalCalls::UIInputField_GetCursorColor(EntityID entityID)
{
    GET_COMPONENT_OR_RETURN(UIInputField, glm::vec4(0.0f))

    return component.GetCursorColor();
}

void UIInternalCalls::UIInputField_SetCursorColor(EntityID entityID, glm::vec4 color)
{
    GET_COMPONENT_OR_RETURN(UIInputField, )

    component.SetCursorColor(color);
}

bool UIInternalCalls::UIInputField_GetAutoColor(EntityID entityID)
{
    GET_COMPONENT_OR_RETURN(UIInputField, false)

    return component.GetCursorAutoColor();
}

void UIInternalCalls::UIInputField_SetAutoColor(EntityID entityID, bool isAuto)
{
    GET_COMPONENT_OR_RETURN(UIInputField, )

    component.SetCursorAutoColor(isAuto);
}

bool UIInternalCalls::UIInputField_GetIsMultiline(EntityID entityID)
{
    GET_COMPONENT_OR_RETURN(UIInputField, false)

    return component.GetIsMultiline();
}

void UIInternalCalls::UIInputField_SetIsMultiline(EntityID entityID, bool isMultiline)
{
    GET_COMPONENT_OR_RETURN(UIInputField, )

    component.SetIsMultiline(isMultiline);
}

TextTypes::TextType UIInternalCalls::UIInputField_GetTextType(EntityID entityID)
{
    GET_COMPONENT_OR_RETURN(UIInputField, TextTypes::Standard)

    return component.GetTextType();
}

void UIInternalCalls::UIInputField_SetTextType(EntityID entityID, TextTypes::TextType type)
{
    GET_COMPONENT_OR_RETURN(UIInputField, )

    component.SetTextType(type);
}

glm::vec4 UIInternalCalls::UIInputField_GetSelectionColor(EntityID entityID)
{
    GET_COMPONENT_OR_RETURN(UIInputField, glm::vec4(1.0f))

    return component.GetSelectionColor();
}

void UIInternalCalls::UIInputField_SetSelectionColor(EntityID entityID, glm::vec4 color)
{
    GET_COMPONENT_OR_RETURN(UIInputField, )

    component.SetSelectionColor(color);
}

bool UIInternalCalls::UICheckBox_GetValue(EntityID entityID)
{
    GET_COMPONENT_OR_RETURN(UICheckBox, false)

    return component.GetValue();
}

void UIInternalCalls::UICheckBox_SetValue(EntityID entityID, bool value)
{
    GET_COMPONENT_OR_RETURN(UICheckBox, )

    component.SetValue(value);
}

bool UIInternalCalls::UICheckBox_GetInteractable(EntityID entityID)
{
    GET_COMPONENT_OR_RETURN(UICheckBox, 0)

    return component.GetInteractable();
}

void UIInternalCalls::UICheckBox_SetInteractable(EntityID entityID, bool interactable)
{
    GET_COMPONENT_OR_RETURN(UICheckBox, )

    component.SetInteractable(interactable);
}

TransitionsInfo UIInternalCalls::UICheckBox_GetTransition(EntityID entityID)
{
    GET_COMPONENT_OR_RETURN(UICheckBox, TransitionsInfo())

    return component.GetTransitionsInfo();
}

void UIInternalCalls::UICheckBox_SetTransition(EntityID entityID, TransitionsInfo info)
{
    GET_COMPONENT_OR_RETURN(UICheckBox, )

    component.SetTransitionsInfo(info);
}

int UIInternalCalls::UITabs_TabsCount(EntityID entityID)
{
    GET_COMPONENT_OR_RETURN(UITabs, 0)

    return component.GetTabsCount();
}

int UIInternalCalls::UITabs_GetActiveTab(EntityID entityID)
{
    GET_COMPONENT_OR_RETURN(UITabs, 0)

    return component.GetActiveTab();
}

void UIInternalCalls::UITabs_SetActiveTab(EntityID entityID, int value)
{
    GET_COMPONENT_OR_RETURN(UITabs, )

    component.SetActiveTab(value);
}

EntityID UIInternalCalls::UITabs_GetTab(EntityID entityID, int index)
{
    GET_COMPONENT_OR_RETURN(UITabs, NULL_ENTITY)

    return component.GetTab(index);
}

EntityID UIInternalCalls::UITabs_AddTab(EntityID entityID, MonoString* name)
{
    GET_COMPONENT_OR_RETURN(UITabs, NULL_ENTITY)

    return component.AddTab(ScriptingCore::ToString(name));
}

bool UIInternalCalls::UITabs_RemoveTab(EntityID entityID, int index)
{
    GET_COMPONENT_OR_RETURN(UITabs, false)

    return component.RemoveTab(index);
}

EntityID UIInternalCalls::UILayoutGroup_AddElement(EntityID entityID, float minWidth, float minHeight, float prefWidth, float prefHeight)
{
    GET_COMPONENT_OR_RETURN(UILayoutGroup, NULL_ENTITY)

    return component.AddElement(minWidth, minHeight, prefWidth, prefHeight);
}

EntityID UIInternalCalls::UILayoutGroup_AddElement2(EntityID entityID, EntityID elementID)
{
    GET_COMPONENT_OR_RETURN(UILayoutGroup, NULL_ENTITY)

    return component.AddElement(elementID);
}

EntityID UIInternalCalls::UIScrollableView_GetContent(EntityID entityID)
{
    GET_COMPONENT_OR_RETURN(UIScrollableView, NULL_ENTITY)

    return component.GetContent();
}

EntityID UIInternalCalls::UI_CreateUIElement()
{
    return Application::Instance->GetCurrentScene()->GetUILayer()->CreateUIElement();
}

EntityID UIInternalCalls::UI_CreateUIElement2(MonoString* name)
{
    return Application::Instance->GetCurrentScene()->GetUILayer()->CreateUIElement(ScriptingCore::ToString(name), NULL_ENTITY);
}

EntityID UIInternalCalls::UI_CreateUIElement3(MonoString* name, EntityID parentEntityID)
{
    return Application::Instance->GetCurrentScene()->GetUILayer()->CreateUIElement(ScriptingCore::ToString(name), parentEntityID);
}

EntityID UIInternalCalls::UI_CreateUIImage()
{
    return Application::Instance->GetCurrentScene()->GetUILayer()->CreateUIImage();
}

EntityID UIInternalCalls::UI_CreateUIImage2(ResourceID spriteID, MonoString* name, EntityID parentEntityID)
{
    Sprite* sprite = Application::Instance->GetResourcesManager()->GetSprite(spriteID);

    return Application::Instance->GetCurrentScene()->GetUILayer()->CreateUIImage(sprite, ScriptingCore::ToString(name), parentEntityID);
}

EntityID UIInternalCalls::UI_CreateUIButton()
{
    return Application::Instance->GetCurrentScene()->GetUILayer()->CreateUIButton();
}

EntityID UIInternalCalls::UI_CreateUIButton2(ResourceID spriteID, MonoString* name, EntityID parentEntityID)
{
    Sprite* sprite = Application::Instance->GetResourcesManager()->GetSprite(spriteID);

    return Application::Instance->GetCurrentScene()->GetUILayer()->CreateUIButton(sprite, ScriptingCore::ToString(name), parentEntityID);
}

EntityID UIInternalCalls::UI_CreateUIText()
{
    return Application::Instance->GetCurrentScene()->GetUILayer()->CreateUIText();
}

EntityID UIInternalCalls::UI_CreateUIText2(MonoString* text, MonoString* name, EntityID parentEntityID)
{
    return Application::Instance->GetCurrentScene()->GetUILayer()->CreateUIText(ScriptingCore::ToString(text), ScriptingCore::ToString(name), parentEntityID);
}

EntityID UIInternalCalls::UI_CreateUIInputField()
{
    return Application::Instance->GetCurrentScene()->GetUILayer()->CreateUIInputField();
}

EntityID UIInternalCalls::UI_CreateUIInputField2(MonoString* name, EntityID parentEntityID)
{
    return Application::Instance->GetCurrentScene()->GetUILayer()->CreateUIInputField(ScriptingCore::ToString(name), parentEntityID);
}

EntityID UIInternalCalls::UI_CreateUIClipping()
{
    return Application::Instance->GetCurrentScene()->GetUILayer()->CreateUIClipping();
}

EntityID UIInternalCalls::UI_CreateUIClipping2(EntityID parentEntityID)
{
    return Application::Instance->GetCurrentScene()->GetUILayer()->CreateUIClipping(parentEntityID);
}

EntityID UIInternalCalls::UI_CreateUICheckBox()
{
    return Application::Instance->GetCurrentScene()->GetUILayer()->CreateUICheckBox();
}

EntityID UIInternalCalls::UI_CreateUICheckBox2(MonoString* label, MonoString* name, EntityID parentEntityID)
{
    return Application::Instance->GetCurrentScene()->GetUILayer()->CreateUICheckBox(ScriptingCore::ToString(label), ScriptingCore::ToString(name), parentEntityID);
}

bool UIInternalCalls::UI_IsPointerOverUI()
{
    return Application::Instance->GetCurrentScene()->GetUILayer()->IsPointerOverUI();
}

EntityID UIInternalCalls::UI_CreateUITabs()
{
    return Application::Instance->GetCurrentScene()->GetUILayer()->CreateUITabs();
}

EntityID UIInternalCalls::UI_CreateUITabs2(MonoArray* tabs, MonoString* name, EntityID parentEntityID)
{
    std::vector<std::string> tabsNames;
    ScriptingCore::FromMonoStringArray(tabs, tabsNames);

    return Application::Instance->GetCurrentScene()->GetUILayer()->CreateUITabs(tabsNames, ScriptingCore::ToString(name), parentEntityID);
}

EntityID UIInternalCalls::UI_CreateUILayoutGroup(LayoutGroupTypes::LayoutGroupType type)
{
    return Application::Instance->GetCurrentScene()->GetUILayer()->CreateUILayoutGroup(type);
}

EntityID UIInternalCalls::UI_CreateUILayoutGroup2(LayoutGroupTypes::LayoutGroupType type, MonoString* name, EntityID parentEntityID)
{
    return Application::Instance->GetCurrentScene()->GetUILayer()->CreateUILayoutGroup(type, ScriptingCore::ToString(name), parentEntityID);
}

EntityID UIInternalCalls::UI_CreateUIScrollableView()
{
    return Application::Instance->GetCurrentScene()->GetUILayer()->CreateUIScrollableView();
}

EntityID UIInternalCalls::UI_CreateUIScrollableView2(MonoString* name, EntityID parentEntityID)
{
    return Application::Instance->GetCurrentScene()->GetUILayer()->CreateUIScrollableView(ScriptingCore::ToString(name), parentEntityID);
}
