#include "gui_system.h"

#include <core/filesystem/filesystem.h>
#include <core/graphics/index_buffer.h>
#include <core/graphics/render_pass.h>
#include <core/graphics/shader.h>
#include <core/graphics/texture.h>
#include <core/graphics/uniform.h>
#include <core/graphics/vertex_buffer.h>
#include <core/logging/logging.h>
#include <core/serialization/associative_archive.h>
#include <core/system/subsystem.h>

#include <runtime/assets/asset_manager.h>
#include <runtime/input/input.h>
#include <runtime/rendering/gpu_program.h>
#include <runtime/rendering/render_window.h>
#include <runtime/rendering/renderer.h>

#include <editor_core/gui/embedded/editor_default.ttf.h>
#include <editor_core/gui/embedded/fontawesome_webfont.ttf.h>
#include <editor_core/gui/embedded/fs_ocornut_imgui.bin.h>
#include <editor_core/gui/embedded/vs_ocornut_imgui.bin.h>
#include <editor_core/gui/gui.h>

//////////////////////////////////////////////////////////////////////////
#include "../meta/interface/gui_system.hpp"

#include <fstream>
#include <unordered_map>
namespace
{
    const gfx::embedded_shader s_embedded_shaders[] = {BGFX_EMBEDDED_SHADER(vs_ocornut_imgui),
                                                       BGFX_EMBEDDED_SHADER(fs_ocornut_imgui),
                                                       BGFX_EMBEDDED_SHADER_END()};
    // -------------------------------------------------------------------
    std::unique_ptr<gpu_program>                   s_program;
    asset_handle<gfx::texture>                     s_font_texture;
    std::uint32_t                                  s_draw_calls = 0;
    static std::map<SDL_SystemCursor, SDL_Cursor*> cursors;

