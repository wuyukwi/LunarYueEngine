#pragma once

#include "runtime/function/framework/component/component.h"
#include "runtime/function/framework/object/object_id_allocator.h"

#include "runtime/resource/res_type/common/object.h"

#include <memory>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

namespace LunarYue
{
    /// Object : Game Object base class
    class Object : public std::enable_shared_from_this<Object>
    {
        typedef std::unordered_set<std::string> TypeNameSet;

    public:
        Object(const GObjectID id) : m_id {id} {}
        virtual ~Object();

        virtual void tick(float delta_time);

        bool load(const ObjectInstanceRes& object_instance_res);
        bool Create(const ObjectInstanceRes& object_instance_res);
        void getInstanceRes(ObjectInstanceRes& out_object_instance_res) const;
        void save(const std::string& path, const std::string& name);

        GObjectID getID() const { return m_id; }

        void               setName(std::string name) { m_name = std::move(name); }
        const std::string& getName() const { return m_name; }
        void               setPath(std::string path) { m_definition_path = std::move(path); }
        const std::string& getPath() const { return m_definition_path; }

        bool addComponent(Reflection::ReflectionPtr<Component> component);

        bool hasComponent(const std::string& component_type_name) const;

        std::vector<Reflection::ReflectionPtr<Component>> getComponents() { return m_components; }

        template<typename TComponent>
        TComponent* tryGetComponent(const std::string& component_type_name)
        {
            for (auto& component : m_components)
            {
                if (component.getTypeName() == component_type_name)
                {
                    return static_cast<TComponent*>(component.operator->());
                }
            }

            return nullptr;
        }

        template<typename TComponent>
        const TComponent* tryGetComponentConst(const std::string& component_type_name) const
        {
            for (const auto& component : m_components)
            {
                if (component.getTypeName() == component_type_name)
                {
                    return static_cast<const TComponent*>(component.operator->());
                }
            }
            return nullptr;
        }

#define tryGetComponent(COMPONENT_TYPE) tryGetComponent<COMPONENT_TYPE>(#COMPONENT_TYPE)
#define tryGetComponentConst(COMPONENT_TYPE) tryGetComponentConst<const COMPONENT_TYPE>(#COMPONENT_TYPE)

    protected:
        GObjectID   m_id {k_invalid_gobject_id};
        std::string m_name;
        std::string m_definition_path;

        // we have to use the ReflectionPtr due to that the components need to be reflected
        // in editor, and it's polymorphism
        std::vector<Reflection::ReflectionPtr<Component>> m_components;
    };
} // namespace LunarYue
