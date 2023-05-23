#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <array>
#include <functional>
#include <vector>

namespace LunarYue
{
    struct WindowCreateInfo
    {
        int         width {1280};
        int         height {720};
        const char* title {"LunarYue"};
        bool        is_fullscreen {false};
    };

    class WindowSystem
    {
    public:
        WindowSystem() = default;
        ~WindowSystem();
        void               initialize(WindowCreateInfo create_info);
        void               pollEvents() const;
        void               setShouldClose(bool value) const;
        bool               shouldClose() const;
        void               setTitle(const char* title);
        GLFWwindow*        getWindow() const;
        std::array<int, 2> getWindowSize() const;

        // �C�x���g�̃R�[���o�b�N�֐���o�^���邽�߂̌^
        typedef std::function<void()>                   onResetFunc;       // ���Z�b�g�C�x���g�p
        typedef std::function<void(int, int, int, int)> onKeyFunc;         // �L�[�C�x���g�p
        typedef std::function<void(unsigned int)>       onCharFunc;        // �����C�x���g�p
        typedef std::function<void(int, unsigned int)>  onCharModsFunc;    // �������f�B�t�@�C�A�C�x���g�p
        typedef std::function<void(int, int, int)>      onMouseButtonFunc; // �}�E�X�{�^���C�x���g�p
        typedef std::function<void(double, double)>     onCursorPosFunc;   // �J�[�\���ʒu�C�x���g�p
        typedef std::function<void(int)>                onCursorEnterFunc; // �J�[�\���i���C�x���g�p
        typedef std::function<void(double, double)>     onScrollFunc;      // �X�N���[���C�x���g�p
        typedef std::function<void(int, const char**)>  onDropFunc;        // �h���b�v�C�x���g�p
        typedef std::function<void(int, int)>           onWindowSizeFunc;  // �E�B���h�E�T�C�Y�ύX�C�x���g�p
        typedef std::function<void()>                   onWindowCloseFunc; // �E�B���h�E�N���[�Y�C�x���g�p

        // �C�x���g�R�[���o�b�N�֐���o�^���郁�\�b�h
        void registerOnResetFunc(onResetFunc func) { m_onResetFunc.push_back(func); }                   // ���Z�b�g�C�x���g
        void registerOnKeyFunc(onKeyFunc func) { m_onKeyFunc.push_back(func); }                         // �L�[�C�x���g
        void registerOnCharFunc(onCharFunc func) { m_onCharFunc.push_back(func); }                      // �����C�x���g
        void registerOnCharModsFunc(onCharModsFunc func) { m_onCharModsFunc.push_back(func); }          // �������f�B�t�@�C�A�C�x���g
        void registerOnMouseButtonFunc(onMouseButtonFunc func) { m_onMouseButtonFunc.push_back(func); } // �}�E�X�{�^���C�x���g
        void registerOnCursorPosFunc(onCursorPosFunc func) { m_onCursorPosFunc.push_back(func); }       // �J�[�\���ʒu�C�x���g
        void registerOnCursorEnterFunc(onCursorEnterFunc func) { m_onCursorEnterFunc.push_back(func); } // �J�[�\���i���C�x���g
        void registerOnScrollFunc(onScrollFunc func) { m_onScrollFunc.push_back(func); }                // �X�N���[���C�x���g
        void registerOnDropFunc(onDropFunc func) { m_onDropFunc.push_back(func); }                      // �h���b�v�C�x���g
        void registerOnWindowSizeFunc(onWindowSizeFunc func) { m_onWindowSizeFunc.push_back(func); }    // �E�B���h�E�T�C�Y�ύX�C�x���g
        void registerOnWindowCloseFunc(onWindowCloseFunc func) { m_onWindowCloseFunc.push_back(func); } // �E�B���h�E�N���[�Y�C�x���g

        bool isMouseButtonDown(int button) const
        {
            if (button < GLFW_MOUSE_BUTTON_1 || button > GLFW_MOUSE_BUTTON_LAST)
            {
                return false;
            }
            return glfwGetMouseButton(m_window, button) == GLFW_PRESS;
        }
        bool getFocusMode() const { return m_is_focus_mode; }
        void setFocusMode(bool mode);

