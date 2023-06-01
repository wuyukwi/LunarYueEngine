#pragma once

#include <map>
#include <memory>
#include <string>

namespace LunarYue
{
    class WindowUI
    {
    public:
        virtual void initialize() = 0;
        virtual void preRender()  = 0;

        std::map<std::string, std::string> getIconTextureMap() const { return m_iconTextureMap; }

    protected:
        std::map<std::string, std::string> m_iconTextureMap {};
    };
} // namespace LunarYue
