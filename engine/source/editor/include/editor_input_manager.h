#pragma once

#include "runtime/core/math/vector2.h"

#include "runtime/function/render/render_camera.h"

#include <vector>

namespace LunarYue
{
    class LunarYueEditor;

    // �G�f�B�^�R�}���h�񋓌^
    enum class EditorCommand : unsigned int
    {
        camera_left      = 1 << 0,  // A
        camera_back      = 1 << 1,  // S
        camera_foward    = 1 << 2,  // W
        camera_right     = 1 << 3,  // D
        camera_up        = 1 << 4,  // Q
        camera_down      = 1 << 5,  // E
        translation_mode = 1 << 6,  // T
        rotation_mode    = 1 << 7,  // R
        scale_mode       = 1 << 8,  // C
        exit             = 1 << 9,  // Esc
        delete_object    = 1 << 10, // Delete
    };

    // �G�f�B�^���̓}�l�[�W���N���X
    class EditorInputManager
    {
    public:
        // �������֐�
        void initialize();
        // �e�B�b�N�֐�
        void tick(float delta_time);

    public:
        // ���͓o�^�֐�
        void registerInput();
        // �J�[�\���̍��W�X�V�֐�
        void updateCursorOnAxis(Vector2 cursor_uv);
        // �G�f�B�^�R�}���h�����֐�
        void processEditorCommand();
        // �G�f�B�^���[�h�ł̃L�[���͊֐�
        void onKeyInEditorMode(int key, int scancode, int action, int mods);

        // �L�[���͊֐�
        void onKey(int key, int scancode, int action, int mods);
        // ���Z�b�g�֐�
        void onReset();
        // �J�[�\���ʒu�֐�
        void onCursorPos(double xpos, double ypos);
        // �J�[�\�����͊֐�
        void onCursorEnter(int entered);
        // �X�N���[���֐�
        void onScroll(double xoffset, double yoffset);
        // �}�E�X�{�^���N���b�N�֐�
        void onMouseButtonClicked(int key, int action);
        // �E�B���h�E�N���[�Y�֐�
        void onWindowClosed();

        // �J�[�\������`���ɂ��邩�ǂ����𔻒肷��֐�
        bool isCursorInRect(Vector2 pos, Vector2 size) const;

    public:
        // �G���W���E�B���h�E�ʒu�擾�֐�
        Vector2 getEngineWindowPos() const { return m_engine_window_pos; };
        // �G���W���E�B���h�E�T�C�Y�擾�֐�
        Vector2 getEngineWindowSize() const { return m_engine_window_size; };
        // �J�������x�擾�֐�
        float getCameraSpeed() const { return m_camera_speed; };

        // �G���W���E�B���h�E�ʒu�ݒ�֐�
        void setEngineWindowPos(Vector2 new_window_pos) { m_engine_window_pos = new_window_pos; };
        // �G���W���E�B���h�E�T�C�Y�ݒ�֐�
        void setEngineWindowSize(Vector2 new_window_size) { m_engine_window_size = new_window_size; };
        // �G�f�B�^�R�}���h���Z�b�g�֐�
        void resetEditorCommand() { m_editor_command = 0; }

    private:
        // �G���W���E�B���h�E�̈ʒu
        Vector2 m_engine_window_pos {0.0f, 0.0f};
        // �G���W���E�B���h�E�̃T�C�Y
        Vector2 m_engine_window_size {1280.0f, 768.0f};
        // �}�E�X��X���W
        float m_mouse_x {0.0f};
        // �}�E�X��Y���W
        float m_mouse_y {0.0f};
        // �J�����̑��x
        float m_camera_speed {0.05f};

        // �J�[�\�������ォ�ǂ����𔻒f���邽�߂̕ϐ�
        size_t m_cursor_on_axis {3};
        // �G�f�B�^�R�}���h
        unsigned int m_editor_command {0};
    };
} // namespace LunarYue