    protected:
        // window event callbacks
        static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            WindowSystem* app = (WindowSystem*)glfwGetWindowUserPointer(window);
            if (app)
            {
                app->onKey(key, scancode, action, mods);
            }
        }
        static void charCallback(GLFWwindow* window, unsigned int codepoint)
        {
            WindowSystem* app = (WindowSystem*)glfwGetWindowUserPointer(window);
            if (app)
            {
                app->onChar(codepoint);
            }
        }
        static void charModsCallback(GLFWwindow* window, unsigned int codepoint, int mods)
        {
            WindowSystem* app = (WindowSystem*)glfwGetWindowUserPointer(window);
            if (app)
            {
                app->onCharMods(codepoint, mods);
            }
        }
        static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
        {
            WindowSystem* app = (WindowSystem*)glfwGetWindowUserPointer(window);
            if (app)
            {
                app->onMouseButton(button, action, mods);
            }
        }
        static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos)
        {
            WindowSystem* app = (WindowSystem*)glfwGetWindowUserPointer(window);
            if (app)
            {
                app->onCursorPos(xpos, ypos);
            }
        }
        static void cursorEnterCallback(GLFWwindow* window, int entered)
        {
            WindowSystem* app = (WindowSystem*)glfwGetWindowUserPointer(window);
            if (app)
            {
                app->onCursorEnter(entered);
            }
        }
        static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
        {
            WindowSystem* app = (WindowSystem*)glfwGetWindowUserPointer(window);
            if (app)
            {
                app->onScroll(xoffset, yoffset);
            }
        }
        static void dropCallback(GLFWwindow* window, int count, const char** paths)
        {
            WindowSystem* app = (WindowSystem*)glfwGetWindowUserPointer(window);
            if (app)
            {
                app->onDrop(count, paths);
            }
        }
        static void windowSizeCallback(GLFWwindow* window, int width, int height)
        {
            WindowSystem* app = (WindowSystem*)glfwGetWindowUserPointer(window);
            if (app)
            {
                app->m_width  = width;
                app->m_height = height;
            }
        }
        static void windowCloseCallback(GLFWwindow* window) { glfwSetWindowShouldClose(window, true); }

        void onReset()
        {
            for (auto& func : m_onResetFunc)
                func();
        }
        void onKey(int key, int scancode, int action, int mods)
        {
            for (auto& func : m_onKeyFunc)
                func(key, scancode, action, mods);
        }
        void onChar(unsigned int codepoint)
        {
            for (auto& func : m_onCharFunc)
                func(codepoint);
        }
        void onCharMods(int codepoint, unsigned int mods)
        {
            for (auto& func : m_onCharModsFunc)
                func(codepoint, mods);
        }
        void onMouseButton(int button, int action, int mods)
        {
            for (auto& func : m_onMouseButtonFunc)
                func(button, action, mods);
        }
        void onCursorPos(double xpos, double ypos)
        {
            for (auto& func : m_onCursorPosFunc)
                func(xpos, ypos);
        }
        void onCursorEnter(int entered)
        {
            for (auto& func : m_onCursorEnterFunc)
                func(entered);
        }
        void onScroll(double xoffset, double yoffset)
        {
            for (auto& func : m_onScrollFunc)
                func(xoffset, yoffset);
        }
        void onDrop(int count, const char** paths)
        {
            for (auto& func : m_onDropFunc)
                func(count, paths);
        }
        void onWindowSize(int width, int height)
        {
            for (auto& func : m_onWindowSizeFunc)
                func(width, height);
        }

    private:
        GLFWwindow* m_window {nullptr};
        int         m_width {0};
        int         m_height {0};

        bool m_is_focus_mode {false};

        std::vector<onResetFunc>       m_onResetFunc;
        std::vector<onKeyFunc>         m_onKeyFunc;
        std::vector<onCharFunc>        m_onCharFunc;
        std::vector<onCharModsFunc>    m_onCharModsFunc;
        std::vector<onMouseButtonFunc> m_onMouseButtonFunc;
        std::vector<onCursorPosFunc>   m_onCursorPosFunc;
        std::vector<onCursorEnterFunc> m_onCursorEnterFunc;
        std::vector<onScrollFunc>      m_onScrollFunc;
        std::vector<onDropFunc>        m_onDropFunc;
        std::vector<onWindowSizeFunc>  m_onWindowSizeFunc;
        std::vector<onWindowCloseFunc> m_onWindowCloseFunc;
    };
} // namespace LunarYue
