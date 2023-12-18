#include "hierarchy_dock.h"
#include "editor_runtime/assets/asset_extensions.h"
#include "editor_runtime/editing/editing_system.h"
#include "editor_runtime/interface/gui_system.h"
#include "editor_runtime/system/project_manager.h"

#include <core/filesystem/filesystem.h>
#include <core/logging/logging.h>
#include <core/system/subsystem.h>

#include <runtime/assets/asset_handle.h>
#include <runtime/assets/asset_manager.h>
#include <runtime/ecs/components/audio_source_component.h>
#include <runtime/ecs/components/camera_component.h>
#include <runtime/ecs/components/light_component.h>
#include <runtime/ecs/components/model_component.h>
#include <runtime/ecs/components/reflection_probe_component.h>
#include <runtime/ecs/components/transform_component.h>
#include <runtime/ecs/constructs/prefab.h>
#include <runtime/ecs/constructs/utils.h>
#include <runtime/ecs/systems/scene_graph.h>
#include <runtime/input/input.h>
#include <runtime/rendering/mesh.h>
namespace
{
    math::bbox calc_bounds(runtime::entity entity)
    {
        const math::vec3 one            = {1.0f, 1.0f, 1.0f};
        math::bbox       bounds         = math::bbox(-one, one);
        auto             ent_trans_comp = entity.get_component<transform_component>().lock();
        if (ent_trans_comp)
        {
            auto target_pos = ent_trans_comp->get_position();
            bounds          = math::bbox(target_pos - one, target_pos + one);

            auto ent_model_comp = entity.get_component<model_component>().lock();
            if (ent_model_comp)
            {
                const auto& model = ent_model_comp->get_model();
                if (model.is_valid())
                {
                    const auto current_mesh = model.get_lod(0);
                    if (current_mesh)
                    {
                        bounds = current_mesh->get_bounds();
                    }
                }
            }
            const auto& world = ent_trans_comp->get_transform();
            bounds            = math::bbox::mul(bounds, world);
        }
        return bounds;
    };

    void focus_entity_on_bounds(runtime::entity entity, const math::bbox& bounds)
    {
        auto        trans_comp  = entity.get_component<transform_component>().lock();
        auto        camera_comp = entity.get_component<camera_component>().lock();
        const auto& cam         = camera_comp->get_camera();

        math::vec3 cen  = bounds.get_center();
        math::vec3 size = bounds.get_dimensions();

        float aspect = cam.get_aspect_ratio();
        float fov    = cam.get_fov();
        // Get the radius of a sphere circumscribing the bounds
        float radius = math::length(size) / 2.0f;
        // Get the horizontal FOV, since it may be the limiting of the two FOVs to properly
        // encapsulate the objects
        float horizontalFOV = math::degrees(2.0f * math::atan(math::tan(math::radians(fov) / 2.0f) * aspect));
        // Use the smaller FOV as it limits what would get cut off by the frustum
        float mfov = math::min(fov, horizontalFOV);
        float dist = radius / (math::sin(math::radians(mfov) / 2.0f));

        camera_comp->set_ortho_size(radius);
        trans_comp->set_position(cen - dist * trans_comp->get_z_axis());
        trans_comp->look_at(cen);
    }

