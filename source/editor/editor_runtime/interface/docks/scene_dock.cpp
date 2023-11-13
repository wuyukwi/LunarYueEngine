#include "scene_dock.h"
#include "../../assets/asset_extensions.h"
#include "../../editing/editing_system.h"
#include "../../system/project_manager.h"

#include <core/graphics/render_pass.h>
#include <core/logging/logging.h>
#include <core/simulation/simulation.h>
#include <core/string_utils/string_utils.h>
#include <core/system/subsystem.h>

#include <runtime/assets/asset_handle.h>
#include <runtime/assets/asset_manager.h>
#include <runtime/ecs/components/camera_component.h>
#include <runtime/ecs/components/model_component.h>
#include <runtime/ecs/components/transform_component.h>
#include <runtime/ecs/constructs/prefab.h>
#include <runtime/ecs/constructs/utils.h>
#include <runtime/input/input.h>
#include <runtime/rendering/camera.h>
#include <runtime/rendering/mesh.h>
#include <runtime/rendering/render_window.h>
#include <runtime/rendering/renderer.h>

#include <numeric>

static bool bar(float _width, float _maxWidth, float _height, const ImVec4& _color)
{
    const ImGuiStyle& style = gui::GetStyle();

    ImVec4 hoveredColor(_color.x + _color.x * 0.1f, _color.y + _color.y * 0.1f, _color.z + _color.z * 0.1f, _color.w + _color.w * 0.1f);

    gui::PushStyleColor(ImGuiCol_Button, _color);
    gui::PushStyleColor(ImGuiCol_ButtonHovered, hoveredColor);
    gui::PushStyleColor(ImGuiCol_ButtonActive, _color);
    gui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
    gui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, style.ItemSpacing.y));

    bool itemHovered = false;

    gui::Button("##barbtn", ImVec2(_width, _height));
    itemHovered |= gui::IsItemHovered();

    gui::SameLine();
    gui::InvisibleButton("##barinvis", ImVec2(_maxWidth - _width + 1, _height));
    itemHovered |= gui::IsItemHovered();

    gui::PopStyleVar(2);
    gui::PopStyleColor(3);

    return itemHovered;
}

static void resource_bar(const char* _name, const char* _tooltip, uint32_t _num, uint32_t _max, float _maxWidth, float _height)
{
    bool itemHovered = false;

    gui::AlignTextToFramePadding();
    gui::Text("%s: %4d / %4d", _name, _num, _max);
    itemHovered |= gui::IsItemHovered();
    gui::SameLine();

    const float         percentage = float(_num) / float(_max);
    static const ImVec4 color(0.5f, 0.5f, 0.5f, 1.0f);

    itemHovered |= bar(std::max(1.0f, percentage * _maxWidth), _maxWidth, _height, color);
    gui::SameLine();

    gui::Text("%5.2f%%", double(percentage * 100.0f));

    if (itemHovered)
    {
        gui::BeginTooltip();
        gui::Text("%s %5.2f%%", _tooltip, double(percentage * 100.0f));
        gui::EndTooltip();
    }
}

