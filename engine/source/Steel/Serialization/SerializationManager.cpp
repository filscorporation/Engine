#include "SerializationManager.h"
#include "TypeInfoStorage.h"
#include "Steel/Core/Application.h"
#include "Steel/Core/Log.h"
#include "Steel/Core/Components.h"
#include "Steel/Resources/ResourcesManager.h"
#include "Steel/Scene/Scene.h"

std::unordered_map<ComponentTypeID, std::vector<AttributeInfo>> SerializationManager::_attributesInfo;

void SerializationManager::Init()
{
    RegisterComponents();

    Log::LogDebug("Serialization manager initialized");
}

void SerializationManager::Terminate()
{
    for (auto& classInfo : _attributesInfo)
    {
        for (auto& info : classInfo.second)
        {
            info.Clear();
        }
    }
    _attributesInfo.clear();

    TypeInfoStorage::Clear();
}

void SerializationManager::SerializeScene(Scene* scene, const std::string& filePath)
{
    YAML::Node node;
    std::string fullPathString = RESOURCES_PATH;
    fullPathString += filePath;

    if (!SerializeScene(scene, node))
    {
        Log::LogError("Error serializing scene");
        return;
    }

    std::ofstream fout(fullPathString);
    fout << node;

    Log::LogInfo("Saved scene " + filePath);
}

Scene* SerializationManager::DeserializeScene(const std::string& filePath)
{
    std::string fullPathString = RESOURCES_PATH;
    fullPathString += filePath;

    std::ifstream infile(fullPathString);
    if (!infile.good())
    {
        Log::LogError("Error loading scene: file {0} does not exist", fullPathString);
        return nullptr;
    }

    YAML::Node node = YAML::Load(infile);
    auto scene = new Scene("");

    if (!DeserializeScene(scene, node))
    {
        Log::LogError("Error serializing scene");
        return nullptr;
    }

    Log::LogInfo("Loaded scene " + filePath);

    return scene;
}

Scene* SerializationManager::CopyScene(Scene* sceneFrom)
{
    auto scene = new Scene("");

    if (!CopySceneInner(sceneFrom, scene))
    {
        Log::LogError("Error copying the scene");
        return nullptr;
    }

    return scene;
}

void SerializationManager::RegisterAttribute(ComponentTypeID classTypeID, const AttributeInfo& attributeInfo)
{
    _attributesInfo[classTypeID].emplace_back(attributeInfo);
}

std::vector<AttributeInfo>& SerializationManager::GetAttributes(ComponentTypeID classTypeID)
{
    return _attributesInfo[classTypeID];
}

bool SerializationManager::SerializeScene(Scene* scene, YAML::Node& node)
{
    SerializationContext context;
    context.SerializedScene = scene;
    context.ResourcesSource = Application::Context()->Resources;

    node["name"] = scene->_name;
    node["mainCamera"] = scene->_mainCameraEntity;
    node["childrenCount"] = scene->GetChildrenCount();
    node["firstChildNode"] = scene->GetFirstChildNode();

    auto entitiesNode = node["entities"];

    for (int i = 0; i < scene->entitiesRegistry->EntityGetMaxID(); ++i)
    {
        EntityID entity = scene->entitiesRegistry->EntityActual(i);

        if (scene->entitiesRegistry->EntityExists(entity))
        {
            YAML::Node entityNode;
            entityNode["uuid"] = scene->entitiesRegistry->HasComponent<IDComponent>(entity)
                    ? scene->entitiesRegistry->GetComponent<IDComponent>(entity).GetUUID()
                    : NULL_UUID;
            entityNode["state"] = (uint32_t)scene->entitiesRegistry->EntityGetState(entity);
            YAML::Node componentsNode = entityNode["components"];

            std::vector<std::pair<ComponentTypeID, void*>> rawData;
            scene->entitiesRegistry->GetAllComponentsForEntity(entity, rawData);
            for (auto dataPair : rawData)
            {
                auto object = static_cast<Serializable*>(dataPair.second);
                auto typeInfo = scene->entitiesRegistry->GetTypeInfoByID(dataPair.first);

                YAML::Node componentNode;

                if (_attributesInfo.find(typeInfo->ID) != _attributesInfo.end())
                {
                    for (auto attribute : _attributesInfo[typeInfo->ID])
                    {
                        attribute.Serialize(object, componentNode, context);
                    }
                }

                componentsNode[typeInfo->TypeName] = componentNode;
            }

            entitiesNode.push_back(entityNode);
        }
    }

    return true;
}