    enum class context_action
    {
        none,
        rename,
    };
} // namespace
static context_action check_context_menu(runtime::entity entity)
{
    auto&          es            = core::get_subsystem<editor::editing_system>();
    auto&          ecs           = core::get_subsystem<runtime::entity_component_system>();
    auto&          editor_camera = es.camera;
    context_action action        = context_action::none;
    if (entity && entity != editor_camera)
    {
        if (gui::BeginPopupContextItem("Entity Context Menu"))
        {
            if (gui::MenuItem("CREATE CHILD"))
            {
                auto object = ecs.create();
                object.assign<transform_component>().lock()->set_parent(entity);
            }

            if (gui::MenuItem("RENAME", "F2"))
            {
                action = context_action::rename;
            }

            if (gui::MenuItem("DUPLICATE", "CTRL + D"))
            {
                auto object = ecs::utils::clone_entity(entity);

                auto obj_trans_comp = object.get_component<transform_component>().lock();
                auto ent_trans_comp = entity.get_component<transform_component>().lock();
                if (obj_trans_comp && ent_trans_comp)
                {
                    obj_trans_comp->set_parent(ent_trans_comp->get_parent(), false, true);
                }

                es.select(object);
            }

            if (gui::MenuItem("DELETE", "DEL"))
            {
                entity.destroy();
            }

            if (gui::MenuItem("FOCUS", "SHIFT + F"))
            {
                if (editor_camera.has_component<transform_component>() && editor_camera.has_component<camera_component>())
                {
                    auto bounds = calc_bounds(entity);
                    focus_entity_on_bounds(editor_camera, bounds);
                }
            }

            gui::EndPopup();
        }
    }
    else
    {
        if (gui::BeginPopupContextWindow())
        {
            if (gui::MenuItem("CREATE EMPTY"))
            {
                auto object = ecs.create();
                object.assign<transform_component>();
                es.select(object);
            }

            if (gui::BeginMenu("3D OBJECTS"))
            {
                static const std::map<std::string, std::vector<std::string>> menu_objects = {
                    {"BASIC", {"SPHERE", "CUBE", "PLANE", "CYLINDER", "CAPSULE", "CONE", "TORUS", "TEAPOT"}},
                    {"POLYGONS", {"ICOSAHEDRON", "DODECAHEDRON"}},
                    {"ICOSPHERES", {"ICOSPHERE0", "ICOSPHERE1", "ICOSPHERE2", "ICOSPHERE3", "ICOSPHERE4", "ICOSPHERE5"}}};

                auto& am = core::get_subsystem<runtime::asset_manager>();
                for (const auto& p : menu_objects)
                {
                    const auto& name         = p.first;
                    const auto& objects_name = p.second;

                    if (gui::BeginMenu(name.c_str()))
                    {
                        for (const auto& name : objects_name)
                        {
                            if (gui::MenuItem(name.c_str()))
                            {
                                const auto id           = "embedded:/" + string_utils::to_lower(name);
                                auto       asset_future = am.load<mesh>(id);
                                model      model;
                                model.set_lod(asset_future.get(), 0);

                                auto object = ecs.create();
                                object.set_name(name);
                                auto transf_comp = object.assign<transform_component>().lock();
                                transf_comp->set_local_position({0.0f, 0.5f, 0.0f});

                                auto model_comp = object.assign<model_component>().lock();
                                model_comp->set_casts_shadow(true);
                                model_comp->set_casts_reflection(false);
                                model_comp->set_model(model);
                                es.select(object);
                            }
                        }
                        gui::EndMenu();
                    }
                }
                gui::EndMenu();
            }

            if (gui::BeginMenu("LIGHTING"))
            {
                if (gui::BeginMenu("LIGHT"))
                {
                    static const std::vector<std::pair<std::string, light_type>> light_objects = {
                        {"DIRECTIONAL", light_type::directional}, {"SPOT", light_type::spot}, {"POINT", light_type::point}};

                    for (const auto& p : light_objects)
                    {
                        const auto& name = p.first;
                        const auto& type = p.second;
                        if (gui::MenuItem(name.c_str()))
                        {
                            auto object = ecs.create();
                            object.set_name(name + " LIGHT");

                            auto transf_comp = object.assign<transform_component>().lock();
                            transf_comp->set_local_position({0.0f, 1.0f, 0.0f});
                            transf_comp->rotate_local(50.0f, -30.0f, 0.0f);

                            light light_data;
                            light_data.color = math::color(255, 244, 214, 255);
                            light_data.type  = type;

                            auto light_comp = object.assign<light_component>().lock();
                            light_comp->set_light(light_data);
                            es.select(object);
                        }
                    }
                    gui::EndMenu();
                }

                if (gui::BeginMenu("REFLECTION PROBES"))
                {
                    static const std::vector<std::pair<std::string, probe_type>> reflection_probes = {{"SPHERE", probe_type::sphere},
                                                                                                      {"BOX", probe_type::box}};
                    for (const auto& p : reflection_probes)
                    {
                        const auto& name = p.first + " PROBE";
                        const auto& type = p.second;

                        if (gui::MenuItem(name.c_str()))
                        {
                            auto object = ecs.create();
                            object.set_name(name);
                            auto transf_comp = object.assign<transform_component>().lock();
                            transf_comp->set_local_position({0.0f, 0.1f, 0.0f});

                            reflection_probe probe;
                            probe.method         = reflect_method::static_only;
                            probe.type           = type;
                            auto reflection_comp = object.assign<reflection_probe_component>().lock();
                            reflection_comp->set_probe(probe);
                            es.select(object);
                        }
                    }
                    gui::EndMenu();
                }
                gui::EndMenu();
            }

            if (gui::BeginMenu("AUDIO"))
            {
                if (gui::MenuItem("SOURCE"))
                {
                    auto object = ecs.create();
                    object.set_name("AUDIO SOURCE");
                    object.assign<transform_component>();
                    object.assign<audio_source_component>();
                    es.select(object);
                }
                gui::EndMenu();
            }

            if (gui::MenuItem("CAMERA"))
            {
                auto object = ecs.create();
                object.set_name("CAMERA");
                auto transf_comp = object.assign<transform_component>().lock();
                transf_comp->set_local_position({0.0f, 2.0f, -5.0f});

                object.assign<camera_component>();
                es.select(object);
            }
            gui::EndPopup();
        }
    }
    return action;
}

