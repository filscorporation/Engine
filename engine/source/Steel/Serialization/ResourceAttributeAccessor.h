#pragma once

#include "SerializationContext.h"
#include "Steel/Common/StringUtils.h"
#include "Steel/EntityComponentSystem/Entity.h"
#include "Steel/Resources/ResourceID.h"

// Attribute accessor for resource links
template <typename T, typename U>
class ResourceAttributeAccessor : public AttributeAccessorBase
{
public:
    typedef U (T::*GetFunction)() const;
    typedef void (T::*SetFunction)(U);

    ResourceAttributeAccessor(GetFunction getFunction, SetFunction setFunction, ResourceTypes::ResourceType resourceType)
            : _getFunction(getFunction), _setFunction(setFunction)
    {
        _resourceType = resourceType;
    }

    void FromString(Serializable* object, const std::string& line, SerializationContext& context) override
    {
        Set(object, static_cast<U>(context.GetResource(_resourceType, StringUtils::FromString<ResourceID>(line))));
    }

    std::string ToString(Serializable* object, SerializationContext& context) const override
    {
        return StringUtils::ToString(context.GetResourceID(Get(object)));
    }

    void Serialize(Serializable* object, const std::string& name, YAML::Node& node, SerializationContext& context) override
    {
        node[name] = context.GetResourceID(Get(object));
    }

    void Deserialize(Serializable* object, const std::string& name, YAML::Node& node, SerializationContext& context) override
    {
        Set(object, static_cast<U>(context.GetResource(_resourceType, node[name].as<ResourceID>())));
    }

private:
    U Get(Serializable* object) const
    {
        T* castedObject = static_cast<T*>(object);
        return (castedObject->*_getFunction)();
    }

    void Set(Serializable* object, const U& value) const
    {
        T* castedObject = static_cast<T*>(object);
        (castedObject->*_setFunction)(value);
    }

    GetFunction _getFunction;
    SetFunction _setFunction;
    ResourceTypes::ResourceType _resourceType = ResourceTypes::Undefined;
};
