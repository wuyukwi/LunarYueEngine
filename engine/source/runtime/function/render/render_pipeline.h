#pragma once

#include "runtime/function/render/render_pipeline_base.h"

namespace LunarYue
{
    class RenderPipeline : public RenderPipelineBase
    {
    public:
        virtual void initialize(RenderPipelineInitInfo init_info) override final;

        virtual void forwardRender(std::shared_ptr<RHI> rhi, std::shared_ptr<RenderResourceBase> render_resource) override final;

        virtual void deferredRender(std::shared_ptr<RHI> rhi, std::shared_ptr<RenderResourceBase> render_resource) override final;

        void passUpdateAfterRecreateSwapchain();

        virtual uint32_t getGuidOfPickedMesh(const Vector2& picked_uv) override final;

        virtual void* getIconId(const std::string& name) override;

        void setAxisVisibleState(bool state);

        void setSelectedAxis(size_t selected_axis);
    };
} // namespace LunarYue