static bool process_drag_drop_source(runtime::entity entity)
{
    if (entity && gui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
    {
        auto entity_index = entity.id().index();
        gui::TextUnformatted(entity.to_string().c_str());
        gui::SetDragDropPayload("entity", &entity_index, sizeof(entity_index));
        gui::EndDragDropSource();
        return true;
    }

    return false;
}

static void process_drag_drop_target(runtime::entity entity)
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

        {
            auto payload = gui::AcceptDragDropPayload("entity");
            if (payload != nullptr)
            {
                std::uint32_t entity_index = 0;
                std::memcpy(&entity_index, payload->Data, std::size_t(payload->DataSize));
                if (ecs.valid_index(entity_index))
                {
                    auto eid            = ecs.create_id(entity_index);
                    auto dropped_entity = ecs.get(eid);
                    if (dropped_entity)
                    {
                        auto trans_comp = dropped_entity.get_component<transform_component>().lock();
                        if (trans_comp)
                        {
                            trans_comp->set_parent(entity);
                        }
                    }
                }
            }
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
                        trans_comp->set_parent(entity);
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
                    object.assign<transform_component>().lock()->set_parent(entity);
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

static void check_drag(runtime::entity entity)
{
    auto& es            = core::get_subsystem<editor::editing_system>();
    auto& editor_camera = es.camera;

    if (entity != editor_camera)
    {
        if (!process_drag_drop_source(entity))
        {
            process_drag_drop_target(entity);
        }
    }
}

void hierarchy_dock::draw_entity(runtime::entity entity)
{
    if (!entity)
    {
        return;
    }

    gui::PushID(static_cast<int>(entity.id().index()));
    gui::PushID(static_cast<int>(entity.id().version()));

    gui::AlignTextToFramePadding();
    auto& es          = core::get_subsystem<editor::editing_system>();
    auto& input       = core::get_subsystem<runtime::input>();
    auto& selected    = es.selection_data.object;
    bool  is_selected = false;
    if (selected && selected.is_type<runtime::entity>())
    {
        is_selected = selected.get_value<runtime::entity>() == entity;
    }

    std::string        name  = entity.to_string();
    ImGuiTreeNodeFlags flags = 0 | ImGuiTreeNodeFlags_AllowOverlap | ImGuiTreeNodeFlags_OpenOnArrow;

    if (is_selected)
    {
        flags |= ImGuiTreeNodeFlags_Selected;
    }

    if (gui::IsWindowFocused())
    {
        if (is_selected && !gui::IsAnyItemActive())
        {
            if (input.is_key_pressed(SDLK_F2))
            {
                edit_label_ = true;
                gui::SetKeyboardFocusHere();
            }
        }
    }
    auto trans_comp  = entity.get_component<transform_component>().lock();
    bool no_children = true;
    if (trans_comp)
    {
        no_children = trans_comp->get_children().empty();
    }

    if (no_children)
    {
        flags |= ImGuiTreeNodeFlags_Leaf;
    }

    auto pos = gui::GetCursorScreenPos();
    gui::AlignTextToFramePadding();

    bool opened = gui::TreeNodeEx(name.c_str(), flags);
    if (!edit_label_)
    {
        check_drag(entity);
    }
    if (edit_label_ && is_selected)
    {
        std::array<char, 64> input_buff;
        input_buff.fill(0);
        std::memcpy(input_buff.data(), name.c_str(), name.size() < input_buff.size() ? name.size() : input_buff.size());

        gui::SetCursorScreenPos(pos);
        gui::PushItemWidth(gui::GetContentRegionAvail().x);

        gui::PushID(static_cast<int>(entity.id().index()));
        gui::PushID(static_cast<int>(entity.id().version()));
        if (gui::InputText("", input_buff.data(), input_buff.size(), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
        {
            entity.set_name(input_buff.data());
            edit_label_ = false;
        }

        gui::PopItemWidth();

        if (!gui::IsItemActive() && (gui::IsMouseClicked(0) || gui::IsMouseDragging(0)))
        {
            edit_label_ = false;
        }
        gui::PopID();
        gui::PopID();
    }

    ImGuiWindow* window = gui::GetCurrentWindow();

    if (gui::IsItemHovered() && !gui::IsMouseDragging(0))
    {
        if (gui::IsMouseClicked(0))
        {
            id_ = window->GetID(trans_comp.get());
        }

        if (gui::IsMouseReleased(0) && window->GetID(trans_comp.get()) == id_)
        {
            if (!is_selected)
            {
                edit_label_ = false;
            }
            es.select(entity);
        }

        if (gui::IsMouseDoubleClicked(0))
        {
            edit_label_ = is_selected;
        }
    }

    if (!edit_label_)
    {
        auto action = check_context_menu(entity);
        if (action == context_action::rename)
        {
            edit_label_ = true;
            es.select(entity);
        }
    }

    if (opened)
    {
        if (!no_children)
        {
            const auto& children = trans_comp->get_children();
            for (auto& child : children)
            {
                if (child.valid())
                {
                    draw_entity(child);
                }
            }
        }

        gui::TreePop();
    }

    gui::PopID();
    gui::PopID();
}

void hierarchy_dock::render()
{
    auto& es    = core::get_subsystem<editor::editing_system>();
    auto& sg    = core::get_subsystem<runtime::scene_graph>();
    auto& input = core::get_subsystem<runtime::input>();

    auto& roots         = sg.get_roots();
    auto& editor_camera = es.camera;
    auto& selected      = es.selection_data.object;

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings;

    if (gui::BeginChild("hierarchy_content", gui::GetContentRegionAvail(), ImGuiChildFlags_Border, flags))
    {

        check_context_menu(runtime::entity());

        if (gui::IsWindowFocused())
        {
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

            if (input.is_key_pressed(SDLK_d, SDLK_LCTRL))
            {
                if (selected && selected.is_type<runtime::entity>())
                {
                    auto sel = selected.get_value<runtime::entity>();
                    if (sel && sel != editor_camera)
                    {
                        auto clone            = ecs::utils::clone_entity(sel);
                        auto clone_trans_comp = clone.get_component<transform_component>().lock();
                        auto sel_trans_comp   = sel.get_component<transform_component>().lock();
                        if (clone_trans_comp && sel_trans_comp)
                        {
                            clone_trans_comp->set_parent(sel_trans_comp->get_parent(), false, true);
                        }
                        es.select(clone);
                    }
                }
            }
        }
        if (input.is_key_pressed(SDLK_f, SDLK_LSHIFT))
        {
            if (selected && selected.is_type<runtime::entity>())
            {
                auto sel = selected.get_value<runtime::entity>();
                if (sel && sel != editor_camera)
                {
                    if (editor_camera.has_component<transform_component>() && editor_camera.has_component<camera_component>())
                    {
                        auto bounds = calc_bounds(sel);
                        focus_entity_on_bounds(editor_camera, bounds);
                    }
                }
            }
        }

        if (editor_camera.valid())
        {
            draw_entity(editor_camera);
            gui::Separator();
        }

        for (auto& root : roots)
        {
            if (root.valid())
            {
                if (root != editor_camera)
                {
                    draw_entity(root);
                }
            }
        }
    }
    gui::EndChild();
    process_drag_drop_target({});
}

hierarchy_dock::hierarchy_dock(const std::string& dtitle) {}