void scene_dock::show_statistics(const ImVec2& area, unsigned int fps, bool& show_gbuffer)
{
    auto stat_pos = gui::GetCurrentWindow()->Pos + gui::GetCursorPos();
    gui::SetNextWindowPos(stat_pos);
    gui::SetNextWindowSizeConstraints(ImVec2(0, 0), area - gui::GetStyle().WindowPadding);

    if (gui::Begin((ICON_FA_BAR_CHART "\tSTATISTICS##" + title).c_str(),
                   nullptr,
                   ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize))
    {

        auto stats = gfx::get_stats();
        gui::Text("Frame %0.3f [ms], %u FPS", 1000.0 / double(fps), fps);

        const double to_cpu_ms = 1000.0 / double(stats->cpuTimerFreq);
        const double to_gpu_ms = 1000.0 / double(stats->gpuTimerFreq);

        if (gui::CollapsingHeader(ICON_FA_INFO_CIRCLE "\tRender Info"))
        {
            gui::PushFont("default");

            gui::Text("Submit CPU %0.3f, GPU %0.3f (L: %d)",
                      double(stats->cpuTimeEnd - stats->cpuTimeBegin) * to_cpu_ms,
                      double(stats->gpuTimeEnd - stats->gpuTimeBegin) * to_gpu_ms,
                      stats->maxGpuLatency);
            if (-std::numeric_limits<std::int64_t>::max() != stats->gpuMemoryUsed)
            {
                char tmp0[64];
                bx::prettify(tmp0, 64, uint64_t(stats->gpuMemoryUsed));
                char tmp1[64];
                bx::prettify(tmp1, 64, uint64_t(stats->gpuMemoryMax));
                gui::Text("GPU mem: %s / %s", tmp0, tmp1);
                char tmp2[64];
                bx::prettify(tmp2, 64, uint64_t(stats->rtMemoryUsed));
                gui::Text("Render Target mem: %s / %s", tmp2, tmp0);
                char tmp3[64];
                bx::prettify(tmp3, 64, uint64_t(stats->textureMemoryUsed));
                gui::Text("Texture mem: %s / %s", tmp3, tmp0);
            }

            gui::Separator();

            const auto&   gui_sys          = core::get_subsystem<gui_system>();
            std::uint32_t ui_draw_calls    = gui_sys.get_draw_calls();
            std::uint32_t total_primitives = std::accumulate(std::begin(stats->numPrims), std::end(stats->numPrims), 0u);

            gui::Text("Primitives %u", total_primitives);
            gui::Text("Total Draw Calls: %u", stats->numDraw);
            gui::Text("UI Draw Calls: %u", ui_draw_calls);
            gui::Text("Scene Draw Calls: %u", math::abs<std::uint32_t>(stats->numDraw - ui_draw_calls));
            gui::PopFont();
        }

        if (gui::CollapsingHeader(ICON_FA_PUZZLE_PIECE "\tResources"))
        {
            const auto caps = gfx::get_caps();

            const float itemHeight = gui::GetTextLineHeightWithSpacing();
            const float maxWidth   = 90.0f;

            gui::PushFont("default");
            gui::AlignTextToFramePadding();
            gui::Text("Res: Num  / Max");
            resource_bar("DIB", "Dynamic index buffers", stats->numDynamicIndexBuffers, caps->limits.maxDynamicIndexBuffers, maxWidth, itemHeight);
            resource_bar("DVB", "Dynamic vertex buffers", stats->numDynamicVertexBuffers, caps->limits.maxDynamicVertexBuffers, maxWidth, itemHeight);
            resource_bar(" FB", "Frame buffers", stats->numFrameBuffers, caps->limits.maxFrameBuffers, maxWidth, itemHeight);
            resource_bar(" IB", "Index buffers", stats->numIndexBuffers, caps->limits.maxIndexBuffers, maxWidth, itemHeight);
            resource_bar(" OQ", "Occlusion queries", stats->numOcclusionQueries, caps->limits.maxOcclusionQueries, maxWidth, itemHeight);
            resource_bar("  P", "Programs", stats->numPrograms, caps->limits.maxPrograms, maxWidth, itemHeight);
            resource_bar("  S", "Shaders", stats->numShaders, caps->limits.maxShaders, maxWidth, itemHeight);
            resource_bar("  T", "Textures", stats->numTextures, caps->limits.maxTextures, maxWidth, itemHeight);
            resource_bar("  U", "Uniforms", stats->numUniforms, caps->limits.maxUniforms, maxWidth, itemHeight);
            resource_bar(" VB", "Vertex buffers", stats->numVertexBuffers, caps->limits.maxVertexBuffers, maxWidth, itemHeight);
            resource_bar(" VD", "Vertex layouts", stats->numVertexLayouts, caps->limits.maxVertexLayouts, maxWidth, itemHeight);
            gui::PopFont();
        }

        if (gui::CollapsingHeader(ICON_FA_CLOCK_O "\tProfiler"))
        {
            if (gui::Checkbox("Enable profiler", &enable_profiler))
            {
                if (enable_profiler)
                {
                    gfx::set_debug(BGFX_DEBUG_PROFILER);
                }
                else
                {
                    gfx::set_debug(BGFX_DEBUG_NONE);
                }
            }

            gui::PushFont("default");

            if (0 == stats->numViews)
            {
                gui::Text("Profiler is not enabled.");
            }
            else
            {
                ImVec4 cpuColor(0.5f, 1.0f, 0.5f, 1.0f);
                ImVec4 gpuColor(0.5f, 0.5f, 1.0f, 1.0f);

                const float  itemHeight            = ImGui::GetTextLineHeightWithSpacing();
                const float  itemHeightWithSpacing = ImGui::GetFrameHeightWithSpacing();
                const double toCpuMs               = 1000.0 / double(stats->cpuTimerFreq);
                const double toGpuMs               = 1000.0 / double(stats->gpuTimerFreq);
                const float  scale                 = 3.0f;

                if (ImGui::BeginListBox("Encoders", ImVec2(ImGui::GetWindowWidth(), stats->numEncoders * itemHeightWithSpacing)))
                {
                    ImGuiListClipper clipper;
                    clipper.Begin(stats->numEncoders, itemHeight);

                    while (clipper.Step())
                    {
                        for (int32_t pos = clipper.DisplayStart; pos < clipper.DisplayEnd; ++pos)
                        {
                            const bgfx::EncoderStats& encoderStats = stats->encoderStats[pos];

                            ImGui::Text("%3d", pos);
                            ImGui::SameLine(64.0f);

                            const float maxWidth = 30.0f * scale;
                            const float cpuMs    = float((encoderStats.cpuTimeEnd - encoderStats.cpuTimeBegin) * toCpuMs);
                            const float cpuWidth = bx::clamp(cpuMs * scale, 1.0f, maxWidth);

                            if (bar(cpuWidth, maxWidth, itemHeight, cpuColor))
                            {
                                ImGui::SetTooltip("Encoder %d, CPU: %f [ms]", pos, cpuMs);
                            }
                        }
                    }

                    ImGui::EndListBox();
                }

                ImGui::Separator();

                if (ImGui::BeginListBox("Views", ImVec2(ImGui::GetWindowWidth(), stats->numViews * itemHeightWithSpacing)))
                {
                    ImGuiListClipper clipper;
                    clipper.Begin(stats->numViews, itemHeight);

                    while (clipper.Step())
                    {
                        for (int32_t pos = clipper.DisplayStart; pos < clipper.DisplayEnd; ++pos)
                        {
                            const bgfx::ViewStats& viewStats = stats->viewStats[pos];

                            ImGui::Text("%3d %3d %s", pos, viewStats.view, viewStats.name);

                            const float maxWidth       = 30.0f * scale;
                            const float cpuTimeElapsed = float((viewStats.cpuTimeEnd - viewStats.cpuTimeBegin) * toCpuMs);
                            const float gpuTimeElapsed = float((viewStats.gpuTimeEnd - viewStats.gpuTimeBegin) * toGpuMs);
                            const float cpuWidth       = bx::clamp(cpuTimeElapsed * scale, 1.0f, maxWidth);
                            const float gpuWidth       = bx::clamp(gpuTimeElapsed * scale, 1.0f, maxWidth);

                            ImGui::SameLine(64.0f);

                            if (bar(cpuWidth, maxWidth, itemHeight, cpuColor))
                            {
                                ImGui::SetTooltip("View %d \"%s\", CPU: %f [ms]", pos, viewStats.name, cpuTimeElapsed);
                            }

                            ImGui::SameLine();
                            if (bar(gpuWidth, maxWidth, itemHeight, gpuColor))
                            {
                                ImGui::SetTooltip("View: %d \"%s\", GPU: %f [ms]", pos, viewStats.name, gpuTimeElapsed);
                            }
                        }
                    }

                    ImGui::EndListBox();
                }
            }
            gui::PopFont();
        }

        gui::Separator();
        gui::Checkbox("SHOW G-BUFFER", &show_gbuffer);
    }
    gui::End();
}

