#include "editor/include/editor_ui.h"

#include "editor/include/IconsFontAwesome6.h"
#include "editor/include/editor_global_context.h"
#include "editor/include/editor_input_manager.h"
#include "editor/include/editor_scene_manager.h"

#include "runtime/core/base/macro.h"
#include "runtime/core/meta/reflection/reflection.h"

#include "runtime/platform/path/path.h"

#include "runtime/resource/asset_manager/asset_manager.h"
#include "runtime/resource/config_manager/config_manager.h"

#include "runtime/engine.h"

#include "runtime/function/framework/level/level.h"
#include "runtime/function/framework/world/world_manager.h"
#include "runtime/function/input/input_system.h"
#include "runtime/function/render/render_camera.h"
#include "runtime/function/render/render_debug_config.h"
#include "runtime/function/render/render_system.h"
#include "runtime/function/render/window_system.h"

#include <imgui.h>
#include <imgui_internal.h>

#include "editor/include/editor_asset_browser.h"
#include "editor/include/editor_detail_window.h"
#include "editor/include/editor_object_window.h"
#include "editor/include/editor_scene_view.h"
#include "function/ui/Settings/PanelWindowSettings.h"

namespace LunarYue
{
    // エディタのノードの状態を保持するための配列
    std::vector<std::pair<std::string, bool>> g_editor_node_state_array;

    // 現在のノードの深さ
    int g_node_depth = -1;

    void DrawVecControl(const std::string& label, LunarYue::Vector3& values, float resetValue = 0.0f, float columnWidth = 100.0f);

    void DrawVecControl(const std::string& label, LunarYue::Quaternion& values, float resetValue = 0.0f, float columnWidth = 100.0f);

