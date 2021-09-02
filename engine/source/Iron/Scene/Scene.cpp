#include "Scene.h"
#include "Transformation.h"
#include "NameComponent.h"
#include "Hierarchy.h"
#include "../Animation/Animator.h"
#include "../Audio/AudioCore.h"
#include "../Audio/AudioListener.h"
#include "../Core/Input.h"
#include "../Core/Time.h"
#include "../Rendering/SpriteRenderer.h"
#include "../Rendering/QuadRenderer.h"
#include "../Rendering/Renderer.h"
#include "../Physics/Physics.h"
#include "../Scene/TransformationSystem.h"
#include "../Scripting/ScriptingSystem.h"
#include "../UI/RectTransformation.h"

Scene::Scene()
{
    entitiesRegistry = new EntitiesRegistry();

    Physics::CreatePhysicsScene(entitiesRegistry);
    AudioCore::CreateAudioScene(entitiesRegistry);
    ScriptingSystem::InitScene(entitiesRegistry);

    hierarchySystem = new HierarchySystem();
    transformationSystem = new TransformationSystem();
    uiLayer = new UILayer(this);
    uiLayer->LoadDefaultResources();
    entitiesRegistry->RegisterSystem<HierarchyNode>(hierarchySystem);
    entitiesRegistry->RegisterSystem<Transformation>(transformationSystem);
    entitiesRegistry->RegisterSystem<RectTransformation>(transformationSystem);
}

Scene::~Scene()
{
    delete uiLayer;
    delete entitiesRegistry;
    delete transformationSystem;
    delete hierarchySystem;

    ScriptingSystem::TeminateScene();
    AudioCore::DeleteAudioScene();
    Physics::DeletePhysicsScene();
}

void Scene::CreateMainCamera()
{
    _mainCameraEntity = CreateEntity();
    auto& mainCamera = entitiesRegistry->AddComponent<Camera>(_mainCameraEntity);
    entitiesRegistry->GetComponent<Transformation>(_mainCameraEntity).SetPosition(glm::vec3(0.0f, 0.0f, 3.0f));
    mainCamera.SetHeight(3.0f);

    AudioCore::CreateAudioListener(_mainCameraEntity);
}

Camera& Scene::GetMainCamera()
{
    return entitiesRegistry->GetComponent<Camera>(_mainCameraEntity);
}

EntitiesRegistry* Scene::GetEntitiesRegistry()
{
    return entitiesRegistry;
}

UILayer* Scene::GetUILayer()
{
    return uiLayer;
}

EntityID Scene::CreateEntity()
{
    return CreateEntity("New entity", NULL_ENTITY);
}

EntityID Scene::CreateEntity(const char* name, EntityID parent)
{
    auto entity = CreateEmptyEntity();
    auto& nameComponent = entitiesRegistry->AddComponent<NameComponent>(entity);
    nameComponent.Name = name;
    entitiesRegistry->AddComponent<Transformation>(entity);
    entitiesRegistry->AddComponent<HierarchyNode>(entity);
    LinkChildToParent(entitiesRegistry, entity, parent);

    return entity;
}

EntityID Scene::CreateEntity(AsepriteData& data)
{
    auto entity = CreateEntity(data.Name, NULL_ENTITY);
    auto& sr = entitiesRegistry->AddComponent<SpriteRenderer>(entity);
    if (!data.Sprites.empty())
        sr.SetImage(data.Sprites[0]);
    auto& an = entitiesRegistry->AddComponent<Animator>(entity);
    for (auto& animation : data.Animations)
    {
        an.Animations.push_back(animation);
    }

    return entity;
}

EntityID Scene::CreateEmptyEntity()
{
    return entitiesRegistry->CreateNewEntity();
}

void Scene::DestroyEntity(EntityID entity)
{
    entitiesToDelete.push_back(entity);
}