void draw_selected_camera(const ImVec2& size)
{
    auto& input         = core::get_subsystem<runtime::input>();
    auto& es            = core::get_subsystem<editor::editing_system>();
    auto& selected      = es.selection_data.object;
    auto& editor_camera = es.camera;

    if (selected.is_type<runtime::entity>())
    {
        auto sel = selected.get_value<runtime::entity>();

        if (sel && (editor_camera != sel) && sel.has_component<camera_component>())
        {
            const auto  selected_camera = sel.get_component<camera_component>().lock();
            const auto& camera          = selected_camera->get_camera();
            auto&       render_view     = selected_camera->get_render_view();
            const auto& viewport_size   = camera.get_viewport_size();
            const auto  surface         = render_view.get_output_fbo(viewport_size);

            float  factor = std::min(size.x / float(viewport_size.width), size.y / float(viewport_size.height)) / 4.0f;
            ImVec2 bounds(viewport_size.width * factor, viewport_size.height * factor);
            auto   p = gui::GetWindowPos();
            p.x += size.x - bounds.x - 20.0f;
            p.y += size.y - bounds.y - 40.0f;
            gui::SetNextWindowPos(p);
            if (gui::Begin("Camera Preview",
                           nullptr,
                           ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                               ImGuiWindowFlags_AlwaysAutoResize))
            {
                auto tex = surface->get_attachment(0).texture;
                gui::Image(gui::get_info(tex), bounds);
            }
            gui::End();

            if (input.is_key_pressed(SDLK_f) && sel.has_component<transform_component>())
            {
                auto transform          = editor_camera.get_component<transform_component>().lock();
                auto transform_selected = sel.get_component<transform_component>().lock();
                transform_selected->set_transform(transform->get_transform());
            }
        }
    }
}

