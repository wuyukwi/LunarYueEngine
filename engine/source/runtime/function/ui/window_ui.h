#pragma once

#include <map>
#include <memory>
#include <string>

namespace LunarYue
{
    class WindowSystem;
    class RenderSystem;

    struct WindowUIInitInfo
    {
        std::shared_ptr<WindowSystem> window_system;
        std::shared_ptr<RenderSystem> render_system;
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
