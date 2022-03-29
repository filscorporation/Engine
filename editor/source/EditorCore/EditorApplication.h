#pragma once

#include <Steel.h>

namespace EditorStates
{
    enum EditorState
    {
        Stopped,
        Playing,
        Paused,
        Step,
    };
}

class EditorApplication : public Application
{
public:
    EditorApplication() : Application() {}

    void Init(ApplicationSettings settings) override;
    void RunUpdate() override;
    bool IsRunning() override;

    EditorStates::EditorState GetState() const;
    void SetState(EditorStates::EditorState newState);
    void EnterPlayMode(EditorStates::EditorState newState);
    void EnterEditMode(EditorStates::EditorState newState);
    void LoadSceneToEdit(const std::string& filePath);

    ApplicationContext* GetAppContext();
    Framebuffer* ApplicationFramebuffer = nullptr;
    Framebuffer* OpenedSceneFramebuffer = nullptr;
    ApplicationContext* EditorContext = nullptr;
    EntityID EditorViewCameraEntity = NULL_ENTITY;

private:
    void Terminate() override;
    void CreateEditorViewCamera();

    EditorStates::EditorState state = EditorStates::Stopped;

    friend class AppView;
    friend class ControlPanel;
    friend class HierarchyView;
    friend class PropertiesView;
    friend class SceneView;
};
