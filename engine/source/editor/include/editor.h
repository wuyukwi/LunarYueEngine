#pragma once

#include "runtime/core/math/vector2.h"

#include <memory>

namespace LunarYue
{
    class EditorUI;
    class LunarYueEngine;

    // LunarYueEditor�N���X�́ALunarYue�G���W���̃G�f�B�^�@�\��񋟂��܂��B
    // ���̃N���X�́AEditorUI��LunarYueEngine�ƘA�g���ē��삵�܂��B
    class LunarYueEditor
    {
        friend class EditorUI;

    public:
        // �R���X�g���N�^
        LunarYueEditor();

        // �f�X�g���N�^
        virtual ~LunarYueEditor();

        // �G���W���̏�����
        // engine_runtime: LunarYueEngine�C���X�^���X�ւ̃|�C���^
        void initialize(LunarYueEngine* engine_runtime);

        // �G�f�B�^�̏�Ԃ��N���A
        void clear();

        // �G�f�B�^�̎��s
        void run();

    protected:
        // EditorUI�̃C���X�^���X�ւ̋��L�|�C���^
        std::shared_ptr<EditorUI> m_editor_ui;

        // LunarYueEngine�C���X�^���X�ւ̃|�C���^
        LunarYueEngine* m_engine_runtime {nullptr};
    };
} // namespace LunarYue
