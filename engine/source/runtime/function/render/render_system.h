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

        // レンダリングシステムの初期化
        void initialize(RenderSystemInitInfo init_info);
        // レンダリングの更新
        void tick(float delta_time);
        // レンダリングデータのクリア
        void clear();

        // 描画データのスワップ
        void swapLogicRenderData();
        // スワップコンテキストの取得
        RenderSwapContext& getSwapContext();
        // レンダリングカメラの取得
        std::shared_ptr<RenderCamera> getRenderCamera() const;
        // RHIの取得
        std::shared_ptr<RHI> getRHI() const;

        // レンダリングパイプラインタイプの設定
        void setRenderPipelineType(RENDER_PIPELINE_TYPE pipeline_type);
        // UIレンダリングバックエンドの初期化
        void initializeUIRenderBackend(WindowUI* window_ui);
        // エンジンコンテンツビューポートの更新
        void updateEngineContentViewport(float offset_x, float offset_y, float width, float height);
        // 選択されたメッシュのGUIDを取得
        uint32_t getGuidOfPickedMesh(const Vector2& picked_uv);
        // メッシュIDによるGObjectIDの取得
        GObjectID getGObjectIDByMeshID(uint32_t mesh_id) const;

        // エンジンコンテンツビューポートの取得
        EngineContentViewport getEngineContentViewport() const;

        // 軸の作成
        void createAxis(std::array<RenderEntity, 3> axis_entities, std::array<RenderMeshData, 3> mesh_datas);
        // 可視軸の設定
        void setVisibleAxis(std::optional<RenderEntity> axis);
        // 選択された軸の設定
        void setSelectedAxis(size_t selected_axis);
        // GOインスタンスIDアロケータの取得
        GuidAllocator<GameObjectPartId>& getGOInstanceIdAllocator();
        // メッシュアセットIDアロケータの取得
        GuidAllocator<MeshSourceDesc>& getMeshAssetIdAllocator();

        // レベルのリロードのためのクリア
        void clearForLevelReloading();

    private:
        // レンダリングパイプラインタイプ
        RENDER_PIPELINE_TYPE m_render_pipeline_type {RENDER_PIPELINE_TYPE::DEFERRED_PIPELINE};

        // レンダリングスワップコンテキスト
        RenderSwapContext m_swap_context;

        // RHIオブジェクト
        std::shared_ptr<RHI> m_rhi;
        // レンダリングカメラオブジェクト
        std::shared_ptr<RenderCamera> m_render_camera;
        // レンダリングシーンオブジェクト
        std::shared_ptr<RenderScene> m_render_scene;
        // レンダリングリソースオブジェクト
        std::shared_ptr<RenderResourceBase> m_render_resource;
        // レンダリングパイプラインオブジェクト
        std::shared_ptr<RenderPipelineBase> m_render_pipeline;

        // スワップデータの処理
        void processSwapData();
    };

} // namespace LunarYue
