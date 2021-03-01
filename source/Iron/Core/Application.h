#pragma once

#include "../Scene/Entity.h"
#include "../Scene/Scene.h"
#include "ResourcesManager.h"
#include "../Rendering/Screen.h"
#include "../Physics/RigidBody.h"


namespace ApplicationStates
{
    enum ApplicationState
    {
        Initializing,
        EntryPoint,
        PollEvents,
        OnUpdate,
        OnPhysicsUpdate,
        OnLateUpdate,
        OnRender,
        CleaningDestroyedEntities,
    };
}

struct ApplicationSettings
{
    int ScreenWidth = 800;
    int ScreenHeight = 600;
    glm::vec3 ScreenColor = glm::vec3(0.2f, 0.3f, 0.3f);
    bool Fullscreen = false;
};

class Application
{
public:
    static Application* Instance;

    Application(ApplicationSettings settings);
    void Run();
    void RunUpdate();
    void Quit();
    ApplicationStates::ApplicationState State();
    ResourcesManager* GetResourcesManager();
    Scene* GetCurrentScene();
    std::string GetRuntimePath();
    std::string GetDataPath();

private:
    bool isRunning;
    ApplicationStates::ApplicationState state;
    ResourcesManager* resources;
    Scene* scene;

    float fpsTimer = 0;
    int fpsCounter = 0;

    void Init(ApplicationSettings settings);
    void Terminate();
};
