#include "runtime/function/render/passes/ui_pass.h"

#include "runtime/function/render/interface/vulkan/vulkan_rhi.h"

#include "runtime/resource/config_manager/config_manager.h"

#include "runtime/function/ui/window_ui.h"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

namespace LunarYue
{
    void UIPass::initialize(const RenderPassInitInfo* init_info)
    {
        RenderPass::initialize(nullptr);

        m_framebuffer.render_pass = static_cast<const UIPassInitInfo*>(init_info)->render_pass;
    }

    void UIPass::initializeUIRenderBackend(std::shared_ptr<WindowUI> window_ui)
    {
        m_window_ui = window_ui;

        ImGui_ImplGlfw_InitForVulkan(std::static_pointer_cast<VulkanRHI>(m_rhi)->m_window, true);
        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance                  = std::static_pointer_cast<VulkanRHI>(m_rhi)->m_instance;
        init_info.PhysicalDevice            = std::static_pointer_cast<VulkanRHI>(m_rhi)->m_physical_device;
        init_info.Device                    = std::static_pointer_cast<VulkanRHI>(m_rhi)->m_device;
        init_info.QueueFamily               = m_rhi->getQueueFamilyIndices().graphics_family.value();
        init_info.Queue                     = static_cast<VulkanQueue*>(m_rhi->getGraphicsQueue())->getResource();
        init_info.DescriptorPool            = std::static_pointer_cast<VulkanRHI>(m_rhi)->m_vk_descriptor_pool;
        init_info.Subpass                   = _main_camera_subpass_ui;

        // may be different from the real swapchain image count
        // see ImGui_ImplVulkanH_GetMinImageCountFromPresentMode
        init_info.MinImageCount = 3;
        init_info.ImageCount    = 3;
        ImGui_ImplVulkan_Init(&init_info, static_cast<VulkanRenderPass*>(m_framebuffer.render_pass)->getResource());

        uploadFonts();
        uploadIconTexture();
    }

    void UIPass::uploadFonts()
    {
        RHICommandBufferAllocateInfo allocInfo = {};
        allocInfo.sType                        = RHI_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level                        = RHI_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool                  = m_rhi->getCommandPoor();
        allocInfo.commandBufferCount           = 1;

        RHICommandBuffer* commandBuffer = new VulkanCommandBuffer();
        if (RHI_SUCCESS != m_rhi->allocateCommandBuffers(&allocInfo, commandBuffer))
        {
            throw std::runtime_error("failed to allocate command buffers!");
        }

        RHICommandBufferBeginInfo beginInfo = {};
        beginInfo.sType                     = RHI_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags                     = RHI_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        if (RHI_SUCCESS != m_rhi->beginCommandBuffer(commandBuffer, &beginInfo))
        {
            throw std::runtime_error("Could not create one-time command buffer!");
        }

        ImGui_ImplVulkan_CreateFontsTexture(((VulkanCommandBuffer*)commandBuffer)->getResource());

        if (RHI_SUCCESS != m_rhi->endCommandBuffer(commandBuffer))
        {
            throw std::runtime_error("failed to record command buffer!");
        }

        RHISubmitInfo submitInfo {};
        submitInfo.sType              = RHI_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers    = &commandBuffer;

        m_rhi->queueSubmit(m_rhi->getGraphicsQueue(), 1, &submitInfo, RHI_NULL_HANDLE);
        m_rhi->queueWaitIdle(m_rhi->getGraphicsQueue());

        m_rhi->freeCommandBuffers(m_rhi->getCommandPoor(), 1, commandBuffer);

        ImGui_ImplVulkan_DestroyFontUploadObjects();
    }

    void UIPass::uploadIconTexture()
    {
        const auto icon_map = m_window_ui->getIconTextureMap();

        RHISampler*          sampler;
        RHISamplerCreateInfo samplerCreateInfo {};
        samplerCreateInfo.sType            = RHI_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerCreateInfo.maxAnisotropy    = 1.0f;
        samplerCreateInfo.anisotropyEnable = true;
        samplerCreateInfo.magFilter        = RHI_FILTER_LINEAR;
        samplerCreateInfo.minFilter        = RHI_FILTER_LINEAR;
        samplerCreateInfo.mipmapMode       = RHI_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerCreateInfo.addressModeU     = RHI_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerCreateInfo.addressModeV     = RHI_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerCreateInfo.addressModeW     = RHI_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerCreateInfo.mipLodBias       = 0.0f;
        samplerCreateInfo.compareOp        = RHI_COMPARE_OP_NEVER;
        samplerCreateInfo.minLod           = 0.0f;
        samplerCreateInfo.maxLod           = 0.0f;
        samplerCreateInfo.borderColor      = RHI_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

        if (RHI_SUCCESS != m_rhi->createSampler(&samplerCreateInfo, sampler))
        {
            throw std::runtime_error("create sampler error");
        }

        for (const auto& icon : icon_map)
        {
            IconResource icon_resource {};
            icon_resource.icon_texture_data = m_render_resource->loadTexture(icon.second);
            m_rhi->createGlobalImage(icon_resource.icon_texture_image,
                                     icon_resource.icon_texture_image_view,
                                     icon_resource.icon_texture_image_allocation,
                                     icon_resource.icon_texture_data->m_width,
                                     icon_resource.icon_texture_data->m_height,
                                     icon_resource.icon_texture_data->m_pixels,
                                     icon_resource.icon_texture_data->m_format);

            auto vulkanDescriptorSet = std::make_shared<VulkanDescriptorSet>();
            vulkanDescriptorSet->setResource(ImGui_ImplVulkan_AddTexture(((VulkanSampler*)sampler)->getResource(),
                                                                         ((VulkanImageView*)icon_resource.icon_texture_image_view)->getResource(),
                                                                         VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL));
            icon_resource.icon_texture_image_descriptorSet = vulkanDescriptorSet;

            m_icon_map[icon.first] = icon_resource;
        }
    }

    void UIPass::draw()
    {
        if (m_window_ui)
        {
            ImGui_ImplVulkan_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            m_window_ui->preRender();

            float color[4] = {1.0f, 1.0f, 1.0f, 1.0f};
            m_rhi->pushEvent(m_rhi->getCurrentCommandBuffer(), "ImGUI", color);

            ImGui::Render();

            ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), std::static_pointer_cast<VulkanRHI>(m_rhi)->m_vk_current_command_buffer);

            m_rhi->popEvent(m_rhi->getCurrentCommandBuffer());
        }
    }

    void* UIPass::getIconId(const std::string& name)
    {
        const auto it = m_icon_map.find(name);
        if (it != m_icon_map.end())
        {
            auto vulkanDescriptorSet = std::static_pointer_cast<VulkanDescriptorSet>(it->second.icon_texture_image_descriptorSet);
            return vulkanDescriptorSet->getResource();
        }
        return nullptr;
    }

    void UIPass::changeUI(const std::shared_ptr<WindowUI>& window_ui) { m_window_ui = window_ui; }
} // namespace LunarYue
