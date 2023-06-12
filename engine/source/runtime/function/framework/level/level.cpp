#include "runtime/function/framework/level/level.h"

#include "runtime/core/base/macro.h"

#include "runtime/resource/asset_manager/asset_manager.h"
#include "runtime/resource/res_type/common/level.h"

#include "runtime/engine.h"
#include "runtime/function/character/character.h"
#include "runtime/function/framework/object/object.h"
#include "runtime/function/particle/particle_manager.h"
#include "runtime/function/physics/physics_manager.h"
#include "runtime/function/physics/physics_scene.h"
#include "runtime/platform/path/path.h"
#include <limits>

namespace LunarYue
{
    void Level::clear()
    {
        m_current_active_character.reset();
        m_objects.clear();

        ASSERT(g_runtime_global_context.m_physics_manager);
        g_runtime_global_context.m_physics_manager->deletePhysicsScene(m_physics_scene);
    }

    ObjectID Level::createObject(const ObjectInstanceRes& object_instance_res)
    {
        ObjectID object_id = ObjectIDAllocator::alloc();
        ASSERT(object_id != k_invalid_object_id);

        auto object = std::make_shared<Object>(object_id);

        bool is_loaded = object->load(object_instance_res);
        if (is_loaded)
        {
            m_objects.emplace(object_id, object);
        }
        else
        {
            LOG_ERROR("loading object " + object_instance_res.m_name + " failed");
            return k_invalid_object_id;
        }
        return object_id;
    }

    ObjectID Level::createEmptyObject(const std::string& path, const std::string& name)
    {
        ObjectID object_id = ObjectIDAllocator::alloc();
        ASSERT(object_id != k_invalid_object_id);

        auto object = std::make_shared<Object>(object_id);

        object->setName("new object");

        auto component = LunarYue_REFLECTION_NEW(TransformComponent);
        object->addComponent(component);

        object->save(path, name);

        // const bool is_save_success = g_runtime_global_context.m_asset_manager->saveAsset(output_level_res, path);

        //  if (is_save_success == false)
        //  {
        //      LOG_ERROR("failed to save {}", m_level_res_url);
        //  }
        //  else
        //  {
        //      LOG_INFO("level save succeed");
        //  }*/

        m_objects.emplace(object_id, object);

        LOG_INFO("create object " + name);

        return object_id;
    }

    bool Level::load(const std::string& level_res_url)
    {
        LOG_INFO("loading level: {}", level_res_url);

        m_level_res_url = level_res_url;

        LevelRes   level_res;
        const bool is_load_success = g_runtime_global_context.m_asset_manager->loadAsset(level_res_url, level_res);
        if (is_load_success == false)
        {
            return false;
        }

        ASSERT(g_runtime_global_context.m_physics_manager);
        m_physics_scene = g_runtime_global_context.m_physics_manager->createPhysicsScene(level_res.m_gravity);
        ParticleEmitterIDAllocator::reset();

        for (const ObjectInstanceRes& object_instance_res : level_res.m_objects)
        {
            createObject(object_instance_res);
        }

        // create active character
        for (const auto& object_pair : m_objects)
        {
            std::shared_ptr<Object> object = object_pair.second;
            if (object == nullptr)
                continue;

            if (level_res.m_character_name == object->getName())
            {
                m_current_active_character = std::make_shared<Character>(object);
                break;
            }
        }

        m_is_loaded = true;

        LOG_INFO("level load succeed");

        return true;
    }

    void Level::create(const std::string& level_res_url)
    {
        LOG_INFO("create level: {}");

        // m_level_res_url = level_res_url;

        const LevelRes level_res {};

        ASSERT(g_runtime_global_context.m_physics_manager);
        m_physics_scene = g_runtime_global_context.m_physics_manager->createPhysicsScene(level_res.m_gravity);
        ParticleEmitterIDAllocator::reset();

        m_is_loaded = g_runtime_global_context.m_asset_manager->saveAsset(level_res, level_res_url);

        LOG_INFO("level create succeed");
    }

    void Level::unload()
    {
        clear();
        LOG_INFO("unload level: {}", m_level_res_url);
    }

    bool Level::save()
    {
        LOG_INFO("saving level: {}", m_level_res_url);

        LevelRes output_level_res;

        const size_t                    object_cout    = m_objects.size();
        std::vector<ObjectInstanceRes>& output_objects = output_level_res.m_objects;
        output_objects.resize(object_cout);

        size_t object_index = 0;
        for (const auto& id_object_pair : m_objects)
        {
            if (id_object_pair.second)
            {
                id_object_pair.second->getInstanceRes(output_objects[object_index]);
                ++object_index;
            }
        }

        const bool is_save_success = g_runtime_global_context.m_asset_manager->saveAsset(output_level_res, m_level_res_url);

        if (is_save_success == false)
        {
            LOG_ERROR("failed to save {}", m_level_res_url);
        }
        else
        {
            LOG_INFO("level save succeed");
        }

        return is_save_success;
    }

    void Level::tick(float delta_time)
    {
        if (!m_is_loaded)
        {
            return;
        }

        std::vector<ObjectID> objectsToRelease;

        for (const auto& id_object_pair : m_objects)
        {
            assert(id_object_pair.second);
            if (id_object_pair.second)
            {
                if (!id_object_pair.second->isAlive())
                {
                    objectsToRelease.emplace_back(id_object_pair.first);
                }
                else
                {
                    id_object_pair.second->tick(delta_time);
                }
            }
        }

        for (const ObjectID& id : objectsToRelease)
        {
            deleteGObjectByID(id);
        }

        std::shared_ptr<PhysicsScene> physics_scene = m_physics_scene.lock();
        if (physics_scene)
        {
            physics_scene->tick(delta_time);
        }
    }

    std::weak_ptr<Object> Level::getGObjectByID(ObjectID id) const
    {
        const auto iter = m_objects.find(id);
        if (iter != m_objects.end())
        {
            return iter->second;
        }

        return {};
    }

    void Level::deleteGObjectByID(ObjectID go_id)
    {
        const auto iter = m_objects.find(go_id);
        if (iter != m_objects.end())
        {
            std::shared_ptr<Object> object = iter->second;
            if (object)
            {
                if (m_current_active_character && m_current_active_character->getObjectID() == object->getID())
                {
                    m_current_active_character->setObject(nullptr);
                }

                object->detachFromParent();
            }
        }

        m_objects.erase(go_id);
    }

} // namespace LunarYue