void manipulation_gizmos()
{
    auto& input         = core::get_subsystem<runtime::input>();
    auto& es            = core::get_subsystem<editor::editing_system>();
    auto& selected      = es.selection_data.object;
    auto& editor_camera = es.camera;
    auto& operation     = es.operation;
    auto& mode          = es.mode;

    if (!input.is_mouse_button_down(SDL_BUTTON_RIGHT) && !gui::IsAnyItemActive() && !imguizmo::is_using())
    {
        if (input.is_key_pressed(SDLK_w))
        {
            operation = imguizmo::operation::translate;
        }
        if (input.is_key_pressed(SDLK_e))
        {
            operation = imguizmo::operation::rotate;
        }
        if (input.is_key_pressed(SDLK_r))
        {
            operation = imguizmo::operation::scale;
            mode      = imguizmo::mode::local;
        }
        if (input.is_key_pressed(SDLK_t))
        {
            mode = imguizmo::mode::local;
        }
        if (input.is_key_pressed(SDLK_y) && operation != imguizmo::operation::scale)
        {
            mode = imguizmo::mode::world;
        }
    }

    if (selected && selected.is_type<runtime::entity>())
    {
        auto sel = selected.get_value<runtime::entity>();
        if (sel && sel != editor_camera && sel.has_component<transform_component>())
        {
            auto p = gui::GetItemRectMin();
            auto s = gui::GetItemRectSize();
            imguizmo::set_view_rect(p.x, p.y, s.x, s.y);
            auto            camera_comp    = editor_camera.get_component<camera_component>().lock();
            auto            transform_comp = sel.get_component<transform_component>().lock();
            auto            transform      = transform_comp->get_transform();
            math::transform delta;
            float*          snap = nullptr;
            if (input.is_key_down(SDLK_LCTRL))
            {
                if (operation == imguizmo::operation::translate)
                {
                    snap = &es.snap_data.translation_snap[0];
                }
                else if (operation == imguizmo::operation::rotate)
                {
                    snap = &es.snap_data.rotation_degree_snap;
                }
                else if (operation == imguizmo::operation::scale)
                {
                    snap = &es.snap_data.scale_snap;
                }
            }
            const auto& camera = camera_comp->get_camera();
            math::mat4  output = transform;
            imguizmo::manipulate(camera.get_view(), camera.get_projection(), operation, mode, math::value_ptr(output), nullptr, snap);

            transform_comp->set_transform(output);

            //						if(sel.has_component<model_component>())
            //						{
            //							const auto model_comp = sel.get_component<model_component>();
            //							const auto model_comp_ptr = model_comp.lock().get();
            //							const auto& model = model_comp_ptr->get_model();
            //							if(!model.is_valid())
            //								return;

            //							const auto mesh = model.get_lod(0);
            //							if(!mesh)
            //								return;

            //							auto rect = mesh->calculate_screen_rect(transform, camera);

            //							gui::GetCurrentWindow()->DrawList->AddRect(ImVec2(rect.left, rect.top),
            //																	   ImVec2(rect.right, rect.bottom),
            //																	   gui::GetColorU32(ImVec4(1.0f, 0.0f,
            //			 0.0f, 1.0f)));
            //						}
        }
    }
}