    void render_func(ImDrawData* _draw_data)
    {
        if (_draw_data == nullptr || s_program == nullptr)
        {
            return;
        }
        // Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
        int fb_width  = (int)(_draw_data->DisplaySize.x * _draw_data->FramebufferScale.x);
        int fb_height = (int)(_draw_data->DisplaySize.y * _draw_data->FramebufferScale.y);
        if (fb_width <= 0 || fb_height <= 0)
            return;

        _draw_data->ScaleClipRects(gui::GetIO().DisplayFramebufferScale);

        auto              viewId = gfx::render_pass::get_pass();
        const bgfx::Caps* caps   = bgfx::getCaps();
        {
            float ortho[16];
            float x      = _draw_data->DisplayPos.x;
            float y      = _draw_data->DisplayPos.y;
            float width  = _draw_data->DisplaySize.x;
            float height = _draw_data->DisplaySize.y;

            bx::mtxOrtho(ortho, x, x + width, y + height, y, 0.0f, 1000.0f, 0.0f, caps->homogeneousDepth);
            bgfx::setViewTransform(viewId, NULL, ortho);
            bgfx::setViewRect(viewId, 0, 0, uint16_t(width), uint16_t(height));
        }

        s_draw_calls = 0;
        auto program = s_program.get();
        program->begin();
        // Render command lists
        for (int32_t ii = 0, num = _draw_data->CmdListsCount; ii < num; ++ii)
        {
            gfx::transient_vertex_buffer tvb;
            gfx::transient_index_buffer  tib;

            const ImDrawList* draw_list    = _draw_data->CmdLists[ii];
            std::uint32_t     num_vertices = static_cast<std::uint32_t>(draw_list->VtxBuffer.size());
            std::uint32_t     num_indices  = static_cast<std::uint32_t>(draw_list->IdxBuffer.size());

            const auto& layout = gfx::pos_texcoord0_color0_vertex::get_layout();

            if (!(gfx::get_avail_transient_vertex_buffer(num_vertices, layout) == num_vertices) ||
                !(gfx::get_avail_transient_index_buffer(num_indices) == num_indices))
            {
                // not enough space in transient buffer just quit drawing the rest...
                break;
            }

            gfx::alloc_transient_vertex_buffer(&tvb, num_vertices, layout);
            gfx::alloc_transient_index_buffer(&tib, num_indices);

            ImDrawVert* verts = reinterpret_cast<ImDrawVert*>(tvb.data);
            std::memcpy(verts, draw_list->VtxBuffer.begin(), num_vertices * sizeof(ImDrawVert));

            ImDrawIdx* indices = reinterpret_cast<ImDrawIdx*>(tib.data);
            std::memcpy(indices, draw_list->IdxBuffer.begin(), num_indices * sizeof(ImDrawIdx));

            std::uint64_t state = 0 | BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_MSAA |
                                  BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA);

            std::uint32_t offset = 0;
            for (const ImDrawCmd *cmd = draw_list->CmdBuffer.begin(), *cmdEnd = draw_list->CmdBuffer.end(); cmd != cmdEnd; ++cmd)
            {
                if (cmd->UserCallback != nullptr)
                {
                    cmd->UserCallback(draw_list, cmd);
                }
                else if (0 != cmd->ElemCount)
                {
                    auto tex = s_font_texture.get();

                    if (nullptr != cmd->TextureId)
                    {
                        tex = reinterpret_cast<gfx::texture*>(cmd->TextureId);
                    }

                    const std::uint16_t x      = std::uint16_t(std::max(cmd->ClipRect.x, 0.0f));
                    const std::uint16_t y      = std::uint16_t(std::max(cmd->ClipRect.y, 0.0f));
                    const std::uint16_t width  = std::uint16_t(std::min(cmd->ClipRect.z, 65535.0f) - x);
                    const std::uint16_t height = std::uint16_t(std::min(cmd->ClipRect.w, 65535.0f) - y);

                    gfx::set_state(state);
                    gfx::set_vertex_buffer(0, &tvb, 0, num_vertices);
                    gfx::set_index_buffer(&tib, offset, cmd->ElemCount);

                    gfx::set_scissor(x, y, width, height);

                    program->set_texture(0, "s_tex", tex);

                    gfx::submit(viewId, program->native_handle());

                    gfx::set_scissor(0, 0, 0, 0);
                    s_draw_calls++;
                }

                offset += cmd->ElemCount;
            }
        }
        program->end();
    }

    static ImGuiKey ImGui_ImplSDL2_KeycodeToImGuiKey(int keycode)
    {
        switch (keycode)
        {
            case SDLK_TAB:
                return ImGuiKey_Tab;
            case SDLK_LEFT:
                return ImGuiKey_LeftArrow;
            case SDLK_RIGHT:
                return ImGuiKey_RightArrow;
            case SDLK_UP:
                return ImGuiKey_UpArrow;
            case SDLK_DOWN:
                return ImGuiKey_DownArrow;
            case SDLK_PAGEUP:
                return ImGuiKey_PageUp;
            case SDLK_PAGEDOWN:
                return ImGuiKey_PageDown;
            case SDLK_HOME:
                return ImGuiKey_Home;
            case SDLK_END:
                return ImGuiKey_End;
            case SDLK_INSERT:
                return ImGuiKey_Insert;
            case SDLK_DELETE:
                return ImGuiKey_Delete;
            case SDLK_BACKSPACE:
                return ImGuiKey_Backspace;
            case SDLK_SPACE:
                return ImGuiKey_Space;
            case SDLK_RETURN:
                return ImGuiKey_Enter;
            case SDLK_ESCAPE:
                return ImGuiKey_Escape;
            case SDLK_QUOTE:
                return ImGuiKey_Apostrophe;
            case SDLK_COMMA:
                return ImGuiKey_Comma;
            case SDLK_MINUS:
                return ImGuiKey_Minus;
            case SDLK_PERIOD:
                return ImGuiKey_Period;
            case SDLK_SLASH:
                return ImGuiKey_Slash;
            case SDLK_SEMICOLON:
                return ImGuiKey_Semicolon;
            case SDLK_EQUALS:
                return ImGuiKey_Equal;
            case SDLK_LEFTBRACKET:
                return ImGuiKey_LeftBracket;
            case SDLK_BACKSLASH:
                return ImGuiKey_Backslash;
            case SDLK_RIGHTBRACKET:
                return ImGuiKey_RightBracket;
            case SDLK_BACKQUOTE:
                return ImGuiKey_GraveAccent;
            case SDLK_CAPSLOCK:
                return ImGuiKey_CapsLock;
            case SDLK_SCROLLLOCK:
                return ImGuiKey_ScrollLock;
            case SDLK_NUMLOCKCLEAR:
                return ImGuiKey_NumLock;
            case SDLK_PRINTSCREEN:
                return ImGuiKey_PrintScreen;
            case SDLK_PAUSE:
                return ImGuiKey_Pause;
            case SDLK_KP_0:
                return ImGuiKey_Keypad0;
            case SDLK_KP_1:
                return ImGuiKey_Keypad1;
            case SDLK_KP_2:
                return ImGuiKey_Keypad2;
            case SDLK_KP_3:
                return ImGuiKey_Keypad3;
            case SDLK_KP_4:
                return ImGuiKey_Keypad4;
            case SDLK_KP_5:
                return ImGuiKey_Keypad5;
            case SDLK_KP_6:
                return ImGuiKey_Keypad6;
            case SDLK_KP_7:
                return ImGuiKey_Keypad7;
            case SDLK_KP_8:
                return ImGuiKey_Keypad8;
            case SDLK_KP_9:
                return ImGuiKey_Keypad9;
            case SDLK_KP_PERIOD:
                return ImGuiKey_KeypadDecimal;
            case SDLK_KP_DIVIDE:
                return ImGuiKey_KeypadDivide;
            case SDLK_KP_MULTIPLY:
                return ImGuiKey_KeypadMultiply;
            case SDLK_KP_MINUS:
                return ImGuiKey_KeypadSubtract;
            case SDLK_KP_PLUS:
                return ImGuiKey_KeypadAdd;
            case SDLK_KP_ENTER:
                return ImGuiKey_KeypadEnter;
            case SDLK_KP_EQUALS:
                return ImGuiKey_KeypadEqual;
            case SDLK_LCTRL:
                return ImGuiKey_LeftCtrl;
            case SDLK_LSHIFT:
                return ImGuiKey_LeftShift;
            case SDLK_LALT:
                return ImGuiKey_LeftAlt;
            case SDLK_LGUI:
                return ImGuiKey_LeftSuper;
            case SDLK_RCTRL:
                return ImGuiKey_RightCtrl;
            case SDLK_RSHIFT:
                return ImGuiKey_RightShift;
            case SDLK_RALT:
                return ImGuiKey_RightAlt;
            case SDLK_RGUI:
                return ImGuiKey_RightSuper;
            case SDLK_APPLICATION:
                return ImGuiKey_Menu;
            case SDLK_0:
                return ImGuiKey_0;
            case SDLK_1:
                return ImGuiKey_1;
            case SDLK_2:
                return ImGuiKey_2;
            case SDLK_3:
                return ImGuiKey_3;
            case SDLK_4:
                return ImGuiKey_4;
            case SDLK_5:
                return ImGuiKey_5;
            case SDLK_6:
                return ImGuiKey_6;
            case SDLK_7:
                return ImGuiKey_7;
            case SDLK_8:
                return ImGuiKey_8;
            case SDLK_9:
                return ImGuiKey_9;
            case SDLK_a:
                return ImGuiKey_A;
            case SDLK_b:
                return ImGuiKey_B;
            case SDLK_c:
                return ImGuiKey_C;
            case SDLK_d:
                return ImGuiKey_D;
            case SDLK_e:
                return ImGuiKey_E;
            case SDLK_f:
                return ImGuiKey_F;
            case SDLK_g:
                return ImGuiKey_G;
            case SDLK_h:
                return ImGuiKey_H;
            case SDLK_i:
                return ImGuiKey_I;
            case SDLK_j:
                return ImGuiKey_J;
            case SDLK_k:
                return ImGuiKey_K;
            case SDLK_l:
                return ImGuiKey_L;
            case SDLK_m:
                return ImGuiKey_M;
            case SDLK_n:
                return ImGuiKey_N;
            case SDLK_o:
                return ImGuiKey_O;
            case SDLK_p:
                return ImGuiKey_P;
            case SDLK_q:
                return ImGuiKey_Q;
            case SDLK_r:
                return ImGuiKey_R;
            case SDLK_s:
                return ImGuiKey_S;
            case SDLK_t:
                return ImGuiKey_T;
            case SDLK_u:
                return ImGuiKey_U;
            case SDLK_v:
                return ImGuiKey_V;
            case SDLK_w:
                return ImGuiKey_W;
            case SDLK_x:
                return ImGuiKey_X;
            case SDLK_y:
                return ImGuiKey_Y;
            case SDLK_z:
                return ImGuiKey_Z;
            case SDLK_F1:
                return ImGuiKey_F1;
            case SDLK_F2:
                return ImGuiKey_F2;
            case SDLK_F3:
                return ImGuiKey_F3;
            case SDLK_F4:
                return ImGuiKey_F4;
            case SDLK_F5:
                return ImGuiKey_F5;
            case SDLK_F6:
                return ImGuiKey_F6;
            case SDLK_F7:
                return ImGuiKey_F7;
            case SDLK_F8:
                return ImGuiKey_F8;
            case SDLK_F9:
                return ImGuiKey_F9;
            case SDLK_F10:
                return ImGuiKey_F10;
            case SDLK_F11:
                return ImGuiKey_F11;
            case SDLK_F12:
                return ImGuiKey_F12;
        }
        return ImGuiKey_None;
    }

    static void ImGui_ImplSDL2_UpdateKeyModifiers(SDL_Keymod sdl_key_mods)
    {
        ImGuiIO& io = ImGui::GetIO();
        io.AddKeyEvent(ImGuiMod_Ctrl, (sdl_key_mods & KMOD_CTRL) != 0);
        io.AddKeyEvent(ImGuiMod_Shift, (sdl_key_mods & KMOD_SHIFT) != 0);
        io.AddKeyEvent(ImGuiMod_Alt, (sdl_key_mods & KMOD_ALT) != 0);
        io.AddKeyEvent(ImGuiMod_Super, (sdl_key_mods & KMOD_GUI) != 0);
    }

    void imgui_handle_event(const SDL_Event& event)
    {
        ImGuiIO& io = ImGui::GetIO();

        switch (event.type)
        {
            case SDL_MOUSEMOTION: {
                ImVec2 mouse_pos((float)event.motion.x, (float)event.motion.y);
                if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
                {
                    int window_x, window_y;
                    SDL_GetWindowPosition(SDL_GetWindowFromID(event.motion.windowID), &window_x, &window_y);
                    mouse_pos.x += window_x;
                    mouse_pos.y += window_y;
                }
                io.AddMouseSourceEvent(event.motion.which == SDL_TOUCH_MOUSEID ? ImGuiMouseSource_TouchScreen : ImGuiMouseSource_Mouse);
                io.AddMousePosEvent(mouse_pos.x, mouse_pos.y);
                break;
            }

            case SDL_MOUSEWHEEL: {
                // IMGUI_DEBUG_LOG("wheel %.2f %.2f, precise %.2f %.2f\n", (float)event->wheel.x, (float)event->wheel.y, event->wheel.preciseX,
                // event->wheel.preciseY);
                float wheel_x = -event.wheel.preciseX;
                float wheel_y = event.wheel.preciseY;
                io.AddMouseSourceEvent(event.wheel.which == SDL_TOUCH_MOUSEID ? ImGuiMouseSource_TouchScreen : ImGuiMouseSource_Mouse);
                io.AddMouseWheelEvent(wheel_x, wheel_y);
                break;
            }

            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP: {
                int mouse_button = -1;
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    mouse_button = 0;
                }
                if (event.button.button == SDL_BUTTON_RIGHT)
                {
                    mouse_button = 1;
                }
                if (event.button.button == SDL_BUTTON_MIDDLE)
                {
                    mouse_button = 2;
                }
                if (event.button.button == SDL_BUTTON_X1)
                {
                    mouse_button = 3;
                }
                if (event.button.button == SDL_BUTTON_X2)
                {
                    mouse_button = 4;
                }
                if (mouse_button == -1)
                    break;
                io.AddMouseSourceEvent(event.button.which == SDL_TOUCH_MOUSEID ? ImGuiMouseSource_TouchScreen : ImGuiMouseSource_Mouse);
                io.AddMouseButtonEvent(mouse_button, (event.type == SDL_MOUSEBUTTONDOWN));
                break;
            }

            case SDL_TEXTINPUT: {
                io.AddInputCharactersUTF8(event.text.text);
                break;
            }

            case SDL_KEYDOWN:
            case SDL_KEYUP: {
                ImGui_ImplSDL2_UpdateKeyModifiers((SDL_Keymod)event.key.keysym.mod);
                ImGuiKey key = ImGui_ImplSDL2_KeycodeToImGuiKey(event.key.keysym.sym);
                io.AddKeyEvent(key, (event.type == SDL_KEYDOWN));
                io.SetKeyEventNativeData(
                    key,
                    event.key.keysym.sym,
                    event.key.keysym.scancode,
                    event.key.keysym
                        .scancode); // To support legacy indexing (<1.87 user code). Legacy backend uses SDLK_*** as indices to IsKeyXXX() functions.
                break;
            }

            case SDL_WINDOWEVENT: {
                // - When capturing mouse, SDL will send a bunch of conflicting LEAVE/ENTER event on every mouse move, but the final ENTER tends to be
                // right.
                // - However we won't get a correct LEAVE event for a captured window.
                // - In some cases, when detaching a window from main viewport SDL may send SDL_WINDOWEVENT_ENTER one frame too late,
                //   causing SDL_WINDOWEVENT_LEAVE on previous frame to interrupt drag operation by clear mouse position. This is why
                //   we delay process the SDL_WINDOWEVENT_LEAVE events by one frame. See issue #5012 for details.
                Uint8 window_event = event.window.event;
                if (window_event == SDL_WINDOWEVENT_ENTER)
                {
                }
                if (window_event == SDL_WINDOWEVENT_LEAVE)
                    if (window_event == SDL_WINDOWEVENT_FOCUS_GAINED)
                        io.AddFocusEvent(true);
                    else if (window_event == SDL_WINDOWEVENT_FOCUS_LOST)
                        io.AddFocusEvent(false);
                if (window_event == SDL_WINDOWEVENT_CLOSE || window_event == SDL_WINDOWEVENT_MOVED || window_event == SDL_WINDOWEVENT_RESIZED)
                    if (ImGuiViewport* viewport = ImGui::FindViewportByPlatformHandle((void*)SDL_GetWindowFromID(event.window.windowID)))
                    {
                        if (window_event == SDL_WINDOWEVENT_CLOSE)
                            viewport->PlatformRequestClose = true;
                        if (window_event == SDL_WINDOWEVENT_MOVED)
                            viewport->PlatformRequestMove = true;
                        if (window_event == SDL_WINDOWEVENT_RESIZED)
                            viewport->PlatformRequestResize = true;
                    }
                break;
            }
        }
    }

    SDL_Cursor* map_cursor(ImGuiMouseCursor cursor)
    {
        static std::map<ImGuiMouseCursor, SDL_SystemCursor> cursor_map = {
            {ImGuiMouseCursor_None, SDL_SYSTEM_CURSOR_NO},
            {ImGuiMouseCursor_Arrow, SDL_SYSTEM_CURSOR_ARROW},
            {ImGuiMouseCursor_ResizeAll, SDL_SYSTEM_CURSOR_SIZEALL},
            {ImGuiMouseCursor_TextInput, SDL_SYSTEM_CURSOR_IBEAM},
            {ImGuiMouseCursor_ResizeNS, SDL_SYSTEM_CURSOR_SIZENS},
            {ImGuiMouseCursor_ResizeEW, SDL_SYSTEM_CURSOR_SIZEWE},
            {ImGuiMouseCursor_ResizeNESW, SDL_SYSTEM_CURSOR_SIZENESW},
            {ImGuiMouseCursor_ResizeNWSE, SDL_SYSTEM_CURSOR_SIZENWSE},
            {ImGuiMouseCursor_Hand, SDL_SYSTEM_CURSOR_HAND},
            {ImGuiMouseCursor_NotAllowed, SDL_SYSTEM_CURSOR_NO},
            //     {ImGuiMouseCursor_Help, SDL_SYSTEM_CURSOR_HELP},
            {ImGuiMouseCursor_Wait, SDL_SYSTEM_CURSOR_WAIT},
            {ImGuiMouseCursor_ArrowWait, SDL_SYSTEM_CURSOR_WAITARROW},
            //        {ImGuiMouseCursor_Cross, SDL_SYSTEM_CURSOR_CROSShair},
        };

        auto id = cursor_map[cursor];

        if (cursors.find(id) == cursors.end())
        {
            SDL_Cursor* sdl_cursor = SDL_CreateSystemCursor(id);
            if (sdl_cursor != nullptr)
            {
                cursors.emplace(id, sdl_cursor);
            }
        }

        return cursors[id];
    }

    void cleanup_cursors()
    {
        for (auto& cursor_pair : cursors)
        {
            SDL_FreeCursor(cursor_pair.second);
        }
        cursors.clear();
    }

    void imgui_frame_begin() { gui::CleanupTextures(); }

    void imgui_set_context(ImGuiContext* context)
    {
        ImGuiContext* last_context = gui::GetCurrentContext();
        if (last_context != nullptr && last_context != context)
        {
            std::memcpy(&context->Style, &last_context->Style, sizeof(ImGuiStyle));
            std::memcpy(&context->IO.KeyMap, &last_context->IO.KeyMap, sizeof(last_context->IO.KeyMap));
            std::memcpy(&context->IO.NavInputs, &last_context->IO.NavInputs, sizeof(last_context->IO.NavInputs));

            context->IO.IniFilename          = last_context->IO.IniFilename;
            context->IO.ConfigFlags          = last_context->IO.ConfigFlags;
            context->IO.BackendFlags         = last_context->IO.BackendFlags;
            context->IO.FontAllowUserScaling = last_context->IO.FontAllowUserScaling;
            context->Initialized             = last_context->Initialized;
        }
        gui::SetCurrentContext(context);
    }

    void imgui_frame_update(render_window& window, delta_t dt)
    {
        auto& io          = gui::GetIO();
        auto  view_size   = window.get_surface()->get_size();
        auto  display_w   = view_size.width;
        auto  display_h   = view_size.height;
        auto  window_size = window.get_size();
        auto  w           = window_size[0];
        auto  h           = window_size[1];

        io.DisplayFramebufferScale = ImVec2(w > 0 ? ((float)display_w / w) : 0, h > 0 ? ((float)display_h / h) : 0);
        // Setup display size (every frame to accommodate for window resizing)
        io.DisplaySize = ImVec2(float(w), float(h));
        // Setup time step
        io.DeltaTime = dt.count();

        irect32_t relative_rect;
        relative_rect.left   = 0;
        relative_rect.top    = 0;
        relative_rect.right  = static_cast<std::int32_t>(w);
        relative_rect.bottom = static_cast<std::int32_t>(h);
        auto mouse_pos       = window.get_mouse_position_in_window();

        if (window.has_focus() && relative_rect.contains({mouse_pos[0], mouse_pos[1]}))
        {
            static auto last_cursor_type = gui::GetMouseCursor();
            auto        cursor           = map_cursor(gui::GetMouseCursor());
            if ((cursor != nullptr) && last_cursor_type != gui::GetMouseCursor())
            {
                window.set_mouse_cursor(cursor);
            }

            last_cursor_type = gui::GetMouseCursor();
        }

        // Start the frame
        gui::NewFrame();

        // gui::SetNextWindowPos(ImVec2(0, 0));
        // gui::SetNextWindowSize(io.DisplaySize);
        //  ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
        //                          ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing;

        // gui::Begin("###workspace", nullptr);
    }

    void imgui_frame_end()
    {
        // gui::End();
        //  auto old = ImGui::GetCurrentContext()->DragDropActive;
        ImGui::GetCurrentContext()->DragDropSourceFrameCount = ImGui::GetCurrentContext()->FrameCount;
        gui::Render();
        // ImGui::GetCurrentContext()->DragDropActive = old;
        render_func(gui::GetDrawData());

        // Update and Render additional Platform Windows
        if (gui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            gui::UpdatePlatformWindows();
            gui::RenderPlatformWindowsDefault();
        }
    }

    ImGuiContext* imgui_create_context(ImFontAtlas& atlas) { return gui::CreateContext(&atlas); }

    void imgui_destroy_context(ImGuiContext* context = nullptr) { gui::DestroyContext(context); }

    void imgui_init_platform_interface()
    {
        ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();

        platform_io.Platform_CreateWindow = [](ImGuiViewport* vp) {
            auto&    renderer  = core::get_subsystem<runtime::renderer>();
            uint32_t sdl_flags = 0;
            sdl_flags |= SDL_WINDOW_ALLOW_HIGHDPI;
            sdl_flags |= SDL_WINDOW_HIDDEN;
            sdl_flags |= (vp->Flags & ImGuiViewportFlags_NoDecoration) ? SDL_WINDOW_BORDERLESS : 0;
            sdl_flags |= (vp->Flags & ImGuiViewportFlags_NoDecoration) ? 0 : SDL_WINDOW_RESIZABLE;
#if !defined(_WIN32)
            // See SDL hack in ImGui_ImplSDL2_ShowWindow().
            sdl_flags |= (viewport->Flags & ImGuiViewportFlags_NoTaskBarIcon) ? SDL_WINDOW_SKIP_TASKBAR : 0;
#endif
            auto window = std::make_unique<render_window>(
                "not title", (int32_t)vp->Size.x, (int32_t)vp->Size.y, (int32_t)vp->Pos.x, (int32_t)vp->Pos.y, vp->ID);
            renderer.register_window(std::move(window));
        };

        platform_io.Platform_DestroyWindow = [](ImGuiViewport* vp) {
            auto& renderer = core::get_subsystem<runtime::renderer>();
            renderer.remove_window_by_id(vp->ID);
        };

        platform_io.Platform_ShowWindow = [](ImGuiViewport* vp) {
            auto& renderer = core::get_subsystem<runtime::renderer>();
            renderer.get_window(vp->ID)->set_visible(true);
        };

        platform_io.Platform_SetWindowPos = [](ImGuiViewport* vp, ImVec2 pos) {
            auto& renderer = core::get_subsystem<runtime::renderer>();
            renderer.get_window(vp->ID)->set_position({int32_t(pos.x), int32_t(pos.y)});
        };

        platform_io.Platform_GetWindowPos = [](ImGuiViewport* vp) -> ImVec2 {
            auto& renderer = core::get_subsystem<runtime::renderer>();
            auto& pos      = renderer.get_window(vp->ID)->get_position();
            return {float(pos[0]), float(pos[1])};
        };

        platform_io.Platform_SetWindowSize = [](ImGuiViewport* vp, ImVec2 pos) {
            auto& renderer = core::get_subsystem<runtime::renderer>();
            renderer.get_window(vp->ID)->set_size({uint32_t(pos.x), uint32_t(pos.y)});
        };

        platform_io.Platform_GetWindowSize = [](ImGuiViewport* vp) -> ImVec2 {
            auto& renderer = core::get_subsystem<runtime::renderer>();
            auto& size     = renderer.get_window(vp->ID)->get_position();
            return {float(size[0]), float(size[1])};
        };

        platform_io.Platform_SetWindowFocus = [](ImGuiViewport* vp) {
            auto& renderer = core::get_subsystem<runtime::renderer>();
            renderer.get_window(vp->ID)->raise_window();
        };

        platform_io.Platform_GetWindowFocus = [](ImGuiViewport* vp) -> bool {
            auto& renderer = core::get_subsystem<runtime::renderer>();
            return renderer.get_window(vp->ID)->has_focus();
        };

        platform_io.Platform_GetWindowMinimized = [](ImGuiViewport* vp) -> bool {
            auto& renderer = core::get_subsystem<runtime::renderer>();
            return renderer.get_window(vp->ID)->get_window_minimized();
        };

        platform_io.Platform_SetWindowTitle = [](ImGuiViewport* vp, const char* title) {
            auto& renderer = core::get_subsystem<runtime::renderer>();
            renderer.get_window(vp->ID)->set_title(title);
        };

        platform_io.Platform_SetWindowAlpha = [](ImGuiViewport* vp, float alpha) {
            auto& renderer = core::get_subsystem<runtime::renderer>();
            renderer.get_window(vp->ID)->set_opacity(alpha);
        };

        // Register main window handle (which is owned by the main application, not by us)
        // This is mostly for simplicity and consistency, so that our code (e.g. mouse handling etc.) can use same logic for main and secondary
        // viewports.
        ImGuiViewport* main_viewport = ImGui::GetMainViewport();

        auto id = main_viewport->ID;
    }

    void imgui_init_clipboard(ImGuiIO& io)
    {
        static char* text_data;

        io.SetClipboardTextFn = [](void*, const char* text) { SDL_SetClipboardText(text); };
        io.GetClipboardTextFn = [](void*) {
            if (text_data)
                SDL_free(text_data);
            text_data = SDL_GetClipboardText();
            return const_cast<const char*>(text_data);
        };
        io.ClipboardUserData    = nullptr;
        io.SetPlatformImeDataFn = [](ImGuiViewport* viewport, ImGuiPlatformImeData* data) {
            if (data->WantVisible)
            {
                SDL_Rect r;
                r.x = (int)(data->InputPos.x - viewport->Pos.x);
                r.y = (int)(data->InputPos.y - viewport->Pos.y + data->InputLineHeight);
                r.w = 1;
                r.h = (int)data->InputLineHeight;
                SDL_SetTextInputRect(&r);
            }
        };
    }

    void imgui_init()
    {
        auto& ts = core::get_subsystem<core::task_system>();
        auto& am = core::get_subsystem<runtime::asset_manager>();

        auto vs_instance = std::make_shared<gfx::shader>(s_embedded_shaders, "vs_ocornut_imgui");
        auto fs_instance = std::make_shared<gfx::shader>(s_embedded_shaders, "fs_ocornut_imgui");

        auto vs_ocornut_imgui = am.load_asset_from_instance<gfx::shader>("embedded:/vs_ocornut_imgui", vs_instance);
        auto fs_ocornut_imgui = am.load_asset_from_instance<gfx::shader>("embedded:/fs_ocornut_imgui", fs_instance);

        ts.push_or_execute_on_owner_thread(
            [](asset_handle<gfx::shader> vs, asset_handle<gfx::shader> fs) { s_program = std::make_unique<gpu_program>(vs, fs); },
            vs_ocornut_imgui,
            fs_ocornut_imgui);

        ImGuiIO& io = gui::GetIO();

        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows

        // imgui_init_platform_interface();
        // imgui_init_clipboard(io);
        // ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();

        // io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;
        // io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;

        io.IniFilename = nullptr;
        // init keyboard mapping
        // io.KeyMap[ImGuiKey_Tab]        = SDL_SCANCODE_TAB;
        // io.KeyMap[ImGuiKey_LeftArrow]  = SDL_SCANCODE_LEFT;
        // io.KeyMap[ImGuiKey_RightArrow] = SDL_SCANCODE_RIGHT;
        // io.KeyMap[ImGuiKey_UpArrow]    = SDL_SCANCODE_UP;
        // io.KeyMap[ImGuiKey_DownArrow]  = SDL_SCANCODE_DOWN;
        // io.KeyMap[ImGuiKey_PageUp]     = SDL_SCANCODE_PAGEUP;
        // io.KeyMap[ImGuiKey_PageDown]   = SDL_SCANCODE_PAGEDOWN;
        // io.KeyMap[ImGuiKey_Home]       = SDL_SCANCODE_HOME;
        // io.KeyMap[ImGuiKey_End]        = SDL_SCANCODE_END;
        // io.KeyMap[ImGuiKey_Insert]     = SDL_SCANCODE_INSERT;
        // io.KeyMap[ImGuiKey_Delete]     = SDL_SCANCODE_DELETE;
        // io.KeyMap[ImGuiKey_Backspace]  = SDL_SCANCODE_BACKSPACE;
        // io.KeyMap[ImGuiKey_Space]      = SDL_SCANCODE_SPACE;
        // io.KeyMap[ImGuiKey_Enter]      = SDL_SCANCODE_RETURN;
        // io.KeyMap[ImGuiKey_Escape]     = SDL_SCANCODE_ESCAPE;
        // io.KeyMap[ImGuiKey_A]          = SDL_SCANCODE_A;
        // io.KeyMap[ImGuiKey_C]          = SDL_SCANCODE_C;
        // io.KeyMap[ImGuiKey_V]          = SDL_SCANCODE_V;
        // io.KeyMap[ImGuiKey_X]          = SDL_SCANCODE_X;
        // io.KeyMap[ImGuiKey_Y]          = SDL_SCANCODE_Y;
        // io.KeyMap[ImGuiKey_Z]          = SDL_SCANCODE_Z;
        std::uint8_t* data   = nullptr;
        int           width  = 0;
        int           height = 0;

        ImFontConfig config;
        config.FontDataOwnedByAtlas = false;
        config.MergeMode            = false;

        gui::AddFont("default", io.Fonts->AddFontDefault(&config));
        gui::AddFont("standard",
                     io.Fonts->AddFontFromMemoryTTF(reinterpret_cast<void*>(std::intptr_t(s_font_default)), sizeof(s_font_default), 20, &config));

        static const ImWchar icons_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
        config.MergeMode                    = true;
        config.PixelSnapH                   = true;
        gui::AddFont(
            "icons",
            io.Fonts->AddFontFromMemoryTTF(
                reinterpret_cast<void*>(std::intptr_t(fontawesome_webfont_ttf)), sizeof(fontawesome_webfont_ttf), 20, &config, icons_ranges));

        config.MergeMode  = false;
        config.PixelSnapH = false;
        gui::AddFont("standard_big",
                     io.Fonts->AddFontFromMemoryTTF(reinterpret_cast<void*>(std::intptr_t(s_font_default)), sizeof(s_font_default), 50, &config));

        config.MergeMode  = true;
        config.PixelSnapH = true;
        gui::AddFont(
            "icons_big",
            io.Fonts->AddFontFromMemoryTTF(
                reinterpret_cast<void*>(std::intptr_t(fontawesome_webfont_ttf)), sizeof(fontawesome_webfont_ttf), 50, &config, icons_ranges));

        io.Fonts->GetTexDataAsRGBA32(&data, &width, &height);

        s_font_texture = std::make_shared<gfx::texture>(static_cast<std::uint16_t>(width),
                                                        static_cast<std::uint16_t>(height),
                                                        false,
                                                        1,
                                                        gfx::texture_format::BGRA8,
                                                        0,
                                                        gfx::copy(data, static_cast<std::uint32_t>(width * height * 4)));

        // Store our identifier
        io.Fonts->SetTexID(s_font_texture.get());
        get_gui_style().load_style();
    }

    void imgui_dispose()
    {
        gui::ClearFonts();
        gui::CleanupTextures();
        s_program.reset();
        s_font_texture.reset();
    }
} // namespace

