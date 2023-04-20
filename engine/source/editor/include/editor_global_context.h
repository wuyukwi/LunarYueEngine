#pragma once

namespace LunarYue
{
    // �O���[�o���R���e�L�X�g���������\����
    struct EditorGlobalContextInitInfo
    {
        class WindowSystem*   window_system;  // �E�B���h�E�V�X�e��
        class RenderSystem*   render_system;  // �����_�[�V�X�e��
        class LunarYueEngine* engine_runtime; // �G���W�������^�C��
    };

    // �G�f�B�^�O���[�o���R���e�L�X�g�N���X
    class EditorGlobalContext
    {
    public:
        class EditorSceneManager* m_scene_manager {nullptr};  // �V�[���}�l�[�W��
        class EditorInputManager* m_input_manager {nullptr};  // ���̓}�l�[�W��
        class RenderSystem*       m_render_system {nullptr};  // �����_�[�V�X�e��
        class WindowSystem*       m_window_system {nullptr};  // �E�B���h�E�V�X�e��
        class LunarYueEngine*     m_engine_runtime {nullptr}; // �G���W�������^�C��

    public:
        // �������֐�
        void initialize(const EditorGlobalContextInitInfo& init_info);
        // �N���A�֐�
        void clear();
    };

    // �G�f�B�^�O���[�o���R���e�L�X�g�O���ϐ�
    extern EditorGlobalContext g_editor_global_context;
} // namespace LunarYue
