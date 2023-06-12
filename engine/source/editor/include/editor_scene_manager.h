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

    // �G�f�B�^�̎����[�h
    enum class EditorAxisMode : int
    {
        TranslateMode = 0,
        RotateMode    = 1,
        ScaleMode     = 2,
        Default       = 3
    };

    // �G�f�B�^�V�[���}�l�[�W���N���X
    class EditorSceneManager
    {
    public:
        // �������֐�
        void initialize();
        // �X�V�֐�
        void tick(float delta_time);

    public:
        // �J�[�\��������ɂ��邩�ǂ������X�V����֐�
        size_t updateCursorOnAxis(Vector2 cursor_uv, Vector2 game_engine_window_size);
        // �I�����ꂽ�G���e�B�e�B�̎���`�悷��֐�
        void drawSelectedEntityAxis();
        // �I�����ꂽGObject���擾����֐�
        std::weak_ptr<Object> getSelectedGObject() const;
        std::weak_ptr<Object> getGObjectFormID(ObjectID id) const;
        // �����[�h�ɉ����������b�V�����擾����֐�
        RenderEntity* getAxisMeshByType(EditorAxisMode axis_mode);
        // GObject���I�����ꂽ�ۂ̏������s���֐�
        void onGObjectSelected(ObjectID selected_gobject_id);
        // �I�����ꂽGObject���폜����֐�
        void onDeleteSelectedGObject();
        // �G���e�B�e�B�̈ړ����s���֐�
        void moveEntity(float     new_mouse_pos_x,
                        float     new_mouse_pos_y,
                        float     last_mouse_pos_x,
                        float     last_mouse_pos_y,
                        Vector2   engine_window_pos,
                        Vector2   engine_window_size,
                        size_t    cursor_on_axis,
                        Matrix4x4 model_matrix);

        // �G�f�B�^�J������ݒ肷��֐�
        void setEditorCamera(std::shared_ptr<RenderCamera> camera) { m_camera = std::move(camera); }
        // �����\�[�X���A�b�v���[�h����֐�
        void uploadAxisResource();
        // �s�b�N���ꂽ���b�V����GUID���擾����֐�
        size_t getGuidOfPickedMesh(const Vector2& picked_uv) const;

    public:
        // �G�f�B�^�J�������擾����֐�
        std::shared_ptr<RenderCamera> getEditorCamera() { return m_camera; };

        // �I�����ꂽ�I�u�W�F�N�g��ID���擾����֐�
        ObjectID getSelectedObjectID() { return m_selected_gobject_id; };
        // �I�����ꂽ�I�u�W�F�N�g�̍s����擾����֐�
        Matrix4x4 getSelectedObjectMatrix() { return m_selected_object_matrix; }
        // �G�f�B�^�����[�h���擾����֐�
        EditorAxisMode getEditorAxisMode() { return m_axis_mode; }

        // �I�����ꂽ�I�u�W�F�N�g��ID��ݒ肷��֐�
        void setSelectedObjectID(ObjectID selected_gobject_id) { m_selected_gobject_id = selected_gobject_id; };
        // �I�����ꂽ�I�u�W�F�N�g�̍s���ݒ肷��֐�
        void setSelectedObjectMatrix(Matrix4x4 new_object_matrix) { m_selected_object_matrix = new_object_matrix; }
        // �G�f�B�^�����[�h��ݒ肷��֐�
        void setEditorAxisMode(EditorAxisMode new_axis_mode) { m_axis_mode = new_axis_mode; }

    private:
        // �G�f�B�^�̕��s�ړ���
        EditorTranslationAxis m_translation_axis;
        // �G�f�B�^�̉�]��
        EditorRotationAxis m_rotation_axis;
        // �G�f�B�^�̊g��k����
        EditorScaleAxis m_scale_aixs;

        // �I�����ꂽGObject��ID
        ObjectID m_selected_gobject_id {k_invalid_object_id};
        // �I�����ꂽ�I�u�W�F�N�g�̍s��
        Matrix4x4 m_selected_object_matrix {Matrix4x4::IDENTITY};

        // �G�f�B�^�̎����[�h
        EditorAxisMode m_axis_mode {EditorAxisMode::TranslateMode};
        // �G�f�B�^�̃J����
        std::shared_ptr<RenderCamera> m_camera;

        // �I�����ꂽ���̃C���f�b�N�X
        size_t m_selected_axis {3};

        // ����\�����邩�ǂ����̃t���O
        bool m_is_show_axis = true;
    };
} // namespace LunarYue
