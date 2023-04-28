#pragma once

#include "runtime/function/render/render_entity.h"
#include "runtime/function/render/render_guid_allocator.h"
#include "runtime/function/render/render_swap_context.h"
#include "runtime/function/render/render_type.h"

#include <array>
#include <memory>
#include <optional>

namespace LunarYue
{
    class WindowSystem;
    class RHI;
    class RenderResourceBase;
    class RenderPipelineBase;
    class RenderScene;
    class RenderCamera;
    class WindowUI;
    class DebugDrawManager;

    struct RenderSystemInitInfo
    {
        std::shared_ptr<WindowSystem>     window_system;
        std::shared_ptr<DebugDrawManager> debugdraw_manager;
    };

    struct EngineContentViewport
    {
        float x {0.f};
        float y {0.f};
        float width {0.f};
        float height {0.f};
    };

    class RenderSystem
    {
    public:
        RenderSystem() = default;
        ~RenderSystem();

        // �����_�����O�V�X�e���̏�����
        void initialize(RenderSystemInitInfo init_info);
        // �����_�����O�̍X�V
        void tick(float delta_time);
        // �����_�����O�f�[�^�̃N���A
        void clear();

        // �`��f�[�^�̃X���b�v
        void swapLogicRenderData();
        // �X���b�v�R���e�L�X�g�̎擾
        RenderSwapContext& getSwapContext();
        // �����_�����O�J�����̎擾
        std::shared_ptr<RenderCamera> getRenderCamera() const;
        // RHI�̎擾
        std::shared_ptr<RHI> getRHI() const;

        // �����_�����O�p�C�v���C���^�C�v�̐ݒ�
        void setRenderPipelineType(RENDER_PIPELINE_TYPE pipeline_type);
        // UI�����_�����O�o�b�N�G���h�̏�����
        void initializeUIRenderBackend(WindowUI* window_ui);
        // �G���W���R���e���c�r���[�|�[�g�̍X�V
        void updateEngineContentViewport(float offset_x, float offset_y, float width, float height);
        // �I�����ꂽ���b�V����GUID���擾
        uint32_t getGuidOfPickedMesh(const Vector2& picked_uv);
        // ���b�V��ID�ɂ��GObjectID�̎擾
        GObjectID getGObjectIDByMeshID(uint32_t mesh_id) const;

        // �G���W���R���e���c�r���[�|�[�g�̎擾
        EngineContentViewport getEngineContentViewport() const;

        // ���̍쐬
        void createAxis(std::array<RenderEntity, 3> axis_entities, std::array<RenderMeshData, 3> mesh_datas);
        // �����̐ݒ�
        void setVisibleAxis(std::optional<RenderEntity> axis);
        // �I�����ꂽ���̐ݒ�
        void setSelectedAxis(size_t selected_axis);
        // GO�C���X�^���XID�A���P�[�^�̎擾
        GuidAllocator<GameObjectPartId>& getGOInstanceIdAllocator();
        // ���b�V���A�Z�b�gID�A���P�[�^�̎擾
        GuidAllocator<MeshSourceDesc>& getMeshAssetIdAllocator();

        // ���x���̃����[�h�̂��߂̃N���A
        void clearForLevelReloading();

    private:
        // �����_�����O�p�C�v���C���^�C�v
        RENDER_PIPELINE_TYPE m_render_pipeline_type {RENDER_PIPELINE_TYPE::DEFERRED_PIPELINE};

        // �����_�����O�X���b�v�R���e�L�X�g
        RenderSwapContext m_swap_context;

        // RHI�I�u�W�F�N�g
        std::shared_ptr<RHI> m_rhi;
        // �����_�����O�J�����I�u�W�F�N�g
        std::shared_ptr<RenderCamera> m_render_camera;
        // �����_�����O�V�[���I�u�W�F�N�g
        std::shared_ptr<RenderScene> m_render_scene;
        // �����_�����O���\�[�X�I�u�W�F�N�g
        std::shared_ptr<RenderResourceBase> m_render_resource;
        // �����_�����O�p�C�v���C���I�u�W�F�N�g
        std::shared_ptr<RenderPipelineBase> m_render_pipeline;

        // �X���b�v�f�[�^�̏���
        void processSwapData();
    };

} // namespace LunarYue