gui_system::gui_system()
{
    runtime::on_platform_events.connect(this, &gui_system::platform_events);
    runtime::on_frame_begin.connect(this, &gui_system::frame_begin);

    initial_context_ = imgui_create_context(atlas_);
    imgui_init();
}

gui_system::~gui_system()
{
    runtime::on_platform_events.connect(this, &gui_system::platform_events);
    runtime::on_frame_begin.disconnect(this, &gui_system::frame_begin);

    cleanup_cursors();

    imgui_dispose();
    imgui_destroy_context(initial_context_);
}

void gui_system::frame_begin(delta_t /*unused*/) { imgui_frame_begin(); }

uint32_t gui_system::get_draw_calls() const { return s_draw_calls; }

ImGuiContext* gui_system::get_context(uint32_t id)
{
    auto it = contexts_.find(id);
    if (it != contexts_.end())
    {
        return it->second;
    }

    auto ins = contexts_.emplace(id, imgui_create_context(atlas_));
    return ins.first->second;
}

void gui_system::push_context(uint32_t id)
{
    auto context = get_context(id);

    imgui_set_context(context);
}

void gui_system::draw_begin(render_window& window, delta_t dt) { imgui_frame_update(window, dt); }

void gui_system::draw_end() { imgui_frame_end(); }

