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
    class Object : public std::enable_shared_from_this<Object>
    {
        typedef std::unordered_set<std::string> TypeNameSet;

    public:
        Object(const ObjectID id) : m_id {id} {}
        virtual ~Object();

        virtual void tick(float delta_time);

        bool load(const ObjectInstanceRes& object_instance_res);
        bool Create(const ObjectInstanceRes& object_instance_res);
        void getInstanceRes(ObjectInstanceRes& out_object_instance_res) const;
        void save(const std::string& path, const std::string& name);

        ObjectID getID() const { return m_id; }

        void               setName(std::string name) { m_name = std::move(name); }
        const std::string& getName() const { return m_name; }
        void               setPath(std::string path) { m_definition_path = std::move(path); }
        const std::string& getPath() const { return m_definition_path; }

        std::weak_ptr<Object>                     getParent();
        ObjectID                                  getParentID() const;
        void                                      setParent(const std::weak_ptr<Object>& object);
        bool                                      hasParent() const;
        void                                      detachFromParent();
        void                                      addChild(const std::weak_ptr<Object>& object);
        void                                      removeChild(const std::weak_ptr<Object>& object);
        const std::vector<std::weak_ptr<Object>>& getChildren() const { return m_children; }

        bool isAlive() const;
        void destroy();

        bool                                              addComponent(Reflection::ReflectionPtr<Component> component);
        bool                                              hasComponent(const std::string& component_type_name) const;
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
        ObjectID    m_id {k_invalid_object_id};
        std::string m_name;
        std::string m_definition_path;

        bool m_isDestroy = false;

        std::vector<Reflection::ReflectionPtr<Component>> m_components;

        ObjectID              m_parentID = 0;
        std::weak_ptr<Object> m_parent;

        std::vector<std::weak_ptr<Object>> m_children;
    };
} // namespace LunarYue