void handle_camera_movement()
{
    if (!gui::IsWindowFocused())
    {
        return;
    }

    if (!gui::IsWindowHovered())
    {
        return;
    }

    auto& es    = core::get_subsystem<editor::editing_system>();
    auto& input = core::get_subsystem<runtime::input>();
    auto& sim   = core::get_subsystem<core::simulation>();
    auto& rend  = core::get_subsystem<runtime::renderer>();

    if (input.is_mouse_button_down(SDL_BUTTON_RIGHT))
    {
        auto wnd = rend.get_focused_window();
        if (wnd)
        {
            auto    pos         = wnd->get_mouse_position_in_window();
            auto    result      = pos;
            auto    win_pos     = gui::GetWindowPos();
            auto    win_size    = gui::GetWindowSize();
            int32_t border_size = 50.0f;
            auto    max_x       = int32_t(win_pos.x + win_size.x) - border_size;
            auto    max_y       = int32_t(win_pos.y + win_size.y) - border_size;
            auto    min_x       = int32_t(win_pos.x) + border_size;
            auto    min_y       = int32_t(win_pos.y) + border_size;

            if (result[0] > max_x)
            {
                result[0] = min_x;
            }
            if (result[0] < min_x)
            {
                result[0] = max_x;
            }

            if (result[1] > max_y)
            {
                result[1] = min_y;
            }
            if (result[1] < min_y)
            {
                result[1] = max_y;
            }
            if (result != pos)
            {
                wnd->set_mouse_position_in_window(result);
                SDL_Event ev;
                ev.type = SDL_MOUSEMOTION;

                ev.motion.x = result[0];
                ev.motion.y = result[1];
                input.mouse_event(ev);
                // input.mouse_event(ev);
            }
        }
    }

    auto& editor_camera = es.camera;
    auto  dt            = sim.get_delta_time().count();

    auto  transform      = editor_camera.get_component<transform_component>().lock();
    float movement_speed = 10.0f;
    float rotation_speed = 0.2f;
    float multiplier     = 5.0f;
    auto  delta_move     = input.get_cursor_delta_move();

    if (input.is_mouse_button_down(SDL_BUTTON_MIDDLE))
    {
        if (input.is_key_down(SDLK_LSHIFT))
        {
            movement_speed *= multiplier;
        }

        if (input.is_key_down(SDLK_w))
        {
            transform->move_local({0.0f, 0.0f, movement_speed * dt});
        }

        if (input.is_key_down(SDLK_s))
        {
            transform->move_local({0.0f, 0.0f, -movement_speed * dt});
        }

        if (input.is_key_down(SDLK_a))
        {
            transform->move_local({-movement_speed * dt, 0.0f, 0.0f});
        }

        if (input.is_key_down(SDLK_d))
        {
            transform->move_local({movement_speed * dt, 0.0f, 0.0f});
        }

        if (input.is_key_down(SDLK_UP))
        {
            transform->move_local({0.0f, 0.0f, movement_speed * dt});
        }

        if (input.is_key_down(SDLK_DOWN))
        {
            transform->move_local({0.0f, 0.0f, -movement_speed * dt});
        }

        if (input.is_key_down(SDLK_LEFT))
        {
            transform->move_local({-movement_speed * dt, 0.0f, 0.0f});
        }

        if (input.is_key_down(SDLK_RIGHT))
        {
            transform->move_local({movement_speed * dt, 0.0f, 0.0f});
        }

        if (input.is_key_down(SDLK_SPACE))
        {
            transform->move_local({0.0f, movement_speed * dt, 0.0f});
        }

        if (input.is_key_down(SDLK_LCTRL))
        {
            transform->move_local({0.0f, -movement_speed * dt, 0.0f});
        }
        auto x = static_cast<float>(delta_move.x);
        auto y = static_cast<float>(delta_move.y);

        // if(x != 0.0f|| y != 0.0f)
        {
            // Make each pixel correspond to a quarter of a degree.
            float dx = x * rotation_speed;
            float dy = y * rotation_speed;

            transform->rotate(0.0f, dx, 0.0f);
            transform->rotate_local(dy, 0.0f, 0.0f);
        }
    }

    float delta_wheel = input.get_mouse_wheel_scroll_delta_move();
    transform->move_local({0.0f, 0.0f, 14.0f * movement_speed * delta_wheel * dt});
}

