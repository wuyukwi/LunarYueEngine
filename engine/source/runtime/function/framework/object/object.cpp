#include "runtime/function/framework/object/object.h"

#include "runtime/engine.h"

#include "runtime/core/meta/reflection/reflection.h"

#include "runtime/resource/asset_manager/asset_manager.h"

#include "runtime/function/framework/component/component.h"
#include "runtime/function/framework/component/transform/transform_component.h"
#include "runtime/function/global/global_context.h"

#include <cassert>
#include <unordered_set>

#include "_generated/serializer/all_serializer.h"

namespace LunarYue
{
    bool shouldComponentTick(const std::string& component_type_name)
    {
        if (g_is_editor_mode)
        {
            return g_editor_tick_component_types.find(component_type_name) != g_editor_tick_component_types.end();
        }
        else
        {
            return true;
        }
    }

    Object::~Object()
    {
        for (auto& component : m_components)
        {
            LunarYue_REFLECTION_DELETE(component);
        }
        m_components.clear();
    }

    void Object::tick(float delta_time)
    {
        m_children.erase(std::remove_if(m_children.begin(), m_children.end(), [](const std::weak_ptr<Object>& child) { return child.expired(); }),
                         m_children.end());

        for (auto& component : m_components)
        {
            if (shouldComponentTick(component.getTypeName()))
            {
                component->tick(delta_time);
            }
        }
    }

    bool Object::addComponent(Reflection::ReflectionPtr<Component> component)
    {
        if (component)
        {
            component->postLoadResource(weak_from_this());

            m_components.push_back(component);
        }

        return false;
    }

    bool Object::hasComponent(const std::string& component_type_name) const
    {
        for (const auto& component : m_components)
        {
            if (component.getTypeName() == component_type_name)
                return true;
        }

        return false;
    }

    bool Object::load(const ObjectInstanceRes& object_instance_res)
    {
        // clear old components
        m_components.clear();

        setName(object_instance_res.m_name);

        // load object instanced components
        m_components = object_instance_res.m_instanced_components;
        for (auto component : m_components)
        {
            if (component)
            {
                component->postLoadResource(weak_from_this());
            }
        }

        // load object definition components
        m_definition_path = object_instance_res.m_definition;

        ObjectDefinitionRes definition_res;

        const bool is_loaded_success = g_runtime_global_context.m_asset_manager->loadAsset(m_definition_path, definition_res);
        if (!is_loaded_success)
            return false;

        for (auto loaded_component : definition_res.m_components)
        {
            const std::string type_name = loaded_component.getTypeName();
            // don't create component if it has been instanced
            if (hasComponent(type_name))
                continue;

            loaded_component->postLoadResource(weak_from_this());

            m_components.push_back(loaded_component);
        }

        return true;
    }

    bool Object::Create(const ObjectInstanceRes& object_instance_res)
    {
        // clear old components
        m_components.clear();

        setName(object_instance_res.m_name);

        // load object instanced components
        m_components = object_instance_res.m_instanced_components;
        for (auto component : m_components)
        {
            if (component)
            {
                component->postLoadResource(weak_from_this());
            }
        }

        // load object definition components
        m_definition_path = object_instance_res.m_definition;

        ObjectDefinitionRes definition_res;

        const bool is_loaded_success = g_runtime_global_context.m_asset_manager->loadAsset(m_definition_path, definition_res);
        if (!is_loaded_success)
            return false;

        for (auto loaded_component : definition_res.m_components)
        {
            const std::string type_name = loaded_component.getTypeName();
            // don't create component if it has been instanced
            if (hasComponent(type_name))
                continue;

            loaded_component->postLoadResource(weak_from_this());

            m_components.push_back(loaded_component);
        }

        return true;
    }

    void Object::getInstanceRes(ObjectInstanceRes& out_object_instance_res) const
    {
        out_object_instance_res.m_name       = m_name;
        out_object_instance_res.m_definition = m_definition_path;

        out_object_instance_res.m_instanced_components = m_components;
    }

    void Object::save(const std::string& path, const std::string& name)
    {
        ObjectDefinitionRes res;
        res.m_components = m_components;

        const auto save_path = path + "/" + name + ".object.json";

        const bool is_save_success = g_runtime_global_context.m_asset_manager->saveAsset(res, save_path);

        if (is_save_success == false)
        {
            LOG_ERROR("failed to save {}", path);
        }
        else
        {
            LOG_INFO("level save succeed");
            m_definition_path = save_path;
            m_name            = name;
        }
    }

    std::weak_ptr<Object> Object::getParent()
    {
        if (!m_parent.expired())
            return m_parent;

        return {};
    }

    ObjectID Object::getParentID() const
    {
        if (!m_parent.expired())
            return m_parentID;

        return k_invalid_object_id;
    }

    void Object::setParent(const std::weak_ptr<Object>& object)
    {
        m_parent   = object;
        m_parentID = object.lock()->getID();
        m_parent.lock()->addChild(shared_from_this());
    }

    bool Object::hasParent() const
    {
        if (m_parent.expired())
            return false;

        return true;
    }

    void Object::detachFromParent()
    {
        if (!m_parent.expired())
        {
            m_parent.lock()->removeChild(shared_from_this());
            m_parent.reset();
            m_parentID = k_invalid_object_id;
        }
    }

    void Object::addChild(const std::weak_ptr<Object>& object) { m_children.emplace_back(object); }

    void Object::removeChild(const std::weak_ptr<Object>& object)
    {
        const auto it = std::find_if(m_children.begin(), m_children.end(), [&](const std::weak_ptr<Object>& child) {
            return !object.expired() && child.lock() == object.lock();
        });

        if (it != m_children.end())
        {
            m_children.erase(it);
        }
    }

    bool Object::isAlive() const { return !m_isDestroy; }

    void Object::destroy() { m_isDestroy = true; }
} // namespace LunarYue