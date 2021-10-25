#include "UITabs.h"
#include "UIButton.h"
#include "UIText.h"
#include "../../Core/Application.h"
#include "../../Core/Log.h"

bool UITabs::Validate(EntitiesRegistry* entitiesRegistry)
{
    return Component::Validate(entitiesRegistry) && CheckRectTransformation(entitiesRegistry);
}

void UITabs::OnCreated(EntitiesRegistry* entitiesRegistry)
{
    auto layer = Application::Instance->GetCurrentScene()->GetUILayer();
    _tabOpenedSprite = layer->UIResources.DefaultTabOpenedSprite;
    _tabClosedSprite = layer->UIResources.DefaultTabClosedSprite;
    buttonWidth = _tabOpenedSprite == nullptr ? 0 : _tabOpenedSprite->SpriteTexture->GetWidth();
    buttonHeight = _tabOpenedSprite == nullptr ? 0 : _tabOpenedSprite->SpriteTexture->GetHeight();

    content = layer->CreateUIElement("Tabs content", Owner);
    auto& contentRT = entitiesRegistry->GetComponent<RectTransformation>(content);
    contentRT.SetAnchorMin(glm::vec2(0.0f, 0.0f));
    contentRT.SetAnchorMax(glm::vec2(1.0f, 1.0f));
    contentRT.SetOffsetMax(glm::vec2(0.0f, buttonHeight));

    auto& rt = entitiesRegistry->GetComponent<RectTransformation>(Owner);
    // Set default size
    rt.SetSize(glm::vec2(buttonWidth * 4, buttonWidth * 8));
}

void UITabs::SetTabsSprites(Sprite* tabOpenedSprite, Sprite* tabClosedSprite)
{
    _tabOpenedSprite = tabOpenedSprite;
    _tabClosedSprite = tabClosedSprite;
    buttonWidth = _tabOpenedSprite == nullptr ? 0 : _tabOpenedSprite->SpriteTexture->GetWidth();
    buttonHeight = _tabOpenedSprite == nullptr ? 0 : _tabOpenedSprite->SpriteTexture->GetHeight();

    auto entitiesRegistry = Application::Instance->GetCurrentScene()->GetEntitiesRegistry();

    for (int i = 0; i < tabs.size(); ++i)
    {
        auto& currentButton = entitiesRegistry->GetComponent<UIButton>(tabs[i].ButtonID);
        auto& currentButtonSprite = entitiesRegistry->GetComponent<UIImage>(currentButton.GetTargetImage());
        if (activeTab == i)
            currentButtonSprite.SetImage(_tabOpenedSprite);
        else
            currentButtonSprite.SetImage(_tabClosedSprite);
    }
}

EntityID UITabs::GetTab(int index)
{
    if (index < 0 || index >= tabs.size())
    {
        Log::LogError("Tab index outside of range");
        return NULL_ENTITY;
    }

    return tabs[index].ContentID;
}

int UITabs::GetTabsCount()
{
    return (int)tabs.size();
}

