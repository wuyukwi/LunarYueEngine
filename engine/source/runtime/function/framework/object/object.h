#pragma once

#include "runtime/function/framework/component/component.h"
#include "runtime/function/framework/object/object_id_allocator.h"

#include "runtime/resource/res_type/common/object.h"

#include <memory>
#include <optional>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

class TransformComponent;

namespace LunarYue
{
    class Object : public std::enable_shared_from_this<Object>
    {
        typedef std::unordered_set<std::string> TypeNameSet;

    public:
        Object(const ObjectID id) : m_id {id} {}
        virtual ~Object();

        // Perform the update logic for the object
        virtual void tick(float delta_time);

        // Load the object
        bool load(const ObjectInstanceRes& object_instance_res);
        // Create the object
        bool Create(const ObjectInstanceRes& object_instance_res);
        // Get the instance resource of the object
        void getInstanceRes(ObjectInstanceRes& out_object_instance_res) const;
        // Save the object
        void save(const std::string& path, const std::string& name);

        // Get the ID of the object
        ObjectID getID() const { return m_id; }

        // Set the name of the object
        void setName(std::string name) { m_name = std::move(name); }
        // Get the name of the object
        const std::string& getName() const { return m_name; }
        // Set the path of the object
        void setPath(std::string path) { m_definition_path = std::move(path); }
        // Get the path of the object
        const std::string& getPath() const { return m_definition_path; }
        // Get the tag of the object
        const std::string& getTag() const { return m_tag; }
        // Set the tag of the object
        void setTag(std::string tag) { m_tag = std::move(tag); }

        // Get a weak pointer to the parent object
        std::weak_ptr<Object> getParent();
        // Get the ID of the parent object
        ObjectID getParentID() const;
        // Set the parent object
        void setParent(const std::weak_ptr<Object>& object);
        // Check if the object has a parent
        bool hasParent() const;
        // Detach from the parent object
        void detachFromParent();
        // Add a child object
        void addChild(const std::weak_ptr<Object>& object);
        // Remove a child object
        void removeChild(const std::weak_ptr<Object>& object);
        // Get the list of child objects
        const std::vector<std::weak_ptr<Object>>& getChildren() const { return m_children; }

        // Check if the object is alive
        bool isAlive() const;
        // Destroy the object
        void destroy();
        // Set the active state of the object
        void setActive(bool p_active);
        // Check if the object is self-active
        bool isSelfActive() const { return m_active; };
        // Check if the object is active
        bool isActive() const;

        // Add a component
        template<typename T, typename... Args>
        T& addComponent(Args&&... p_args)
        {
            static_assert(std::is_base_of_v<Component, T>, "T should derive from Component");

            // Check if the component already exists
            if (auto found = getComponent<T>(); found != nullptr)
            {
                // Return the existing component
                return *found;
            }

            // Create a new instance of the component
            Reflection::ReflectionPtr<T> component = LunarYue_REFLECTION_NEW(T, std::forward<Args>(p_args)...);

            // Bind the component to the object's pointer
            component->postLoadResource(weak_from_this());

            // Add the component to the component list
            m_components.push_back(component);

            // Return a reference to the newly added component
            return *component;
        }

        // Remove a component
        template<typename T>
        bool removeComponent()
        {
            static_assert(std::is_base_of_v<Component, T>, "T should derive from Component");
            static_assert(!std::is_same_v<TransformComponent, T>, "You can't remove a Transform from an object");

            for (auto it = m_components.begin(); it != m_components.end(); ++it)
            {
                if (std::is_same_v<T, decltype(*it->getPtr())>)
                {
                    m_components.erase(it);
                    return true;
                }
            }
            return false;
        }

        // Get a component
        template<typename T>
        T* getComponent()
        {
            static_assert(std::is_base_of_v<Component, T>, "T should derive from Component");

            for (auto& component : m_components)
            {
                if (std::is_same_v<T, decltype(*component.getPtr())>)
                {
                    return static_cast<T*>(component.getPtr());
                }
            }
            return nullptr;
        }

        // Get a component (const version)
        template<typename T>
        const T* getComponentConst()
        {
            static_assert(std::is_base_of_v<Component, T>, "T should derive from Component");

            for (const auto& component : m_components)
            {
                if (std::is_same_v<T, decltype(*component.getPtr())>)
                {
                    return static_cast<const T*>(component.getPtr());
                }
            }
            return nullptr;
        }

        // Check if a specific component type exists
        template<typename T>
        bool hasComponent()
        {
            return getComponentConst<T>() ? true : false;
        }
        bool hasComponent(const std::string& component_type_name) const;

        // Get all components
        std::vector<Reflection::ReflectionPtr<Component>> getComponents() { return m_components; }

        template<typename TComponent>
        TComponent* tryGetComponentFormTypeName(const std::string& component_type_name)
        {
            for (auto& component : m_components)
            {
                if (component.getTypeName() == component_type_name)
                {
                    return static_cast<TComponent*>(component.getPtr());
                }
            }

            return nullptr;
        }

        template<typename TComponent>
        const TComponent* tryGetComponentConstFormTypeName(const std::string& component_type_name) const
        {
            for (const auto& component : m_components)
            {
                if (component.getTypeName() == component_type_name)
                {
                    return static_cast<const TComponent*>(component.getPtr());
                }
            }
            return nullptr;
        }

#define tryGetComponentFormTypeName(COMPONENT_TYPE) tryGetComponentFormTypeName<COMPONENT_TYPE>(#COMPONENT_TYPE)
#define tryGetComponentConstFormTypeName(COMPONENT_TYPE) tryGetComponentConstFormTypeName<const COMPONENT_TYPE>(#COMPONENT_TYPE)

    protected:
        ObjectID    m_id {k_invalid_object_id}; // ID of the object
        std::string m_name;                     // Name of the object
        std::string m_definition_path;          // Path of the object's definition

        std::string m_tag; // Tag of the object

        bool m_destroy = false; // Destruction flag of the object
        bool m_active  = true;  // Active state flag of the object

        std::vector<Reflection::ReflectionPtr<Component>> m_components; // List of components associated with the object

        ObjectID              m_parentID = 0; // ID of the parent object
        std::weak_ptr<Object> m_parent;       // Weak reference to the parent object

        std::vector<std::weak_ptr<Object>> m_children; // List of weak references to child objects
    };
} // namespace LunarYue