static void process_drag_drop_target(const std::shared_ptr<camera_component>& camera_comp)
{
    auto& ecs = core::get_subsystem<runtime::entity_component_system>();
    auto& am  = core::get_subsystem<runtime::asset_manager>();
    auto& es  = core::get_subsystem<editor::editing_system>();

    if (gui::BeginDragDropTarget())
    {
        if (gui::IsDragDropPayloadBeingAccepted())
        {
            gui::SetMouseCursor(ImGuiMouseCursor_Hand);
        }
        else
        {
            gui::SetMouseCursor(ImGuiMouseCursor_NotAllowed);
        }

        for (const auto& type : ex::get_suported_formats<prefab>())
        {
            auto payload = gui::AcceptDragDropPayload(type.c_str());
            if (payload != nullptr)
            {
                std::string absolute_path(reinterpret_cast<const char*>(payload->Data), std::size_t(payload->DataSize));

                std::string key   = fs::convert_to_protocol(fs::path(absolute_path)).generic_string();
                using asset_t     = prefab;
                using entry_t     = asset_handle<asset_t>;
                auto entry        = entry_t {};
                auto entry_future = am.find_asset_entry<asset_t>(key);
                if (entry_future.is_ready())
                {
                    entry = entry_future.get();
                }
                if (entry)
                {
                    auto object     = entry->instantiate();
                    auto trans_comp = object.get_component<transform_component>().lock();
                    if (trans_comp)
                    {
                        math::vec3 projected_pos;
                        auto       cursor_pos = gui::GetMousePos();
                        if (camera_comp->get_camera().viewport_to_world(
                                math::vec2 {cursor_pos.x, cursor_pos.y},
                                math::plane::from_point_normal(math::vec3 {0.0f, 0.0f, 0.0f}, math::vec3 {0.0f, 1.0f, 0.0f}),
                                projected_pos,
                                false))
                        {

                            trans_comp->set_position(projected_pos);
                        }
                    }
                    es.select(object);
                }
            }
        }
        for (const auto& type : ex::get_suported_formats<mesh>())
        {
            auto payload = gui::AcceptDragDropPayload(type.c_str());
            if (payload != nullptr)
            {
                std::string absolute_path(reinterpret_cast<const char*>(payload->Data), std::size_t(payload->DataSize));

                std::string key   = fs::convert_to_protocol(fs::path(absolute_path)).generic_string();
                using asset_t     = mesh;
                using entry_t     = asset_handle<asset_t>;
                auto entry        = entry_t {};
                auto entry_future = am.find_asset_entry<asset_t>(key);
                if (entry_future.is_ready())
                {
                    entry = entry_future.get();
                }
                if (entry)
                {
                    model mdl;
                    mdl.set_lod(entry, 0);

                    auto object = ecs.create();
                    // Add component and configure it.
                    auto trans_comp = object.assign<transform_component>().lock();
                    if (trans_comp)
                    {
                        math::vec3 projected_pos;
                        auto       cursor_pos = gui::GetMousePos();
                        if (camera_comp->get_camera().viewport_to_world(
                                math::vec2 {cursor_pos.x, cursor_pos.y},
                                math::plane::from_point_normal(math::vec3 {0.0f, 0.0f, 0.0f}, math::vec3 {0.0f, 1.0f, 0.0f}),
                                projected_pos,
                                false))
                        {

                            trans_comp->set_position(projected_pos);
                        }
                    }
                    // Add component and configure it.
                    auto model_comp = object.assign<model_component>().lock();
                    model_comp->set_casts_shadow(true);
                    model_comp->set_casts_reflection(false);
                    model_comp->set_model(mdl);

                    es.select(object);
                }
            }
        }

        gui::EndDragDropTarget();
    }
}

