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
        void initializeUIRenderBackend(WindowUI* window_ui) override final;
        void draw() override final;

        void* getIconId(const std::string& name);

    private:
        void uploadFonts();
        void uploadIconTexture();

    private:
        WindowUI* m_window_ui;

        struct IconResource
        {
            RHIImage*     icon_texture_image;
            RHIImageView* icon_texture_image_view;
            VmaAllocation icon_texture_image_allocation;
            // RHIDescriptorSet icon_texture_image_descriptorSet;
            std::shared_ptr<RHIDescriptorSet> icon_texture_image_descriptorSet;

            std::shared_ptr<TextureData> icon_texture_data;
        };

        std::map<std::string, IconResource> m_icon_map {};
    };
} // namespace LunarYue
