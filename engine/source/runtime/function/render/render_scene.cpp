#include "runtime/function/render/render_scene.h"

#include "render_camera.h"

#include "runtime/function/render/render_helper.h"

namespace LunarYue
{
    void RenderScene::clear() {}

    void RenderScene::updateVisibleObjects(std::shared_ptr<RenderResource> render_resource, std::shared_ptr<RenderCamera> camera)
    {
        updateVisibleObjectsDirectionalLight(render_resource, camera);
        updateVisibleObjectsPointLight(render_resource);
        updateVisibleObjectsMainCamera(render_resource, camera);
        updateVisibleObjectsAxis(render_resource);
        updateVisibleObjectsParticle(render_resource);
    }

    void RenderScene::setVisibleNodesReference() {}

    GuidAllocator<GameObjectPartId>& RenderScene::getInstanceIdAllocator() { return m_instance_id_allocator; }

    GuidAllocator<MeshSourceDesc>& RenderScene::getMeshAssetIdAllocator() { return m_mesh_asset_id_allocator; }

    GuidAllocator<MaterialSourceDesc>& RenderScene::getMaterialAssetdAllocator() { return m_material_asset_id_allocator; }

    void RenderScene::addInstanceIdToMap(uint32_t instance_id, ObjectID go_id) { m_mesh_object_id_map[instance_id] = go_id; }

    ObjectID RenderScene::getGObjectIDByMeshID(uint32_t mesh_id) const
    {
        auto find_it = m_mesh_object_id_map.find(mesh_id);
        if (find_it != m_mesh_object_id_map.end())
        {
            return find_it->second;
        }
        return ObjectID();
    }

    void RenderScene::deleteEntityByGObjectID(ObjectID go_id)
    {
        for (auto it = m_mesh_object_id_map.begin(); it != m_mesh_object_id_map.end(); it++)
        {
            if (it->second == go_id)
            {
                m_mesh_object_id_map.erase(it);
                break;
            }
        }

        GameObjectPartId part_id = {go_id, 0};
        size_t           find_guid;
        if (m_instance_id_allocator.getElementGuid(part_id, find_guid))
        {
            for (auto it = m_render_entities.begin(); it != m_render_entities.end(); it++)
            {
                if (it->m_instance_id == find_guid)
                {
                    m_render_entities.erase(it);
                    break;
                }
            }
        }
    }

    void RenderScene::clearForLevelReloading()
    {
        m_instance_id_allocator.clear();
        m_mesh_object_id_map.clear();
        m_render_entities.clear();
    }

    void RenderScene::updateVisibleObjectsDirectionalLight(std::shared_ptr<RenderResource> render_resource, std::shared_ptr<RenderCamera> camera)
    {
        Matrix4x4 directional_light_proj_view = CalculateDirectionalLightCamera(*this, *camera);

        ClusterFrustum frustum = CreateClusterFrustumFromMatrix(directional_light_proj_view, -1.0, 1.0, -1.0, 1.0, 0.0, 1.0);

        for (const RenderEntity& entity : m_render_entities)
        {
            BoundingBox mesh_asset_bounding_box {entity.m_bounding_box.getMinCorner(), entity.m_bounding_box.getMaxCorner()};

            if (TiledFrustumIntersectBox(frustum, BoundingBoxTransform(mesh_asset_bounding_box, entity.m_model_matrix)))
            {}
        }
    }

    void RenderScene::updateVisibleObjectsPointLight(std::shared_ptr<RenderResource> render_resource)
    {

        std::vector<BoundingSphere> point_lights_bounding_spheres;
        uint32_t                    point_light_num = static_cast<uint32_t>(m_point_light_list.m_lights.size());
        point_lights_bounding_spheres.resize(point_light_num);
        for (size_t i = 0; i < point_light_num; i++)
        {
            point_lights_bounding_spheres[i].m_center = m_point_light_list.m_lights[i].m_position;
            point_lights_bounding_spheres[i].m_radius = m_point_light_list.m_lights[i].calculateRadius();
        }

        for (const RenderEntity& entity : m_render_entities)
        {
            BoundingBox mesh_asset_bounding_box {entity.m_bounding_box.getMinCorner(), entity.m_bounding_box.getMaxCorner()};

            bool intersect_with_point_lights = true;
            for (size_t i = 0; i < point_light_num; i++)
            {
                if (!BoxIntersectsWithSphere(BoundingBoxTransform(mesh_asset_bounding_box, entity.m_model_matrix), point_lights_bounding_spheres[i]))
                {
                    intersect_with_point_lights = false;
                    break;
                }
            }

            if (intersect_with_point_lights)
            {}
        }
    }

    void RenderScene::updateVisibleObjectsMainCamera(std::shared_ptr<RenderResource> render_resource, std::shared_ptr<RenderCamera> camera)
    {

        Matrix4x4 view_matrix      = camera->getViewMatrix();
        Matrix4x4 proj_matrix      = camera->getPersProjMatrix();
        Matrix4x4 proj_view_matrix = proj_matrix * view_matrix;

        ClusterFrustum f = CreateClusterFrustumFromMatrix(proj_view_matrix, -1.0, 1.0, -1.0, 1.0, 0.0, 1.0);

        for (const RenderEntity& entity : m_render_entities)
        {
            BoundingBox mesh_asset_bounding_box {entity.m_bounding_box.getMinCorner(), entity.m_bounding_box.getMaxCorner()};

            if (TiledFrustumIntersectBox(f, BoundingBoxTransform(mesh_asset_bounding_box, entity.m_model_matrix)))
            {}
        }
    }

    void RenderScene::updateVisibleObjectsAxis(std::shared_ptr<RenderResource> render_resource)
    {
        if (m_render_axis.has_value())
        {}
    }

    void RenderScene::updateVisibleObjectsParticle(std::shared_ptr<RenderResource> render_resource)
    {
        // TODO
    }
} // namespace LunarYue