void gui_system::pop_context() { imgui_set_context(initial_context_); }

void gui_system::platform_events(const std::vector<SDL_Event>& events)
{
    for (const auto& e : events)
    {
        imgui_handle_event(e);
    }
}

gui_style& get_gui_style()
{
    static gui_style s_gui_style;
    return s_gui_style;
}

void gui_style::reset_style() { set_style_colors(hsv_setup()); }

void gui_style::set_style_colors(const hsv_setup& _setup)
{
    setup           = _setup;
    ImVec4 col_text = ImColor::HSV(setup.col_text_hue, setup.col_text_sat, setup.col_text_val);
    ImVec4 col_main = ImColor::HSV(setup.col_main_hue, setup.col_main_sat, setup.col_main_val);
    ImVec4 col_back = ImColor::HSV(setup.col_back_hue, setup.col_back_sat, setup.col_back_val);
    ImVec4 col_area = ImColor::HSV(setup.col_area_hue, setup.col_area_sat, setup.col_area_val);
    float  rounding = setup.frame_rounding;

    ImGuiStyle& style                           = gui::GetStyle();
    style.FrameRounding                         = rounding;
    style.WindowRounding                        = rounding;
    style.Colors[ImGuiCol_Text]                 = ImVec4(col_text.x, col_text.y, col_text.z, 1.00f);
    style.Colors[ImGuiCol_TextDisabled]         = ImVec4(col_text.x, col_text.y, col_text.z, 0.58f);
    style.Colors[ImGuiCol_WindowBg]             = ImVec4(col_back.x, col_back.y, col_back.z, 1.00f);
    style.Colors[ImGuiCol_ChildBg]              = ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
    style.Colors[ImGuiCol_PopupBg]              = ImVec4(col_area.x * 0.8f, col_area.y * 0.8f, col_area.z * 0.8f, 1.00f);
    style.Colors[ImGuiCol_Border]               = ImVec4(col_text.x, col_text.y, col_text.z, 0.30f);
    style.Colors[ImGuiCol_BorderShadow]         = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg]              = ImVec4(col_back.x, col_back.y, col_back.z, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered]       = ImVec4(col_main.x, col_main.y, col_main.z, 0.68f);
    style.Colors[ImGuiCol_FrameBgActive]        = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
    style.Colors[ImGuiCol_TitleBg]              = ImVec4(col_main.x, col_main.y, col_main.z, 1.0f);
    style.Colors[ImGuiCol_TitleBgActive]        = ImVec4(col_main.x, col_main.y, col_main.z, 1.0f);
    style.Colors[ImGuiCol_TitleBgCollapsed]     = ImVec4(col_main.x, col_main.y, col_main.z, 1.0f);
    style.Colors[ImGuiCol_MenuBarBg]            = ImVec4(col_area.x, col_area.y, col_area.z, 1.0f);
    style.Colors[ImGuiCol_ScrollbarBg]          = ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrab]        = ImVec4(col_main.x, col_main.y, col_main.z, 0.31f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.78f);
    style.Colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
    style.Colors[ImGuiCol_CheckMark]            = ImVec4(col_text.x, col_text.y, col_text.z, 0.80f);
    style.Colors[ImGuiCol_SliderGrab]           = ImVec4(col_main.x, col_main.y, col_main.z, 0.54f);
    style.Colors[ImGuiCol_SliderGrabActive]     = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
    style.Colors[ImGuiCol_Button]               = ImVec4(col_main.x, col_main.y, col_main.z, 0.44f);
    style.Colors[ImGuiCol_ButtonHovered]        = ImVec4(col_main.x, col_main.y, col_main.z, 0.86f);
    style.Colors[ImGuiCol_ButtonActive]         = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
    style.Colors[ImGuiCol_Header]               = ImVec4(col_main.x, col_main.y, col_main.z, 0.46f);
    style.Colors[ImGuiCol_HeaderActive]         = ImVec4(col_main.x, col_main.y, col_main.z, 0.86f);
    style.Colors[ImGuiCol_HeaderHovered]        = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
    style.Colors[ImGuiCol_Separator]            = ImVec4(col_main.x, col_main.y, col_main.z, 0.44f);
    style.Colors[ImGuiCol_SeparatorHovered]     = ImVec4(col_main.x, col_main.y, col_main.z, 0.86f);
    style.Colors[ImGuiCol_SeparatorActive]      = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
    style.Colors[ImGuiCol_Tab]                  = style.Colors[ImGuiCol_Header];
    style.Colors[ImGuiCol_TabHovered]           = style.Colors[ImGuiCol_HeaderHovered];
    style.Colors[ImGuiCol_TabActive]            = style.Colors[ImGuiCol_HeaderActive];
    style.Colors[ImGuiCol_TabUnfocused]         = style.Colors[ImGuiCol_Tab];
    style.Colors[ImGuiCol_TabUnfocusedActive]   = style.Colors[ImGuiCol_TabActive];
    style.Colors[ImGuiCol_ResizeGrip]           = ImVec4(col_main.x, col_main.y, col_main.z, 0.20f);
    style.Colors[ImGuiCol_ResizeGripHovered]    = ImVec4(col_main.x, col_main.y, col_main.z, 0.78f);
    style.Colors[ImGuiCol_ResizeGripActive]     = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
    style.Colors[ImGuiCol_PlotLines]            = ImVec4(col_text.x, col_text.y, col_text.z, 0.63f);
    style.Colors[ImGuiCol_PlotLinesHovered]     = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram]        = ImVec4(col_text.x, col_text.y, col_text.z, 0.63f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg]       = ImVec4(col_main.x, col_main.y, col_main.z, 0.43f);
    style.Colors[ImGuiCol_NavHighlight]         = style.Colors[ImGuiCol_HeaderHovered];
}

