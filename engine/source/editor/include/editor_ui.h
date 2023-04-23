#pragma once

#include "runtime/function/framework/object/object.h"
#include "runtime/function/ui/window_ui.h"

#include "editor/include/editor_file_service.h"

#include <chrono>

namespace LunarYue
{
    // LunarYue�G�f�B�^��UI�N���X
    class EditorUI : public WindowUI
    {
    public:
        // �R���X�g���N�^
        EditorUI();

    private:
        // �t�@�C���R���e���c�A�C�e�����N���b�N���ꂽ�Ƃ��̏���
        void onFileContentItemClicked(EditorFileNode* node);
        // �G�f�B�^�t�@�C���A�Z�b�gUI�c���[�̍\�z
        void buildEditorFileAssetsUITree(EditorFileNode* node);
        // ���̃g�O���{�^����`��
        void drawAxisToggleButton(const char* string_id, bool check_state, int axis_mode);
        // �N���XUI���쐬
        void createClassUI(Reflection::ReflectionInstance& instance);
        // ���[�t�m�[�hUI���쐬
        void createLeafNodeUI(Reflection::ReflectionInstance& instance);
        // ���[�tUI�m�[�h�̐e���x�����擾
        std::string getLeafUINodeParentLabel();

        // �G�f�B�^UI��\��
        void showEditorUI();
        // �G�f�B�^���j���[��\��
        void showEditorMenu(bool* p_open);
        // �G�f�B�^���[���h�I�u�W�F�N�g�E�B���h�E��\��
        void showEditorWorldObjectsWindow(bool* p_open);
        // �G�f�B�^�t�@�C���R���e���c�E�B���h�E��\��
        void showEditorFileContentWindow(bool* p_open);
        // �G�f�B�^�Q�[���E�B���h�E��\��
        void showEditorGameWindow(bool* p_open);
        // �G�f�B�^�ڍ׃E�B���h�E��\��
        void showEditorDetailWindow(bool* p_open);

        // UI�̃J���[�X�^�C����ݒ�
        void setUIColorStyle();

    public:
        // ������
        virtual void initialize(WindowUIInitInfo init_info) override final;
        // �v�������_�[
        virtual void preRender() override final;

    private:
        // �G�f�B�^UI�N���G�[�^�̃}�b�v
        std::unordered_map<std::string, std::function<void(std::string, void*)>> m_editor_ui_creator;
        // �V�����I�u�W�F�N�g�C���f�b�N�X�}�b�v
        std::unordered_map<std::string, unsigned int> m_new_object_index_map;
        // �G�f�B�^�t�@�C���T�[�r�X
        EditorFileService m_editor_file_service;
        // �Ō�̃t�@�C���c���[�A�b�v�f�[�g
        std::chrono::time_point<std::chrono::steady_clock> m_last_file_tree_update;

        // �e�E�B���h�E�̃I�[�v����ԃt���O
        bool m_editor_menu_window_open       = true;
        bool m_asset_window_open             = true;
        bool m_game_engine_window_open       = true;
        bool m_file_content_window_open      = true;
        bool m_detail_window_open            = true;
        bool m_scene_lights_window_open      = true;
        bool m_scene_lights_data_window_open = true;
    };
} // namespace LunarYue
