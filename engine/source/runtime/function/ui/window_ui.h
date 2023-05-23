#pragma once

#include <map>
#include <memory>
#include <string>

namespace LunarYue
{
    class WindowSystem;
    class RenderSystem;
    class ConfigManager;

    struct WindowUIInitInfo
    {
        std::shared_ptr<WindowSystem>  window_system;
        std::shared_ptr<RenderSystem>  render_system;
        std::shared_ptr<ConfigManager> config_manager;
    };

    class WindowUI
    {
    public:
        virtual void initialize(WindowUIInitInfo init_info) = 0;
        virtual void preRender()                            = 0;

        std::map<std::string, std::string> getIconTextureMap() const { return m_iconTextureMap; }

    protected:
        std::map<std::string, std::string> m_iconTextureMap {};
    };
} // namespace LunarYue