void gui_style::load_style()
{
    const fs::path absoluteKey = fs::resolve_protocol("editor:/config/style.cfg");
    fs::error_code err;
    if (!fs::exists(absoluteKey, err))
    {
        save_style();
    }
    else
    {
        std::ifstream                  output(absoluteKey.string());
        cereal::iarchive_associative_t ar(output);

        try_load(ar, cereal::make_nvp("style", setup));
    }
}

void gui_style::save_style()
{
    const fs::path                 absoluteKey = fs::resolve_protocol("editor:/config/style.cfg");
    std::ofstream                  output(absoluteKey.string());
    cereal::oarchive_associative_t ar(output);

    try_save(ar, cereal::make_nvp("style", setup));
}

gui_style::hsv_setup gui_style::get_dark_style()
{
    hsv_setup result;
    result.col_main_hue = 0.0f / 255.0f;
    result.col_main_sat = 0.0f / 255.0f;
    result.col_main_val = 80.0f / 255.0f;

    result.col_area_hue = 0.0f / 255.0f;
    result.col_area_sat = 0.0f / 255.0f;
    result.col_area_val = 50.0f / 255.0f;

    result.col_back_hue = 0.0f / 255.0f;
    result.col_back_sat = 0.0f / 255.0f;
    result.col_back_val = 35.0f / 255.0f;

    result.col_text_hue   = 0.0f / 255.0f;
    result.col_text_sat   = 0.0f / 255.0f;
    result.col_text_val   = 255.0f / 255.0f;
    result.frame_rounding = 0.0f;

    return result;
}

gui_style::hsv_setup gui_style::get_lighter_red()
{
    hsv_setup result;
    result.col_main_hue = 0.0f / 255.0f;
    result.col_main_sat = 200.0f / 255.0f;
    result.col_main_val = 170.0f / 255.0f;

    result.col_area_hue = 0.0f / 255.0f;
    result.col_area_sat = 0.0f / 255.0f;
    result.col_area_val = 80.0f / 255.0f;

    result.col_back_hue = 0.0f / 255.0f;
    result.col_back_sat = 0.0f / 255.0f;
    result.col_back_val = 35.0f / 255.0f;

    result.col_text_hue   = 0.0f / 255.0f;
    result.col_text_sat   = 0.0f / 255.0f;
    result.col_text_val   = 255.0f / 255.0f;
    result.frame_rounding = 0.0f;

    return result;
}
