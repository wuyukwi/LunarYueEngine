#include "runtime/function/render/render_system.h"

#include "runtime/core/base/macro.h"

#include "runtime/resource/asset_manager/asset_manager.h"
#include "runtime/resource/config_manager/config_manager.h"

#include "bgfx_utils.h"
#include "entry/entry.h"
#include "imgui/imgui.h"
#include "runtime/function/global/global_context.h"
#include "runtime/function/render/debugdraw/debug_draw_manager.h"
#include "runtime/function/render/render_camera.h"
#include "runtime/function/render/render_scene.h"
#include "runtime/function/render/window_system.h"
#include <bx/uint32_t.h>

namespace LunarYue
{
    RenderSystem::~RenderSystem()
    {
        imguiDestroy();

        bgfx::shutdown();
    }

    void RenderSystem::initialize(RenderSystemInitInfo init_info)
    {
        m_callback = std::make_unique<RendererCallback>();

        const auto windows_system = init_info.window_system;

        auto window_info = init_info.window_system->getWindowInfo();

        bgfx::Init bgfxInit;
        bgfxInit.type     = bgfx::RendererType::Vulkan;
        bgfxInit.vendorId = BGFX_PCI_ID_NONE;

        bgfxInit.resolution.format = bgfx::TextureFormat::RGBA8;

        bgfxInit.resolution.width  = window_info.m_width;
        bgfxInit.resolution.height = window_info.m_height;
        bgfxInit.resolution.reset  = window_info.m_reset;
        bgfxInit.debug             = window_info.m_debug;

        bgfxInit.platformData.nwh = entry::getNativeWindowHandle(entry::kDefaultWindowHandle);
        bgfxInit.platformData.ndt = entry::getNativeDisplayHandle();

        bgfxInit.callback = m_callback.get();

        if (bgfx::init(bgfxInit) == false)
        {
            LOG_ERROR("bgfx init failed")
        }

        bgfx::setDebug(window_info.m_debug);

        bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f, 0);

        imguiCreate();
    }

    void RenderSystem::beginFrame()
    {
        const auto window_state = g_runtime_global_context.m_window_system->getWindowState();

        imguiBeginFrame(window_state.m_mouse.m_mx,
                        window_state.m_mouse.m_my,
                        (window_state.m_mouse.m_buttons[entry::MouseButton::Left] ? IMGUI_MBUT_LEFT : 0) |
                            (window_state.m_mouse.m_buttons[entry::MouseButton::Right] ? IMGUI_MBUT_RIGHT : 0) |
                            (window_state.m_mouse.m_buttons[entry::MouseButton::Middle] ? IMGUI_MBUT_MIDDLE : 0),
                        window_state.m_mouse.m_mz,
                        uint16_t(window_state.m_width),
                        uint16_t(window_state.m_height));

        // Set view 0 default viewport.
        bgfx::setViewRect(0, 0, 0, uint16_t(window_state.m_width), uint16_t(window_state.m_height));
    }

    void RenderSystem::endFrame()
    {
        imguiEndFrame();

        bgfx::frame();
    }

    void RenderSystem::tick(float delta_time)
    {
        // This dummy draw call is here to make sure that view 0 is cleared
        // if no other draw calls are submitted to view 0.
        bgfx::touch(0);

        // Use debug font to print information about this example.
        bgfx::dbgTextClear();
        // bgfx::dbgTextImage(
        //     bx::max<uint16_t>(uint16_t(m_width / 2 / 8), 20) - 20, bx::max<uint16_t>(uint16_t(m_height / 2 / 16), 6) - 6, 40, 12, s_logo,
        // 160);
        bgfx::dbgTextPrintf(
            0, 1, 0x0f, "Color can be changed with ANSI \x1b[9;me\x1b[10;ms\x1b[11;mc\x1b[12;ma\x1b[13;mp\x1b[14;me\x1b[0m code too.");

        bgfx::dbgTextPrintf(
            80, 1, 0x0f, "\x1b[;0m    \x1b[;1m    \x1b[; 2m    \x1b[; 3m    \x1b[; 4m    \x1b[; 5m    \x1b[; 6m    \x1b[; 7m    \x1b[0m");
        bgfx::dbgTextPrintf(
            80, 2, 0x0f, "\x1b[;8m    \x1b[;9m    \x1b[;10m    \x1b[;11m    \x1b[;12m    \x1b[;13m    \x1b[;14m    \x1b[;15m    \x1b[0m");

        const bgfx::Stats* stats = bgfx::getStats();
        bgfx::dbgTextPrintf(0,
                            2,
                            0x0f,
                            "Backbuffer %dW x %dH in pixels, debug text %dW x %dH in characters.",
                            stats->width,
                            stats->height,
                            stats->textWidth,
                            stats->textHeight);
    }

    void RenderSystem::clear() {}

    void RenderSystem::swapLogicRenderData() {}

    std::shared_ptr<RenderCamera> RenderSystem::getRenderCamera() const { return m_render_camera; }

    std::shared_ptr<RHI> RenderSystem::getRHI() const { return m_rhi; }

    void RenderSystem::updateEngineContentViewport(float offset_x, float offset_y, float width, float height)
    {

        m_render_camera->setAspect(width / height);
    }

    uint32_t RenderSystem::getGuidOfPickedMesh(const Vector2& picked_uv) { return k_invalid_object_id; }

    ObjectID RenderSystem::getGObjectIDByMeshID(uint32_t mesh_id) const { return m_render_scene->getGObjectIDByMeshID(mesh_id); }

    void RenderSystem::createAxis(std::array<RenderEntity, 3> axis_entities, std::array<RenderMeshData, 3> mesh_datas) {}

    void RenderSystem::setVisibleAxis(std::optional<RenderEntity> axis) { m_render_scene->m_render_axis = axis; }

    void RenderSystem::setSelectedAxis(size_t selected_axis) {}

    GuidAllocator<GameObjectPartId>& RenderSystem::getGOInstanceIdAllocator() { return m_render_scene->getInstanceIdAllocator(); }

    GuidAllocator<MeshSourceDesc>& RenderSystem::getMeshAssetIdAllocator() { return m_render_scene->getMeshAssetIdAllocator(); }

    void RenderSystem::clearForLevelReloading() { m_render_scene->clearForLevelReloading(); }

    void RenderSystem::setRenderPipelineType(RENDER_PIPELINE_TYPE pipeline_type) { m_render_pipeline_type = pipeline_type; }

    void RenderSystem::initializeUIRenderBackend(std::shared_ptr<WindowUI> window_ui) {}

    void RenderSystem::processSwapData() {}
} // namespace LunarYue