void Scene::Update()
{
    // Poll UI events
    UIEvent uiEvent = Input::GetUIEvent();
    uiLayer->PollEvent(uiEvent);

    // Update scripts
    auto scripts = entitiesRegistry->GetComponentIterator<ScriptComponent>();
    int scriptsSize = scripts.Size();
    for (int i = 0; i < scriptsSize; ++i)
        if (scripts[i].IsAlive()) scripts[i].OnUpdate();

    // Update coroutines
    ScriptingSystem::UpdateCoroutines();

    if (Time::FixedUpdate())
    {
        for (int i = 0; i < scriptsSize; ++i)
            if (scripts[i].IsAlive()) scripts[i].OnFixedUpdate();

        // Apply transformations to physics objects and then simulate
        Physics::UpdatePhysicsTransformations();
        Physics::Simulate(Time::FixedDeltaTime());
        Physics::GetPhysicsTransformations();
        Physics::SendEvents();
    }

    for (int i = 0; i < scriptsSize; ++i)
        if (scripts[i].IsAlive()) scripts[i].OnLateUpdate();

    // Update inner components
    auto animators = entitiesRegistry->GetComponentIterator<Animator>();
    int animatorsSize = animators.Size();
    for (int i = 0; i < animatorsSize; ++i)
        if (animators[i].IsAlive()) animators[i].OnUpdate();

    auto audioSources = entitiesRegistry->GetComponentIterator<AudioSource>();
    int audioSourcesSize = audioSources.Size();
    for (int i = 0; i < audioSourcesSize; ++i)
        if (audioSources[i].IsAlive()) audioSources[i].OnUpdate();

    auto audioListeners = entitiesRegistry->GetComponentIterator<AudioListener>();
    int audioListenersSize = audioListeners.Size();
    for (int i = 0; i < audioListenersSize; ++i)
        if (audioListeners[i].IsAlive()) audioListeners[i].OnUpdate();

    // Update UI elements
    uiLayer->Update();

    // Clean destroyed entities
    CleanDestroyedEntities();

    // Sort hierarchy from parents to children and then apply transforms
    SortByHierarchy();
    UpdateGlobalTransformation();
    SortByDrawOrder();
    Renderer::OnBeforeRender(GetMainCamera());
    RefreshTransformation();

    Time::Update(); // TODO: should be independent from scene
}

void Scene::Draw()
{
    Renderer::Clear(Screen::GetColor());

    auto quadRenderers = entitiesRegistry->GetComponentIterator<QuadRenderer>();

    // Opaque pass
    for (int i = 0; i < quadRenderers.Size(); ++i)
        if (quadRenderers[i].Queue == RenderingQueue::Opaque)
            Renderer::Draw(quadRenderers[i]);

    // Transparent pass
    for (int i = quadRenderers.Size() - 1; i >=0; --i)
        if (quadRenderers[i].Queue == RenderingQueue::Transparent)
            Renderer::Draw(quadRenderers[i]);

    // Clear depth buffer before rendering UI
    Renderer::PrepareUIRender();
    // Draw UI on top
    uiLayer->Draw();

    Renderer::OnAfterRender();
}

void Scene::SortByHierarchy()
{
    // Sort all objects by hierarchy depth
    struct
    {
        bool operator()(const HierarchyNode& a, const HierarchyNode& b) const
        {
            return a.HierarchyDepth < b.HierarchyDepth || (a.HierarchyDepth == b.HierarchyDepth && a.NodeIndex < b.NodeIndex);
        }
    } DepthComparer;
    entitiesRegistry->SortComponents<HierarchyNode>(DepthComparer);
    entitiesRegistry->ApplyOrder<HierarchyNode, Transformation>();
    entitiesRegistry->ApplyOrder<HierarchyNode, RectTransformation>();
}

void Scene::UpdateGlobalTransformation()
{
    auto hierarchyNodes = entitiesRegistry->GetComponentIterator<HierarchyNode>();
    auto transformationsAccessor = entitiesRegistry->GetComponentAccessor<Transformation>();
    // Components to apply changed transformation
    auto srAccessor = entitiesRegistry->GetComponentAccessor<SpriteRenderer>();
    for (auto& hierarchyNode : hierarchyNodes)
    {
        if (transformationsAccessor.Has(hierarchyNode.Owner))
        {
            auto& transformation = transformationsAccessor.Get(hierarchyNode.Owner);
            transformation.UpdateTransformation(transformationsAccessor, hierarchyNode);

            bool transformationDirty = transformation.DidTransformationChange();
            if (transformationDirty && srAccessor.Has(hierarchyNode.Owner))
                srAccessor.Get(hierarchyNode.Owner).UpdateRenderer(transformation);
        }
    }
}

void Scene::RefreshTransformation()
{
    auto transformations = entitiesRegistry->GetComponentIterator<Transformation>();
    for (auto& transformation : transformations)
        transformation.RefreshTransformation();
}

void Scene::SortByDrawOrder()
{
    // Sort all objects by Z
    struct
    {
        bool operator()(QuadRenderer& a, QuadRenderer& b) const
        { return a.SortingOrder > b.SortingOrder; } // TODO: check direction
    } ZComparer;
    entitiesRegistry->SortComponents<QuadRenderer>(ZComparer);
}

void Scene::CleanDestroyedEntities()
{
    for (auto entity : entitiesToDelete)
    {
        if (entity != NULL_ENTITY)
            DestroyAndRemoveEntity(entity);
    }
    entitiesToDelete.clear();

    entitiesRegistry->ClearRemoved();
}

void Scene::CleanAllEntities()
{
    entitiesRegistry->CleanAllEntities();
}

void Scene::DestroyAndRemoveEntity(EntityID entity)
{
    DestroyEntityInner(entity);
}

void Scene::DestroyEntityInner(EntityID entity)
{
    entitiesRegistry->DeleteEntity(entity);
}