EntityID UITabs::AddTab(const std::string& name)
{
    // TODO: check for memory errors - after layer->CreateUI..., current UITabs component can move

    if (content == NULL_ENTITY)
    {
        Log::LogError("Tabs {0} content is null", Owner);
        return NULL_ENTITY;
    }

    auto layer = Application::Instance->GetCurrentScene()->GetUILayer();
    auto entitiesRegistry = Application::Instance->GetCurrentScene()->GetEntitiesRegistry();
    int index = (int)tabs.size();
    auto tabSprite = _tabClosedSprite;
    EntityID thisEntityID = Owner;
    EntityID contentEntityID = content;
    int currentActiveTab = activeTab;

    EntityID newContentEntity = layer->CreateUIElement((name + " tab").c_str(), contentEntityID);
    auto& tabRT = entitiesRegistry->GetComponent<RectTransformation>(newContentEntity);
    tabRT.SetAnchorMin(glm::vec2(0.0f, 0.0f));
    tabRT.SetAnchorMax(glm::vec2(1.0f, 1.0f));

    EntityID buttonEntity = layer->CreateUIButton(tabSprite, (name + " tab").c_str(), thisEntityID);
    auto& buttonRT = entitiesRegistry->GetComponent<RectTransformation>(buttonEntity);
    SetButtonRect(buttonRT, index);
    auto& button = entitiesRegistry->GetComponent<UIButton>(buttonEntity);
    button.Callback = [thisEntityID](EntityID entityID)
    {
        auto registry = Application::Instance->GetCurrentScene()->GetEntitiesRegistry();
        if (registry->HasComponent<UITabs>(thisEntityID))
            registry->GetComponent<UITabs>(thisEntityID).SetActiveTabByButtonID(entityID);
    };

    EntityID textEntity = layer->CreateUIText(name.c_str(), "Label", buttonEntity);
    auto& textRT = entitiesRegistry->GetComponent<RectTransformation>(textEntity);
    textRT.SetAnchorMin(glm::vec2(0.0f, 0.0f));
    textRT.SetAnchorMax(glm::vec2(1.0f, 1.0f));
    textRT.SetOffsetMin(glm::vec2(4.0f, 2.0f));
    textRT.SetOffsetMax(glm::vec2(4.0f, 2.0f));
    auto& text = entitiesRegistry->GetComponent<UIText>(textEntity);
    text.SetColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

    tabs.emplace_back(buttonEntity, newContentEntity);

    entitiesRegistry->EntitySetActive(newContentEntity, false, true);
    if (currentActiveTab == -1)
        SetActiveTab((int)index);

    return newContentEntity;
}

bool UITabs::RemoveTab(int index)
{
    if (index < 0 || index >= tabs.size())
    {
        Log::LogError("Tab index outside of range");
        return false;
    }

    auto entitiesRegistry = Application::Instance->GetCurrentScene()->GetEntitiesRegistry();
    entitiesRegistry->DeleteEntity(tabs[index].ContentID);
    entitiesRegistry->DeleteEntity(tabs[index].ButtonID);

    tabs.erase(tabs.begin() + index);
    for (int i = index; i < tabs.size(); ++i)
    {
        SetButtonRect(entitiesRegistry->GetComponent<RectTransformation>(tabs[i].ButtonID), i);
    }

    if (activeTab == index)
    {
        activeTab = -1;
        if (!tabs.empty())
            SetActiveTab(0);
    }
    else if (activeTab > index)
        activeTab--;

    return true;
}

int UITabs::GetActiveTab() const
{
    return activeTab;
}

void UITabs::SetActiveTab(int index)
{
    // TODO: check for memory errors - after set active, current UITabs component can move

    if (index == activeTab)
        return;

    auto entitiesRegistry = Application::Instance->GetCurrentScene()->GetEntitiesRegistry();

    if (activeTab != -1)
    {
        entitiesRegistry->EntitySetActive(tabs[activeTab].ContentID, false, true);
        auto& lastButton = entitiesRegistry->GetComponent<UIButton>(tabs[activeTab].ButtonID);
        auto& lastButtonSprite = entitiesRegistry->GetComponent<UIImage>(lastButton.GetTargetImage());
        lastButtonSprite.SetImage(_tabClosedSprite);
    }

    activeTab = index;
    entitiesRegistry->EntitySetActive(tabs[activeTab].ContentID, true, true);
    auto& currentButton = entitiesRegistry->GetComponent<UIButton>(tabs[activeTab].ButtonID);
    auto& currentButtonSprite = entitiesRegistry->GetComponent<UIImage>(currentButton.GetTargetImage());
    currentButtonSprite.SetImage(_tabOpenedSprite);
}

void UITabs::SetActiveTabByButtonID(EntityID buttonID)
{
    for (int i = 0; i < tabs.size(); ++i)
    {
        if (tabs[i].ButtonID == buttonID)
        {
            SetActiveTab(i);
            return;
        }
    }
}

void UITabs::SetButtonRect(RectTransformation& buttonRT, int index)
{
    buttonRT.SetAnchorMin(glm::vec2(0.0f, 1.0f));
    buttonRT.SetAnchorMax(glm::vec2(0.0f, 1.0f));
    buttonRT.SetPivot(glm::vec2(0.0f, 1.0f));
    buttonRT.SetAnchoredPosition(glm::vec2(index * buttonWidth, 0.0f));
    buttonRT.SetSize(glm::vec2(buttonWidth, buttonHeight));
}
