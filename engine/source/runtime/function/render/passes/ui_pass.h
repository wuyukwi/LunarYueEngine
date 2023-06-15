#pragma once

#include "runtime/function/render/render_pass.h"

namespace LunarYue
{
    class WindowUI;

    struct UIPassInitInfo : RenderPassInitInfo
    {
        RHIRenderPass* render_pass;
    };

    class UIPass : public RenderPass
    {
    public:
        void initialize(const RenderPassInitInfo* init_info) override final;
        void initializeUIRenderBackend(std::shared_ptr<WindowUI> window_ui) override final;
        void draw() override final;

        void* getIconId(const std::string& name);

        void changeUI(const std::shared_ptr<WindowUI>& window_ui);

    private:
        void uploadFonts();
        void uploadIconTexture();

    private:
        std::shared_ptr<WindowUI> m_window_ui = nullptr;

        struct IconResource
        {
            RHIImage*                         icon_texture_image {};
            RHIImageView*                     icon_texture_image_view {};
            VmaAllocation                     icon_texture_image_allocation {};
            std::shared_ptr<RHIDescriptorSet> icon_texture_image_descriptorSet;

            std::shared_ptr<TextureData> icon_texture_data;
        };

        std::map<std::string, IconResource> m_icon_map {};
    };
} // namespace LunarYue