void scene_dock::render(const ImVec2& area)
{
    auto& es       = core::get_subsystem<editor::editing_system>();
    auto& renderer = core::get_subsystem<runtime::renderer>();
    auto& input    = core::get_subsystem<runtime::input>();
    auto& sim      = core::get_subsystem<core::simulation>();

    auto  window        = renderer.get_focused_window();
    auto& editor_camera = es.camera;
    auto& selected      = es.selection_data.object;

    bool has_edit_camera = editor_camera && editor_camera.has_component<camera_component>() && editor_camera.has_component<transform_component>();

    show_statistics(area, sim.get_fps(), show_gbuffer);

    if (!has_edit_camera)
    {
        return;
    }

    auto size = gui::GetContentRegionAvail();
    auto pos  = gui::GetCursorScreenPos();
    draw_selected_camera(size);

    auto camera_comp = editor_camera.get_component<camera_component>().lock();
    if (size.x > 0 && size.y > 0)
    {
        camera_comp->get_camera().set_viewport_pos({static_cast<std::uint32_t>(pos.x), static_cast<std::uint32_t>(pos.y)});
        camera_comp->set_viewport_size({static_cast<std::uint32_t>(size.x), static_cast<std::uint32_t>(size.y)});

        const auto& camera        = camera_comp->get_camera();
        auto&       render_view   = camera_comp->get_render_view();
        const auto& viewport_size = camera.get_viewport_size();
        const auto  surface       = render_view.get_output_fbo(viewport_size);
        auto        tex           = surface->get_attachment(0).texture;
        gui::Image(gui::get_info(tex), size);

        if (gui::IsItemClicked(1) || gui::IsItemClicked(2))
        {
            gui::SetWindowFocus();
            if (window != nullptr)
            {
                window->set_mouse_cursor_visible(false);
            }
        }

        manipulation_gizmos();
        handle_camera_movement();

        if (gui::IsWindowFocused())
        {
            gui::PushStyleColor(ImGuiCol_Border, gui::GetStyle().Colors[ImGuiCol_Button]);
            gui::RenderFrameEx(gui::GetItemRectMin(), gui::GetItemRectMax(), true, 0.0f, 2.0f);
            gui::PopStyleColor();

            if (input.is_key_pressed(SDLK_DELETE))
            {
                if (selected && selected.is_type<runtime::entity>())
                {
                    auto sel = selected.get_value<runtime::entity>();
                    if (sel && sel != editor_camera)
                    {
                        sel.destroy();
                        es.unselect();
                    }
                }
            }

            if (input.is_key_pressed(SDLK_d))
            {
                if (input.is_key_down(SDLK_LCTRL))
                {
                    if (selected && selected.is_type<runtime::entity>())
                    {
                        auto sel = selected.get_value<runtime::entity>();
                        if (sel && sel != editor_camera)
                        {
                            auto clone = ecs::utils::clone_entity(sel);
                            clone.get_component<transform_component>().lock()->set_parent(
                                sel.get_component<transform_component>().lock()->get_parent(), false, true);
                            es.select(clone);
                        }
                    }
                }
            }
        }

        if (gui::IsMouseReleased(1) || gui::IsMouseReleased(2))
        {
            if (window != nullptr)
            {
                window->set_mouse_cursor_visible(true);
            }
        }

        if (show_gbuffer)
        {
            auto g_buffer_fbo = render_view.get_g_buffer_fbo(viewport_size).get();
            for (std::uint32_t i = 0; i < g_buffer_fbo->get_attachment_count(); ++i)
            {
                const auto tex = g_buffer_fbo->get_attachment(i).texture;
                gui::Image(gui::get_info(tex), size);

                if (gui::IsItemClicked(1) || gui::IsItemClicked(2))
                {
                    gui::SetWindowFocus();
                    if (window != nullptr)
                    {
                        window->set_mouse_cursor_visible(false);
                    }
                }
            }
        }
    }

    process_drag_drop_target(camera_comp);
}

scene_dock::scene_dock(const std::string& dtitle, bool close_button, const ImVec2& min_size)
{

    initialize(dtitle, close_button, min_size, std::bind(&scene_dock::render, this, std::placeholders::_1));
}