bool SerializationManager::DeserializeScene(Scene* scene, YAML::Node& node)
{
    SerializationContext context;
    context.SerializedScene = scene;
    context.ResourcesSource = Application::Context()->Resources;

    scene->_name = node["name"].as<std::string>();
    scene->_mainCameraEntity = node["mainCamera"].as<EntityID>();
    scene->SetChildrenCount(node["childrenCount"].as<uint32_t>());
    scene->SetFirstChildNode(node["firstChildNode"].as<EntityID>());

    auto entitiesNode = node["entities"];
    Log::LogInfo("Entities count loaded: {0}", entitiesNode.size());
    if (entitiesNode)
    {
        // Pre pass to fill UUID to EntityID map (for link attributes)
        for (auto entityNode : entitiesNode)
        {
            auto uuid = entityNode["uuid"].as<UUID>();
            if (uuid != NULL_UUID)
            {
                auto entity = scene->entitiesRegistry->CreateNewEntity();
                scene->SetEntityByUUID(uuid, entity);
            }
        }

        // Main pass
        for (auto entityNode : entitiesNode)
        {
            auto uuid = entityNode["uuid"].as<UUID>();
            auto entity = scene->GetEntityByUUID(uuid);
            if (entity == NULL_ENTITY)
                entity = scene->entitiesRegistry->CreateNewEntity();
            auto entityState = (EntityStates::EntityState)entityNode["state"].as<uint32_t>();

            if (!scene->entitiesRegistry->RestoreEntityState(entity, entityState))
            {
                Log::LogError("Entity {0} state was not restored correctly", entity);
                continue;
            }

            auto componentsNode = entityNode["components"];
            if (componentsNode)
            {
                for (auto componentNode : componentsNode)
                {
                    auto typeName = componentNode.first.as<std::string>();
                    auto typeID = Math::StringHash(typeName.c_str());

                    // This will add component map to registry, so we can add component of deserialized type without template arguments
                    if (!TypeInfoStorage::CheckTypeRegistered(typeID, scene->entitiesRegistry))
                    {
                        Log::LogError("Type {0} not registered", typeName);
                        continue;
                    }
                    auto object = static_cast<Serializable*>(scene->entitiesRegistry->RestoreComponent(typeID, entity));
                    if (!object)
                    {
                        Log::LogError("Object of type {0} was not restored properly", typeName);
                        continue;
                    }

                    if (_attributesInfo.find(typeID) != _attributesInfo.end())
                    {
                        for (auto attribute : _attributesInfo[typeID])
                        {
                            attribute.Deserialize(object, componentNode.second, context);
                        }
                    }
                }
            }
        }
    }

    return true;
}

