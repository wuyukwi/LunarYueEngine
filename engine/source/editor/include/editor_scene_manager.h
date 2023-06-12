#pragma once

#include "editor/include/axis.h"

#include "runtime/function/framework/object/object.h"
#include "runtime/function/render/render_object.h"

#include <memory>
#include <utility>

namespace LunarYue
{
    class LunarYueEditor;
    class RenderCamera;
    class RenderEntity;

    // エディタの軸モード
    enum class EditorAxisMode : int
    {
        TranslateMode = 0,
        RotateMode    = 1,
        ScaleMode     = 2,
        Default       = 3
    };

    // エディタシーンマネージャクラス
    class EditorSceneManager
    {
    public:
        // 初期化関数
        void initialize();
        // 更新関数
        void tick(float delta_time);

    public:
        // カーソルが軸上にあるかどうかを更新する関数
        size_t updateCursorOnAxis(Vector2 cursor_uv, Vector2 game_engine_window_size);
        // 選択されたエンティティの軸を描画する関数
        void drawSelectedEntityAxis();
        // 選択されたGObjectを取得する関数
        std::weak_ptr<Object> getSelectedGObject() const;
        std::weak_ptr<Object> getGObjectFormID(ObjectID id) const;
        // 軸モードに応じた軸メッシュを取得する関数
        RenderEntity* getAxisMeshByType(EditorAxisMode axis_mode);
        // GObjectが選択された際の処理を行う関数
        void onGObjectSelected(ObjectID selected_gobject_id);
        // 選択されたGObjectを削除する関数
        void onDeleteSelectedGObject();
        // エンティティの移動を行う関数
        void moveEntity(float     new_mouse_pos_x,
                        float     new_mouse_pos_y,
                        float     last_mouse_pos_x,
                        float     last_mouse_pos_y,
                        Vector2   engine_window_pos,
                        Vector2   engine_window_size,
                        size_t    cursor_on_axis,
                        Matrix4x4 model_matrix);

        // エディタカメラを設定する関数
        void setEditorCamera(std::shared_ptr<RenderCamera> camera) { m_camera = std::move(camera); }
        // 軸リソースをアップロードする関数
        void uploadAxisResource();
        // ピックされたメッシュのGUIDを取得する関数
        size_t getGuidOfPickedMesh(const Vector2& picked_uv) const;

    public:
        // エディタカメラを取得する関数
        std::shared_ptr<RenderCamera> getEditorCamera() { return m_camera; };

        // 選択されたオブジェクトのIDを取得する関数
        ObjectID getSelectedObjectID() { return m_selected_gobject_id; };
        // 選択されたオブジェクトの行列を取得する関数
        Matrix4x4 getSelectedObjectMatrix() { return m_selected_object_matrix; }
        // エディタ軸モードを取得する関数
        EditorAxisMode getEditorAxisMode() { return m_axis_mode; }

        // 選択されたオブジェクトのIDを設定する関数
        void setSelectedObjectID(ObjectID selected_gobject_id) { m_selected_gobject_id = selected_gobject_id; };
        // 選択されたオブジェクトの行列を設定する関数
        void setSelectedObjectMatrix(Matrix4x4 new_object_matrix) { m_selected_object_matrix = new_object_matrix; }
        // エディタ軸モードを設定する関数
        void setEditorAxisMode(EditorAxisMode new_axis_mode) { m_axis_mode = new_axis_mode; }

    private:
        // エディタの平行移動軸
        EditorTranslationAxis m_translation_axis;
        // エディタの回転軸
        EditorRotationAxis m_rotation_axis;
        // エディタの拡大縮小軸
        EditorScaleAxis m_scale_aixs;

        // 選択されたGObjectのID
        ObjectID m_selected_gobject_id {k_invalid_object_id};
        // 選択されたオブジェクトの行列
        Matrix4x4 m_selected_object_matrix {Matrix4x4::IDENTITY};

        // エディタの軸モード
        EditorAxisMode m_axis_mode {EditorAxisMode::TranslateMode};
        // エディタのカメラ
        std::shared_ptr<RenderCamera> m_camera;

        // 選択された軸のインデックス
        size_t m_selected_axis {3};

        // 軸を表示するかどうかのフラグ
        bool m_is_show_axis = true;
    };
} // namespace LunarYue