    // コンストラクタ
    EditorUI::EditorUI()
    {
        // アセットフォルダのパスを取得する
        const auto& asset_folder = g_runtime_global_context.m_config_manager->getAssetFolder();

        // 各種UIコントロールを作成するためのラムダ式を設定する

        m_editor_ui_creator["TreeNodePush"] = [this](const std::string& name, void* value_ptr) -> void {
            // テーブルフラグを設定する
            static ImGuiTableFlags flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings;
            // ノードの状態を保持するフラグ
            bool node_state = false;
            // ノードの深さを増やす
            g_node_depth++;
            if (g_node_depth > 0)
            {
                // 親ノードが開いている場合は、ノードを展開する
                if (g_editor_node_state_array[g_node_depth - 1].second)
                {
                    node_state = ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_SpanFullWidth);
                }
                // 親ノードが閉じている場合は、ノードの状態を保持する
                else
                {
                    g_editor_node_state_array.emplace_back(std::pair(name.c_str(), node_state));
                    return;
                }
            }
            // ノードの深さが0の場合は、ノードを展開する
            else
            {
                node_state = ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_SpanFullWidth);
            }
            // ノードの状態を保持する
            g_editor_node_state_array.emplace_back(std::pair(name.c_str(), node_state));
        };

        m_editor_ui_creator["TreeNodePop"] = [this](const std::string& name, void* value_ptr) -> void {
            // ノードが展開されている場合は、ノードを閉じる
            if (g_editor_node_state_array[g_node_depth].second)
            {
                ImGui::TreePop();
            }
            // ノードの状態を保持する配列からノードを削除する
            g_editor_node_state_array.pop_back();
            // ノードの深さを減らす
            g_node_depth--;
        };

        // TransformのUIを描画する
        m_editor_ui_creator["Transform"] = [this](const std::string& name, void* value_ptr) -> void {
            if (g_editor_node_state_array[g_node_depth].second) // ノードが開かれている場合のみ描画
            {
                auto trans_ptr = static_cast<Transform*>(value_ptr);

                // 角度を度数法で取得
                Vector3 degrees_val = trans_ptr->getRotationDegrees();

                // 位置、回転、スケールのUIを描画
                DrawVecControl("Position", trans_ptr->m_position);
                DrawVecControl("Rotation", degrees_val);
                DrawVecControl("Scale", trans_ptr->m_scale);

                // Quaternionに角度を反映
                trans_ptr->degreesToQuaternion(degrees_val);

                // 選択されたエンティティの軸を描画
                g_editor_global_context.m_scene_manager->drawSelectedEntityAxis();
            }
        };

        m_editor_ui_creator["bool"] = [this](const std::string& name, void* value_ptr) -> void {
            // ノードがルート階層の場合は、ラベルとチェックボックスを表示
            if (g_node_depth == -1)
            {
                std::string label = "##" + name;
                ImGui::Text("%s", name.c_str());
                ImGui::SameLine();
                ImGui::Checkbox(label.c_str(), static_cast<bool*>(value_ptr));
            }
            else
            {
                // ノードが展開されている場合は、ラベルとチェックボックスを表示
                if (g_editor_node_state_array[g_node_depth].second)
                {
                    std::string full_label = "##" + getLeafUINodeParentLabel() + name;
                    ImGui::Text("%s", name.c_str());
                    ImGui::Checkbox(full_label.c_str(), static_cast<bool*>(value_ptr));
                }
            }
        };

        m_editor_ui_creator["int"] = [this](const std::string& name, void* value_ptr) -> void {
            // ノードがルート階層の場合は、ラベルと数値入力欄を表示
            if (g_node_depth == -1)
            {
                std::string label = "##" + name;
                ImGui::Text("%s", name.c_str());
                ImGui::SameLine();
                ImGui::InputInt(label.c_str(), static_cast<int*>(value_ptr));
            }
            else
            {
                // ノードが展開されている場合は、ラベルと数値入力欄を表示
                if (g_editor_node_state_array[g_node_depth].second)
                {
                    std::string full_label = "##" + getLeafUINodeParentLabel() + name;
                    ImGui::Text("%s", (name + ":").c_str());
                    ImGui::InputInt(full_label.c_str(), static_cast<int*>(value_ptr));
                }
            }
        };

        m_editor_ui_creator["float"] = [this](const std::string& name, void* value_ptr) -> void {
            // ノードがルート階層の場合は、ラベルと数値入力欄を表示
            if (g_node_depth == -1)
            {
                std::string label = "##" + name;
                ImGui::Text("%s", name.c_str());
                ImGui::SameLine();
                ImGui::InputFloat(label.c_str(), static_cast<float*>(value_ptr));
            }
            else
            {
                // ノードが展開されている場合は、ラベルと数値入力欄を表示
                if (g_editor_node_state_array[g_node_depth].second)
                {
                    std::string full_label = "##" + getLeafUINodeParentLabel() + name;
                    ImGui::Text("%s", (name + ":").c_str());
                    ImGui::InputFloat(full_label.c_str(), static_cast<float*>(value_ptr));
                }
            }
        };

        // Vector3型のUIを作成するラムダ式
        m_editor_ui_creator["Vector3"] = [this](const std::string& name, void* value_ptr) -> void {
            auto  vec_ptr = static_cast<Vector3*>(value_ptr);
            float val[3]  = {vec_ptr->x, vec_ptr->y, vec_ptr->z};
            if (g_node_depth == -1)
            {
                // ルートノードの場合はラベルを表示してDragFloat3を使用
                std::string label = "##" + name;
                ImGui::Text("%s", name.c_str());
                ImGui::SameLine();
                ImGui::DragFloat3(label.c_str(), val);
            }
            else
            {
                // リーフノードの場合は親要素とのラベルを合成した文字列を使用
                if (g_editor_node_state_array[g_node_depth].second)
                {
                    std::string full_label = "##" + getLeafUINodeParentLabel() + name;
                    ImGui::Text("%s", (name + ":").c_str());
                    ImGui::DragFloat3(full_label.c_str(), val);
                }
            }
            vec_ptr->x = val[0];
            vec_ptr->y = val[1];
            vec_ptr->z = val[2];
        };

        // Quaternion型のUIを作成するラムダ式
        m_editor_ui_creator["Quaternion"] = [this](const std::string& name, void* value_ptr) -> void {
            auto  qua_ptr = static_cast<Quaternion*>(value_ptr);
            float val[4]  = {qua_ptr->x, qua_ptr->y, qua_ptr->z, qua_ptr->w};
            if (g_node_depth == -1)
            {
                // ルートノードの場合はラベルを表示してDragFloat4を使用
                std::string label = "##" + name;
                ImGui::Text("%s", name.c_str());
                ImGui::SameLine();
                ImGui::DragFloat4(label.c_str(), val);
            }
            else
            {
                // リーフノードの場合は親要素とのラベルを合成した文字列を使用
                if (g_editor_node_state_array[g_node_depth].second)
                {
                    std::string full_label = "##" + getLeafUINodeParentLabel() + name;
                    ImGui::Text("%s", (name + ":").c_str());
                    ImGui::DragFloat4(full_label.c_str(), val);
                }
            }
            qua_ptr->x = val[0];
            qua_ptr->y = val[1];
            qua_ptr->z = val[2];
            qua_ptr->w = val[3];
        };

        // std::string型のUIを作成するラムダ式を登録する
        m_editor_ui_creator["std::string"] = [this, &asset_folder](const std::string& name, void* value_ptr) -> void {
            // ノードがルートノードの場合
            if (g_node_depth == -1)
            {
                // ラベルを作成してUIに表示する
                std::string label = "##" + name;
                ImGui::Text("%s", name.c_str());
                ImGui::SameLine();
                ImGui::Text("%s", (*static_cast<std::string*>(value_ptr)).c_str());
            }
            // ノードがルートノードではなく、かつ編集可能な状態の場合
            else
            {
                if (g_editor_node_state_array[g_node_depth].second)
                {
                    // 親のラベルを取得してフルラベルを作成する
                    std::string full_label = "##" + getLeafUINodeParentLabel() + name;
                    // ラベル名とコロンを表示する
                    ImGui::Text("%s", (name + ":").c_str());
                    // std::string型の値を文字列に変換して表示する
                    std::string value_str = *static_cast<std::string*>(value_ptr);
                    // 値がファイルパスの場合は相対パスに変換する
                    if (value_str.find_first_of('/') != std::string::npos)
                    {
                        std::filesystem::path value_path(value_str);
                        if (value_path.is_absolute())
                        {
                            value_path = Path::getRelativePath(asset_folder, value_path);
                        }
                        value_str = value_path.generic_string();
                        // 相対パスが'..'で始まる場合は空文字列にする
                        if (value_str.size() >= 2 && value_str[0] == '.' && value_str[1] == '.')
                        {
                            value_str.clear();
                        }
                    }
                    ImGui::Text("%s", value_str.c_str());
                }
            }
        };
    }

    std::string EditorUI::getLeafUINodeParentLabel()
    {
        std::string parent_label;                                  // 親ラベルを格納するための変数
        int         array_size = g_editor_node_state_array.size(); // 配列のサイズを取得
        for (int index = 0; index < array_size; index++)           // 配列のサイズ分繰り返し
        {
            // 各インデックスのエディタノード状態を取得し、親ラベルに追加
            parent_label += g_editor_node_state_array[index].first + "::";
        }
        return parent_label; // 親ラベルを返す
    }

    void EditorUI::showEditorUI()
    {
        showEditorMenu(&m_editor_menu_window_open);
        showEditorWorldObjectsWindow(&m_asset_window_open);
        showEditorGameWindow(&m_game_engine_window_open);
        showEditorFileContentWindow(&m_file_content_window_open);
        showEditorDetailWindow(&m_detail_window_open);
        if (m_imgui_demo_window_open)
        {
            ImGui::ShowDemoWindow(&m_imgui_demo_window_open);
        }
    }

    void EditorUI::showEditorMenu(bool* p_open)
    {
        ImGuiDockNodeFlags dock_flags = ImGuiDockNodeFlags_DockSpace;

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                                        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground |
                                        ImGuiConfigFlags_NoMouseCursorChange | ImGuiWindowFlags_NoBringToFrontOnFocus;

        const ImGuiViewport* main_viewport = ImGui::GetMainViewport();                                    // メインビューポートの取得
        ImGui::SetNextWindowPos(main_viewport->WorkPos, ImGuiCond_Always);                                // ウィンドウの位置を設定
        std::array<int, 2> window_size = g_runtime_global_context.m_window_system->getWindowSize();       // ウィンドウサイズの取得
        ImGui::SetNextWindowSize(ImVec2((float)window_size[0], (float)window_size[1]), ImGuiCond_Always); // ウィンドウサイズを設定

        ImGui::SetNextWindowViewport(main_viewport->ID); // メインビューポートを設定

        ImGui::Begin("Editor menu", p_open, window_flags); // エディタメニューウィンドウを開始

        ImGuiID main_docking_id = ImGui::GetID("Main Docking");
        if (ImGui::DockBuilderGetNode(main_docking_id) == nullptr)
        {
            ImGui::DockBuilderRemoveNode(main_docking_id);

            ImGui::DockBuilderAddNode(main_docking_id, dock_flags);
            ImGui::DockBuilderSetNodePos(main_docking_id, ImVec2(main_viewport->WorkPos.x, main_viewport->WorkPos.y + 18.0f));
            ImGui::DockBuilderSetNodeSize(main_docking_id, ImVec2((float)window_size[0], (float)window_size[1] - 18.0f));

            ImGuiID center = main_docking_id;
            ImGuiID left;
            ImGuiID right = ImGui::DockBuilderSplitNode(center, ImGuiDir_Right, 0.30f, nullptr, &left);

            ImGuiID left_other;
            ImGuiID left_file_content = ImGui::DockBuilderSplitNode(left, ImGuiDir_Down, 0.30f, nullptr, &left_other);

            ImGuiID left_game_engine;
            ImGuiID left_asset = ImGui::DockBuilderSplitNode(left_other, ImGuiDir_Left, 0.25f, nullptr, &left_game_engine);

            ImGui::DockBuilderDockWindow("World Objects", left_asset);
            ImGui::DockBuilderDockWindow("Components Details", right);
            ImGui::DockBuilderDockWindow("File Content", left_file_content);
            ImGui::DockBuilderDockWindow("Game Engine", left_game_engine);

            ImGui::DockBuilderFinish(main_docking_id);
        }

        ImGui::DockSpace(main_docking_id);

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Menu"))
            {
                if (ImGui::MenuItem("Reload Current Level"))
                {
                    g_runtime_global_context.m_world_manager->reloadCurrentLevel();
                }

                if (ImGui::MenuItem("Save Current Level"))
                {
                    g_runtime_global_context.m_world_manager->saveCurrentLevel();
                }

                // デバッグメニュー
                if (ImGui::BeginMenu("Debug"))
                {
                    // サブメニューを作成
                    auto createSubMenu = [&](const std::string& title, std::initializer_list<std::pair<std::string, bool*>> options) {
                        if (ImGui::BeginMenu(title.c_str()))
                        {
                            for (const auto& option : options)
                            {
                                ImGui::Checkbox(option.first.c_str(), option.second);
                            }
                            ImGui::EndMenu();
                        }
                    };

                    createSubMenu("Animation",
                                  {{"Show Skeleton", &g_runtime_global_context.m_render_debug_config->animation.show_skeleton},
                                   {"Show Bone Name", &g_runtime_global_context.m_render_debug_config->animation.show_bone_name}});

                    createSubMenu("Camera", {{"Show Runtime Info", &g_runtime_global_context.m_render_debug_config->camera.show_runtime_info}});

                    createSubMenu("Game Object",
                                  {{"Show Bounding Box", &g_runtime_global_context.m_render_debug_config->gameObject.show_bounding_box}});

                    ImGui::EndMenu();
                }

                // エンジンを終了
                if (ImGui::MenuItem("Exit"))
                {
                    g_editor_global_context.m_engine_runtime->shutdownEngine();
                    g_runtime_global_context.m_window_system->setShouldClose(true);
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Window"))
            {
                ImGui::MenuItem("World Objects", nullptr, &m_asset_window_open);
                ImGui::MenuItem("Game", nullptr, &m_game_engine_window_open);
                ImGui::MenuItem("File Content", nullptr, &m_file_content_window_open);
                ImGui::MenuItem("Detail", nullptr, &m_detail_window_open);
                ImGui::MenuItem("ImGui Demo", nullptr, &m_imgui_demo_window_open);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Create new level"))
            {
                if (ImGui::MenuItem("Create"))
                {
                    g_runtime_global_context.m_world_manager->createNewLevel("asset/level/test.json");
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Create new object"))
            {

                if (ImGui::MenuItem("Create"))
                {
                    // 現在のアクティブなレベルを取得する
                    std::shared_ptr<Level> level = g_runtime_global_context.m_world_manager->getCurrentActiveLevel().lock();
                    if (level == nullptr)
                        return;

                    // 新しいGameObjectを作成し、シーンに追加する
                    const size_t new_gobject_id = level->createEmptyObject("asset/objects", "test");
                    if (new_gobject_id != k_invalid_object_id)
                    {
                        // 新しいGameObjectが正常に作成された場合、それを選択する
                        g_editor_global_context.m_scene_manager->onGObjectSelected(new_gobject_id);
                    }
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("add component"))
            {
                // 選択されたオブジェクトを取得
                std::shared_ptr<Object> selected_object = g_editor_global_context.m_scene_manager->getSelectedGObject().lock();
                if (selected_object != nullptr)
                {
                    auto&& selected_object_components = selected_object->getComponents();

                    for (auto& component_ptr : selected_object_components)
                    {
                        auto object_instance = Reflection::ReflectionInstance(Reflection::TypeMeta::newMetaFromName(component_ptr.getTypeName()),
                                                                              component_ptr.getPtr());

                        // 基本クラスのリフレクションインスタンスリストを取得
                        Reflection::ReflectionInstance* reflection_instance;
                        const int count = object_instance.m_meta.getBaseClassReflectionInstanceList(reflection_instance, object_instance.m_instance);

                        // 基本クラスのリフレクションインスタンスを反復処理し、対応するUIを作成
                        for (int index = 0; index < count; index++)
                        {
                            createClassUI(reflection_instance[index]);
                        }

                        // リーフノードUIを作成
                        // createLeafNodeUI(instance);

                        // 反復処理中に割り当てたリフレクションインスタンスを削除（必要に応じて）
                        if (count > 0)
                            delete[] reflection_instance;
                    }
                }
                else
                {}

                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        ImGui::End();
    }

    void EditorUI::showEditorWorldObjectsWindow(bool* p_open)
    {
        // ウィンドウが開いていない場合、処理をスキップ
        if (!*p_open)
            return;

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;

        // "World Objects" ウィンドウの開始
        if (!ImGui::Begin("World Objects", p_open, window_flags))
        {
            ImGui::End();
            return;
        }

        // 現在のアクティブなレベルを取得
        const auto current_active_level = g_runtime_global_context.m_world_manager->getCurrentActiveLevel().lock();
        if (!current_active_level)
            return;

        // レベル内のすべてのゲームオブジェクトを取得
        const LevelObjectsMap& all_gobjects = current_active_level->getAllGObjects();

        // ゲームオブジェクトをループ処理
        for (const auto& id_object_pair : all_gobjects)
        {
            const ObjectID          object_id = id_object_pair.first;
            std::shared_ptr<Object> object    = id_object_pair.second;
            const std::string       name      = object->getName();

            // オブジェクト名が空でない場合、選択可能なアイテムを表示
            if (!name.empty())
            {
                bool is_selected = g_editor_global_context.m_scene_manager->getSelectedObjectID() == object_id;
                if (ImGui::Selectable(name.c_str(), is_selected))
                {
                    // 選択されたオブジェクトのIDを切り替え
                    ObjectID new_selected_id = is_selected ? k_invalid_object_id : object_id;
                    g_editor_global_context.m_scene_manager->onGObjectSelected(new_selected_id);
                    break;
                }
            }
        }
        ImGui::End(); // "World Objects" ウィンドウの終了
    }

    void EditorUI::createClassUI(Reflection::ReflectionInstance& instance)
    {
        // 基本クラスのリフレクションインスタンスリストを取得
        Reflection::ReflectionInstance* reflection_instance;
        int                             count = instance.m_meta.getBaseClassReflectionInstanceList(reflection_instance, instance.m_instance);

        // 基本クラスのリフレクションインスタンスを反復処理し、対応するUIを作成
        for (int index = 0; index < count; index++)
        {
            createClassUI(reflection_instance[index]);
        }

        // リーフノードUIを作成
        createLeafNodeUI(instance);

        // 反復処理中に割り当てたリフレクションインスタンスを削除（必要に応じて）
        if (count > 0)
            delete[] reflection_instance;
    }

    void EditorUI::createLeafNodeUI(Reflection::ReflectionInstance& instance)
    {
        // 反射インスタンスのフィールド情報を取得
        Reflection::FieldAccessor* fields;
        int                        fields_count = instance.m_meta.getFieldsList(fields);

        // 各フィールドを反復処理
        for (size_t index = 0; index < fields_count; index++)
        {
            auto field = fields[index];

            // フィールドが配列タイプの場合
            if (field.isArrayType())
            {

                Reflection::ArrayAccessor array_accessor;
                if (Reflection::TypeMeta::newArrayAccessorFromName(field.getFieldTypeName(), array_accessor))
                {
                    void* field_instance = field.get(instance.m_instance);
                    int   array_count    = array_accessor.getSize(field_instance);

                    // 配列のUIノードを作成
                    m_editor_ui_creator["TreeNodePush"](std::string(field.getFieldName()) + "[" + std::to_string(array_count) + "]", nullptr);

                    // 配列要素の型情報を取得し、対応するUIクリエータを検索
                    auto item_type_meta_item      = Reflection::TypeMeta::newMetaFromName(array_accessor.getElementTypeName());
                    auto item_ui_creator_iterator = m_editor_ui_creator.find(item_type_meta_item.getTypeName());

                    // 配列の各要素についてUIを作成
                    for (int val = 0; val < array_count; val++)
                    {

                        if (item_ui_creator_iterator == m_editor_ui_creator.end())
                        {

                            m_editor_ui_creator["TreeNodePush"]("[" + std::to_string(val) + "]", nullptr);

                            // 配列要素のインスタンスを作成し、対応するUIを生成
                            auto object_instance =
                                Reflection::ReflectionInstance(LunarYue::Reflection::TypeMeta::newMetaFromName(item_type_meta_item.getTypeName()),
                                                               array_accessor.get(val, field_instance));
                            createClassUI(object_instance);

                            m_editor_ui_creator["TreeNodePop"]("[" + std::to_string(val) + "]", nullptr);
                        }
                        else
                        {
                            // 既存のUIクリエータがある場合、それを使用してUIを作成
                            m_editor_ui_creator[item_type_meta_item.getTypeName()]("[" + std::to_string(val) + "]",
                                                                                   array_accessor.get(val, field_instance));
                        }
                    }

                    // 配列のUIノードを閉じる
                    m_editor_ui_creator["TreeNodePop"](field.getFieldName(), nullptr);
                }
            }
            // フィールドの型に対応するUIクリエータを検索
            auto ui_creator_iterator = m_editor_ui_creator.find(field.getFieldTypeName());

            // 既存のUIクリエータがない場合、
            if (ui_creator_iterator == m_editor_ui_creator.end())
            {

                // フィールドの型情報を取得し、型に対応するUIを作成
                Reflection::TypeMeta field_meta = Reflection::TypeMeta::newMetaFromName(field.getFieldTypeName());
                if (field.getTypeMeta(field_meta))
                {
                    // 子クラスのインスタンスを作成し、対応するUIを生成
                    auto child_instance = Reflection::ReflectionInstance(field_meta, field.get(instance.m_instance));

                    // 子クラスのUIノードを開く
                    m_editor_ui_creator["TreeNodePush"](field_meta.getTypeName(), nullptr);

                    createClassUI(child_instance);

                    // 子クラスのUIノードを閉じる
                    m_editor_ui_creator["TreeNodePop"](field_meta.getTypeName(), nullptr);
                }
                else
                {
                    // 既存のUIクリエータがない場合、処理をスキップ
                    if (ui_creator_iterator == m_editor_ui_creator.end())
                    {
                        continue;
                    }
                    // 既存のUIクリエータがある場合、それを使用してUIを作成
                    m_editor_ui_creator[field.getFieldTypeName()](field.getFieldName(), field.get(instance.m_instance));
                }
            }
            else
            {
                // 既存のUIクリエータがある場合、それを使用してUIを作成
                m_editor_ui_creator[field.getFieldTypeName()](field.getFieldName(), field.get(instance.m_instance));
            }
        }

        // 反復処理が終了したら、フィールドを削除
        delete[] fields;
    }

    void EditorUI::showEditorDetailWindow(bool* p_open)
    {
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;

        if (!*p_open)
            return;

        if (!ImGui::Begin("Components Details", p_open, window_flags))
        {
            ImGui::End();
            return;
        }

        // 選択されたオブジェクトを取得
        std::shared_ptr<Object> selected_object = g_editor_global_context.m_scene_manager->getSelectedGObject().lock();
        if (selected_object == nullptr)
        {
            ImGui::End();
            return;
        }

        // オブジェクト名を表示
        const std::string& name       = selected_object->getName();
        static char        cname[128] = {};
        memcpy(cname, name.c_str(), name.size());

        ImGui::Text("Name");
        ImGui::SameLine();
        ImGui::InputText("##Name", cname, IM_ARRAYSIZE(cname), ImGuiInputTextFlags_ReadOnly);

        // コンポーネントを一覧表示
        static ImGuiTableFlags flags                      = ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings;
        auto&&                 selected_object_components = selected_object->getComponents();
        for (auto& component_ptr : selected_object_components)
        {
            std::string componentNameTag = "<" + component_ptr.getTypeName() + ">";
            m_editor_ui_creator["TreeNodePush"](componentNameTag, nullptr);

            auto object_instance = Reflection::ReflectionInstance(LunarYue::Reflection::TypeMeta::newMetaFromName(component_ptr.getTypeName()),
                                                                  component_ptr.operator->());

            createClassUI(object_instance);
            m_editor_ui_creator["TreeNodePop"](componentNameTag, nullptr);
        }

        ImGui::End();
    }

    void EditorUI::setupUI()
    {
        UI::Settings::PanelWindowSettings settings;
        settings.closable    = true;
        settings.collapsable = true;
        settings.dockable    = true;

        g_editor_global_context.m_panels_manager->CreatePanel<MenuBar>("Menu Bar");
        g_editor_global_context.m_panels_manager->CreatePanel<SceneView>("Scene View", true, settings);
        g_editor_global_context.m_panels_manager->CreatePanel<AssetBrowser>("Asset Browser", true, settings);
        g_editor_global_context.m_panels_manager->CreatePanel<ObjectWindow>("Object Window", true, settings);
        g_editor_global_context.m_panels_manager->CreatePanel<DetailWindow>("Detail Window", true, settings);

        m_canvas.MakeDockspace(true);
        g_editor_global_context.m_ui_manager->SetCanvas(m_canvas);
    }

    void EditorUI::showEditorFileContentWindow(bool* p_open)
    {
        // ウィンドウフラグを定義する
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;

        if (!*p_open)
            return;

        if (!ImGui::Begin("File Content", p_open, window_flags))
        {
            ImGui::End();
            return;
        }

        // ファイルツリーを更新する
        auto current_time = std::chrono::steady_clock::now();
        if (current_time - m_last_file_tree_update > std::chrono::seconds(600))
        {
            m_editor_file_service.buildEngineFileTree();
            m_last_file_tree_update = current_time;
        }
        m_last_file_tree_update = current_time;

        ImGui::SameLine();
        if (ImGui::Button(ICON_FA_ROTATE))
        {
            m_editor_file_service.buildEngineFileTree();
            m_last_file_tree_update = current_time;
        }

        // ファイルツリーのルートノードを取得する
        EditorFileNode* editor_root_node = m_editor_file_service.getEditorRootNode();

        static float size_ratio   = 0.35f;
        float        window_width = ImGui::GetWindowContentRegionWidth();

        // フォルダの階層を表示する
        ImGui::BeginChild("FolderHierarchy", ImVec2(window_width * size_ratio, 0), true);
        static int current_folder = 0;
        current_folder            = 0;
        buildEditorFolderHierarchy(editor_root_node);
        ImGui::EndChild();

        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.8f, 0.8f, 0.4f));
        ImGui::Button("##vsplitter", ImVec2(8.0f, -1.0f));
        ImGui::PopStyleColor();
        if (ImGui::IsItemHovered())
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
        if (ImGui::IsItemActive())
            size_ratio += ImGui::GetIO().MouseDelta.x / window_width;

        // フォルダのコンテンツを表示する
        ImGui::SameLine();
        ImGui::BeginChild("FolderContent", ImVec2(0, 0), true);
        EditorFileNode* node = m_editor_file_service.getSelectedFolderNode();
        if (node == nullptr)
        {
            node = m_editor_file_service.getEditorRootNode();
        }
        buildEditorFileAssetsUIGrid(node);

        ImGui::EndChild();

        ImGui::End();
    }

    void EditorUI::buildEditorFileAssetsUIGrid(const EditorFileNode* node)
    {
        static float padding       = 45.0f;
        static float thumbnailSize = 90.0f;
        float        cellSize      = thumbnailSize + padding;

        if (ImGui::Button(ICON_FA_ARROW_LEFT))
        {
            if (node->m_parent_node)
                m_editor_file_service.setSelectedFolderNode(node->m_parent_node.get());
        }
        ImGui::SameLine();
        ImGui::Dummy(ImVec2(20.0f, 0.0f));
        ImGui::SameLine();
        if (ImGui::Button(ICON_FA_S))
        {
            thumbnailSize = 60.0f;
            padding       = 30.0f;
        }
        ImGui::SameLine();
        if (ImGui::Button(ICON_FA_M))
        {
            thumbnailSize = 90.0f;
            padding       = 45.0f;
        }
        ImGui::SameLine();
        if (ImGui::Button(ICON_FA_L))
        {
            thumbnailSize = 120.0f;
            padding       = 60.0f;
        }

        const float panelWidth  = ImGui::GetContentRegionAvail().x;
        int         columnCount = static_cast<int>(panelWidth / cellSize);
        if (columnCount < 1)
            columnCount = 1;

        ImGui::Columns(columnCount, nullptr, false);

        for (const auto& child_node : node->m_child_nodes)
        {
            ImGui::PushID(child_node->m_file_name.c_str());

            std::string icon;
            if (!child_node->m_child_nodes.empty())
            {
                icon = "folder";
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
            }
            else if (child_node->m_file_type == "object")
            {
                icon = "object";
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
            }
            else
            {
                icon = "question";
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
            }

            ImGui::ImageButton(g_runtime_global_context.m_render_system->getIconId(icon), ImVec2(thumbnailSize, thumbnailSize));

            if (ImGui::BeginDragDropSource())
            {
                ImGui::SetDragDropPayload("FILE_DRAG_DROP", &child_node, sizeof(child_node));
                ImGui::EndDragDropSource();
            }

            ImGui::PopStyleColor();
            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
            {
                if (!child_node->m_child_nodes.empty())
                {
                    m_editor_file_service.setSelectedFolderNode(child_node.get());
                }
                else
                {
                    onFileContentItemClicked(child_node.get());
                }
            }

            std::string name = child_node->m_file_name.substr(0, child_node->m_file_name.find_first_of('.'));
            ImGui::TextWrapped(name.c_str());

            ImGui::NextColumn();

            ImGui::PopID();
        }

        ImGui::Columns(1);
    }

    void EditorUI::buildEditorFolderHierarchy(EditorFileNode* node)
    {
        // フォルダかどうかを判断する
        const bool is_folder = (!node->m_child_nodes.empty());

        bool is_root_node = false;
        if (node->m_file_name == m_editor_file_service.getEditorRootNode()->m_file_name)
            is_root_node = true;

        if (is_folder)
        {
            // ツリービューでフォルダを表示するために、ImGuiTreeNodeFlagsを指定する
            ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
            if (node == m_editor_file_service.getSelectedFolderNode())
            {
                node_flags |= ImGuiTreeNodeFlags_Selected;
            }

            // アイコンとともにフォルダを表示する
            ImGui::PushID(node->m_file_name.c_str());

            if (is_root_node)
            {
                node_flags |= ImGuiTreeNodeFlags_DefaultOpen;
            }

            const bool open = ImGui::TreeNodeEx(node->m_file_name.c_str(),
                                                node_flags,
                                                "%s %s",
                                                node->m_folder_open ? ICON_FA_FOLDER_OPEN : ICON_FA_FOLDER_CLOSED,
                                                node->m_file_name.c_str());

            ImGui::PopID();

            // フォルダがクリックされたら、選択されたフォルダを更新する
            if (ImGui::IsItemClicked())
            {
                m_editor_file_service.setSelectedFolderNode(node);
            }

            if (open)
            {
                node->m_folder_open = true;

                // 子フォルダを表示する
                ImGui::Indent(10.0f);
                for (auto& m_child_node : node->m_child_nodes)
                {
                    buildEditorFolderHierarchy(m_child_node.get()); // 親ノードとして現在のノードを渡す
                }
                ImGui::Unindent(10.0f);

                ImGui::TreePop();
            }
            else
            {
                node->m_folder_open = false;
            }
        }
        // フォルダじゃない場合
        else
        {
            const char*        icon;
            ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf;

            // ファイルタイプが "object" でなければ処理を戻す
            if (node->m_file_type == "object")
            {
                icon = ICON_FA_FILE_CIRCLE_CHECK;
                node_flags |= ImGuiTreeNodeFlags_Bullet;
            }
            else
            {
                icon = ICON_FA_FILE_CIRCLE_QUESTION;
            }

            const std::string name = node->m_file_name.substr(0, node->m_file_name.find_last_of('.'));

            // アイコンとともにフォルダを表示する
            ImGui::PushID(name.c_str());
            ImGui::TreeNodeEx(name.c_str(), node_flags, "%s %s", icon, name.c_str());
            ImGui::TreePop();
            ImGui::PopID();

            if (ImGui::IsItemClicked())
            {
                onFileContentItemClicked(node);
            }
        }
    }

    void EditorUI::onFileContentItemClicked(EditorFileNode* node)
    {
        // ファイルタイプが "object" でなければ処理を戻す
        if (node->m_file_type != "object")
            return;

        // 現在のアクティブなレベルを取得する
        std::shared_ptr<Level> level = g_runtime_global_context.m_world_manager->getCurrentActiveLevel().lock();
        if (level == nullptr)
            return;

        // 新しいオブジェクトのインデックスをインクリメントする
        const unsigned int new_object_index = ++m_new_object_index_map[node->m_file_name];

        // 新しいオブジェクトインスタンスリソースを作成する
        ObjectInstanceRes new_object_instance_res;
        new_object_instance_res.m_name       = "New_" + Path::getFilePureName(node->m_file_name) + "_" + std::to_string(new_object_index);
        new_object_instance_res.m_definition = g_runtime_global_context.m_asset_manager->getFullPath(node->m_file_path).generic_string();

        // 新しいGameObjectを作成し、シーンに追加する
        size_t new_gobject_id = level->createObject(new_object_instance_res);
        if (new_gobject_id != k_invalid_object_id)
        {
            // 新しいGameObjectが正常に作成された場合、それを選択する
            g_editor_global_context.m_scene_manager->onGObjectSelected(new_gobject_id);
        }
    }

    void EditorUI::showEditorGameWindow(bool* p_open)
    {
        ImGuiIO&         io           = ImGui::GetIO();
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_MenuBar;

        const ImGuiViewport* main_viewport = ImGui::GetMainViewport();

        if (!*p_open)
            return;

        if (!ImGui::Begin("Game Engine", p_open, window_flags))
        {
            ImGui::End();
            return;
        }

        static bool trans_button_ckecked  = false;
        static bool rotate_button_ckecked = false;
        static bool scale_button_ckecked  = false;

        switch (g_editor_global_context.m_scene_manager->getEditorAxisMode())
        {
            case EditorAxisMode::TranslateMode:
                trans_button_ckecked  = true;
                rotate_button_ckecked = false;
                scale_button_ckecked  = false;
                break;
            case EditorAxisMode::RotateMode:
                trans_button_ckecked  = false;
                rotate_button_ckecked = true;
                scale_button_ckecked  = false;
                break;
            case EditorAxisMode::ScaleMode:
                trans_button_ckecked  = false;
                rotate_button_ckecked = false;
                scale_button_ckecked  = true;
                break;
            default:
                break;
        }

        if (ImGui::BeginMenuBar())
        {
            ImGui::Indent(10.f);
            drawAxisToggleButton("Trans", trans_button_ckecked, (int)EditorAxisMode::TranslateMode);
            ImGui::Unindent();

            ImGui::SameLine();

            drawAxisToggleButton("Rotate", rotate_button_ckecked, (int)EditorAxisMode::RotateMode);

            ImGui::SameLine();

            drawAxisToggleButton("Scale", scale_button_ckecked, (int)EditorAxisMode::ScaleMode);

            ImGui::SameLine();

            float indent_val = 0.0f;

#if defined(__GNUC__) && defined(__MACH__)
            float indent_scale = 1.0f;
#else // Not tested on Linux
            float x_scale, y_scale;
            glfwGetWindowContentScale(g_runtime_global_context.m_window_system->getWindow(), &x_scale, &y_scale);
            float indent_scale = fmaxf(1.0f, fmaxf(x_scale, y_scale));
#endif
            indent_val = g_editor_global_context.m_input_manager->getEngineWindowSize().x - 100.0f * indent_scale;

            ImGui::Indent(indent_val);
            if (g_is_editor_mode)
            {
                ImGui::PushID("Editor Mode");
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
                if (ImGui::Button(ICON_FA_PLAY))
                {
                    g_is_editor_mode = false;
                    g_editor_global_context.m_scene_manager->drawSelectedEntityAxis();
                    g_editor_global_context.m_input_manager->resetEditorCommand();
                    g_runtime_global_context.m_window_system->setFocusMode(true);
                }
                ImGui::PopStyleColor();
                ImGui::PopID();
            }
            else
            {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
                if (ImGui::Button(ICON_FA_PAUSE))
                {
                    g_is_editor_mode = true;
                    g_editor_global_context.m_scene_manager->drawSelectedEntityAxis();
                    g_runtime_global_context.m_input_system->resetGameCommand();
                    g_runtime_global_context.m_render_system->getRenderCamera()->setMainViewMatrix(
                        g_editor_global_context.m_scene_manager->getEditorCamera()->getViewMatrix());
                }
                ImGui::PopStyleColor();
            }

            ImGui::Unindent();
            ImGui::EndMenuBar();
        }

        if (!g_is_editor_mode)
        {
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Press Left Alt key to display the mouse cursor!");
        }
        else
        {
            ImGui::TextColored(
                ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Current editor camera move speed: [%f]", g_editor_global_context.m_input_manager->getCameraSpeed());
        }

        // GetWindowPos() ----->  X--------------------------------------------O
        //                        |                                            |
        //                        |                                            |
        // menu_bar_rect.Min -->  X--------------------------------------------O
        //                        |    It is the menu bar window.              |
        //                        |                                            |
        //                        O--------------------------------------------X  <-- menu_bar_rect.Max
        //                        |                                            |
        //                        |     It is the render target window.        |
        //                        |                                            |
        //                        O--------------------------------------------O

        Vector2 render_target_window_pos  = {0.0f, 0.0f};
        Vector2 render_target_window_size = {0.0f, 0.0f};

        auto menu_bar_rect = ImGui::GetCurrentWindow()->MenuBarRect();

        render_target_window_pos.x  = ImGui::GetWindowPos().x;
        render_target_window_pos.y  = menu_bar_rect.Max.y;
        render_target_window_size.x = ImGui::GetWindowSize().x;
        render_target_window_size.y = (ImGui::GetWindowSize().y + ImGui::GetWindowPos().y) - menu_bar_rect.Max.y;
        // coord of right bottom point of full window minus coord of right bottom point of menu bar window.

        // if (new_window_pos != m_engine_window_pos || new_window_size != m_engine_window_size)
        {
#if defined(__MACH__)
            // The dpi_scale is not reactive to DPI changes or monitor switching, it might be a bug from ImGui.
            // Return value from ImGui::GetMainViewport()->DpiScal is always the same as first frame.
            // glfwGetMonitorContentScale and glfwSetWindowContentScaleCallback are more adaptive.
            float dpi_scale = main_viewport->DpiScale;
            g_runtime_global_context.m_render_system->updateEngineContentViewport(render_target_window_pos.x * dpi_scale,
                                                                                  render_target_window_pos.y * dpi_scale,
                                                                                  render_target_window_size.x * dpi_scale,
                                                                                  render_target_window_size.y * dpi_scale);
#else
            g_runtime_global_context.m_render_system->updateEngineContentViewport(
                render_target_window_pos.x, render_target_window_pos.y, render_target_window_size.x, render_target_window_size.y);
#endif
            g_editor_global_context.m_input_manager->setEngineWindowPos(render_target_window_pos);
            g_editor_global_context.m_input_manager->setEngineWindowSize(render_target_window_size);
        }

        ImGui::InvisibleButton("DropTarget", ImVec2(render_target_window_size.x, render_target_window_size.y));
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_DRAG_DROP"))
            {
                IM_ASSERT(payload->DataSize == sizeof(std::shared_ptr<EditorFileNode>));
                std::shared_ptr<EditorFileNode>* dropped_node_ptr = static_cast<std::shared_ptr<EditorFileNode>*>(payload->Data);
                std::shared_ptr<EditorFileNode>  dropped_node     = *dropped_node_ptr;

                ImVec2 mouse_pos = ImGui::GetMousePos();

                ImVec2 game_window_pos  = ImGui::GetWindowPos();
                ImVec2 game_window_size = ImGui::GetWindowSize();
                ImVec2 relative_mouse_pos;
                relative_mouse_pos.x = mouse_pos.x - game_window_pos.x;
                relative_mouse_pos.y = mouse_pos.y - game_window_pos.y;

                ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "relative_mouse_pos: [%f][%f]", relative_mouse_pos.x, relative_mouse_pos.y);

                if (relative_mouse_pos.x >= 0 && relative_mouse_pos.y >= 0 && relative_mouse_pos.x < game_window_size.x &&
                    relative_mouse_pos.y < game_window_size.y)
                {
                    // 現在のアクティブなレベルを取得する
                    std::shared_ptr<Level> level = g_runtime_global_context.m_world_manager->getCurrentActiveLevel().lock();
                    if (level == nullptr)
                        return;

                    // 新しいオブジェクトのインデックスをインクリメントする
                    const unsigned int new_object_index = ++m_new_object_index_map[dropped_node->m_file_name];

                    // 新しいオブジェクトインスタンスリソースを作成する
                    ObjectInstanceRes new_object_instance_res;
                    new_object_instance_res.m_name =
                        "New_" + Path::getFilePureName(dropped_node->m_file_name) + "_" + std::to_string(new_object_index);
                    new_object_instance_res.m_definition =
                        g_runtime_global_context.m_asset_manager->getFullPath(dropped_node->m_file_path).generic_string();

                    // 新しいGameObjectを作成し、シーンに追加する
                    size_t new_gobject_id = level->createObject(new_object_instance_res);
                    if (new_gobject_id != k_invalid_object_id)
                    {
                        // 新しいGameObjectが正常に作成された場合、それを選択する
                        g_editor_global_context.m_scene_manager->onGObjectSelected(new_gobject_id);
                    }
                }
            }
            ImGui::EndDragDropTarget();
        }

        ImGui::End();
    }

    void EditorUI::drawAxisToggleButton(const char* string_id, bool check_state, int axis_mode)
    {
        if (check_state)
        {
            ImGui::PushID(string_id);
            auto check_button_color = ImVec4(93.0f / 255.0f, 10.0f / 255.0f, 66.0f / 255.0f, 1.00f);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(check_button_color.x, check_button_color.y, check_button_color.z, 0.40f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, check_button_color);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, check_button_color);
            ImGui::Button(string_id);
            ImGui::PopStyleColor(3);
            ImGui::PopID();
        }
        else
        {
            if (ImGui::Button(string_id))
            {
                check_state = true;
                g_editor_global_context.m_scene_manager->setEditorAxisMode((EditorAxisMode)axis_mode);
                g_editor_global_context.m_scene_manager->drawSelectedEntityAxis();
            }
        }
    }

    void EditorUI::initialize()
    {
        g_editor_global_context.m_panels_manager = std::make_shared<PanelsManager>(m_canvas);

        m_iconTextureMap["folder"]   = "resource/icon/folder.png";
        m_iconTextureMap["object"]   = "resource/icon/object.png";
        m_iconTextureMap["question"] = "resource/icon/question.png";

        setupUI();

        // initialize imgui vulkan render backend
        g_runtime_global_context.m_render_system->initializeUIRenderBackend(this);
    }

    void EditorUI::preRender()
    {
        g_editor_global_context.m_ui_manager->Render();

        auto& sceneView = g_editor_global_context.m_panels_manager->GetPanelAs<SceneView>("Scene View");

        if (sceneView.IsOpened())
            sceneView._Render_Impl();

        // showEditorUI();
    }

    void DrawVecControl(const std::string& label, LunarYue::Vector3& values, float resetValue, float columnWidth)
    {
        ImGui::PushID(label.c_str());

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);
        ImGui::Text("%s", label.c_str());
        ImGui::NextColumn();

        ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2 {0, 0});

        float  lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
        ImVec2 buttonSize = {lineHeight + 3.0f, lineHeight};

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4 {0.8f, 0.1f, 0.15f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4 {0.9f, 0.2f, 0.2f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4 {0.8f, 0.1f, 0.15f, 1.0f});
        if (ImGui::Button("X", buttonSize))
            values.x = resetValue;
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4 {0.2f, 0.45f, 0.2f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4 {0.3f, 0.55f, 0.3f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4 {0.2f, 0.45f, 0.2f, 1.0f});
        if (ImGui::Button("Y", buttonSize))
            values.y = resetValue;
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4 {0.1f, 0.25f, 0.8f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4 {0.2f, 0.35f, 0.9f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4 {0.1f, 0.25f, 0.8f, 1.0f});
        if (ImGui::Button("Z", buttonSize))
            values.z = resetValue;
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();

        ImGui::PopStyleVar();

        ImGui::Columns(1);
        ImGui::PopID();
    }

    void DrawVecControl(const std::string& label, LunarYue::Quaternion& values, float resetValue, float columnWidth)
    {
        ImGui::PushID(label.c_str());

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);
        ImGui::Text("%s", label.c_str());
        ImGui::NextColumn();

        ImGui::PushMultiItemsWidths(4, ImGui::CalcItemWidth());
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2 {0, 0});

        float  lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
        ImVec2 buttonSize = {lineHeight + 3.0f, lineHeight};

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4 {0.8f, 0.1f, 0.15f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4 {0.9f, 0.2f, 0.2f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4 {0.8f, 0.1f, 0.15f, 1.0f});
        if (ImGui::Button("X", buttonSize))
            values.x = resetValue;
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4 {0.2f, 0.45f, 0.2f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4 {0.3f, 0.55f, 0.3f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4 {0.2f, 0.45f, 0.2f, 1.0f});
        if (ImGui::Button("Y", buttonSize))
            values.y = resetValue;
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4 {0.1f, 0.25f, 0.8f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4 {0.2f, 0.35f, 0.9f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4 {0.1f, 0.25f, 0.8f, 1.0f});
        if (ImGui::Button("Z", buttonSize))
            values.z = resetValue;
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4 {0.5f, 0.25f, 0.5f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4 {0.6f, 0.35f, 0.6f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4 {0.5f, 0.25f, 0.5f, 1.0f});
        if (ImGui::Button("W", buttonSize))
            values.w = resetValue;
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##W", &values.w, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();

        ImGui::PopStyleVar();

        ImGui::Columns(1);
        ImGui::PopID();
    }
} // namespace LunarYue