bool SerializationManager::CopySceneInner(Scene* sceneFrom, Scene* sceneTo)
{
    SerializationContext contextFrom;
    contextFrom.SerializedScene = sceneFrom;
    contextFrom.ResourcesSource = Application::Context()->Resources;

    SerializationContext contextTo;
    contextTo.SerializedScene = sceneTo;
    contextTo.ResourcesSource = Application::Context()->Resources;

    sceneTo->_name = sceneFrom->_name;
    sceneTo->_mainCameraEntity = sceneFrom->_mainCameraEntity;
    sceneTo->SetChildrenCount(sceneFrom->GetChildrenCount());
    sceneTo->SetFirstChildNode(sceneFrom->GetFirstChildNode());

    // Pre pass to fill UUID to EntityID map (for link attributes)
    for (int i = 0; i < sceneFrom->entitiesRegistry->EntityGetMaxID(); ++i)
    {
        EntityID entityFrom = sceneFrom->entitiesRegistry->EntityActual(i);
        auto uuid = sceneFrom->entitiesRegistry->HasComponent<IDComponent>(entityFrom)
                    ? sceneFrom->entitiesRegistry->GetComponent<IDComponent>(entityFrom).GetUUID()
                    : NULL_UUID;
        if (uuid != NULL_UUID)
        {
            auto entityTo = sceneTo->entitiesRegistry->CreateNewEntity();
            sceneTo->SetEntityByUUID(uuid, entityTo);
        }
    }

    // Main pass
    for (int i = 0; i < sceneFrom->entitiesRegistry->EntityGetMaxID(); ++i)
    {
        EntityID entityFrom = sceneFrom->entitiesRegistry->EntityActual(i);

        if (sceneFrom->entitiesRegistry->EntityExists(entityFrom))
        {
            auto uuid = sceneFrom->entitiesRegistry->HasComponent<IDComponent>(entityFrom)
                        ? sceneFrom->entitiesRegistry->GetComponent<IDComponent>(entityFrom).GetUUID()
                        : NULL_UUID;
            auto entityTo = sceneTo->GetEntityByUUID(uuid);
            if (entityTo == NULL_ENTITY)
                entityTo = sceneTo->entitiesRegistry->CreateNewEntity();
            auto entityState = sceneFrom->entitiesRegistry->EntityGetState(entityFrom);

            if (!sceneTo->entitiesRegistry->RestoreEntityState(entityTo, entityState))
            {
                Log::LogError("Entity {0} state was not restored correctly", entityTo);
                continue;
            }

            std::vector<std::pair<ComponentTypeID, void*>> rawData;
            sceneFrom->entitiesRegistry->GetAllComponentsForEntity(entityFrom, rawData);
            for (auto dataPair : rawData)
            {
                auto objectFrom = static_cast<Serializable*>(dataPair.second);
                auto typeInfo = sceneFrom->entitiesRegistry->GetTypeInfoByID(dataPair.first);

                // This will add component map to registry, so we can add component of deserialized type without template arguments
                if (!TypeInfoStorage::CheckTypeRegistered(typeInfo->ID, sceneTo->entitiesRegistry))
                {
                    Log::LogError("Type {0} not registered", typeInfo->TypeName);
                    continue;
                }
                auto objectTo = static_cast<Serializable*>(sceneTo->entitiesRegistry->RestoreComponent(typeInfo->ID, entityTo));
                if (!objectTo)
                {
                    Log::LogError("Object of type {0} was not restored properly", typeInfo->TypeName);
                    continue;
                }

                if (_attributesInfo.find(typeInfo->ID) != _attributesInfo.end())
                {
                    for (auto attribute : _attributesInfo[typeInfo->ID])
                    {
                        attribute.Copy(objectFrom, objectTo, contextFrom, contextTo);
                    }
                }
            }
        }
    }

    return true;
}

void SerializationManager::RegisterComponents()
{
    Animator::RegisterType();
    AudioListener::RegisterType();
    AudioSource::RegisterType();
    BoxCollider::RegisterType();
    CircleCollider::RegisterType();
    RigidBody::RegisterType();
    Camera::RegisterType();
    MeshRenderer::RegisterType();
    SpriteRenderer::RegisterType();
    HierarchyNode::RegisterType();
    IDComponent::RegisterType();
    NameComponent::RegisterType();
    Transformation::RegisterType();
    ScriptComponent::RegisterType();
    UIButton::RegisterType();
    UICheckBox::RegisterType();
    UIClipping::RegisterType();
    UIImage::RegisterType();
    UIInputField::RegisterType();
    UILayoutElement::RegisterType();
    UILayoutGroup::RegisterType();
    UITabs::RegisterType();
    UIText::RegisterType();
    RectTransformation::RegisterType();
    UIEventHandler::RegisterType();
}
