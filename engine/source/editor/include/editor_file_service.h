#pragma once

#include <memory>
#include <string>
#include <vector>

namespace LunarYue
{
    // �G�f�B�^�t�@�C���m�[�h�N���X�̒�`
    class EditorFileNode;
    // �G�f�B�^�t�@�C���m�[�h�̔z����`
    using EditorFileNodeArray = std::vector<std::shared_ptr<EditorFileNode>>;

    // �G�f�B�^�t�@�C���m�[�h�\����
    struct EditorFileNode
    {
        std::string         m_file_name;   // �t�@�C����
        std::string         m_file_type;   // �t�@�C���^�C�v
        std::string         m_file_path;   // �t�@�C���p�X
        int                 m_node_depth;  // �m�[�h�̐[��
        bool                m_folder_open; // �t�H���_���J���Ă��邩�ǂ���
        EditorFileNodeArray m_child_nodes; // �q�m�[�h�̔z��

        // �f�t�H���g�R���X�g���N�^
        EditorFileNode() = default;
        // �p�����[�^�t���R���X�g���N�^
        EditorFileNode(const std::string& name, const std::string& type, const std::string& path, int depth) :
            m_file_name(name), m_file_type(type), m_file_path(path), m_node_depth(depth)
        {}
    };

    // �G�f�B�^�t�@�C���T�[�r�X�N���X
    class EditorFileService
    {
        EditorFileNodeArray m_file_node_array;                            // �G�f�B�^�t�@�C���m�[�h�̔z��
        EditorFileNode      m_root_node {"asset", "Folder", "asset", -1}; // ���[�g�m�[�h
        EditorFileNode*     m_selected_folder_node = nullptr;             // �I�����ꂽ�t�H���_�m�[�h

    private:
        // �t�@�C���z����`�F�b�N����֐�
        bool checkFileArray(EditorFileNode* file_node);

    public:
        // �G�f�B�^�̃��[�g�m�[�h���擾����֐�
        EditorFileNode* getEditorRootNode() { return m_file_node_array.empty() ? nullptr : m_file_node_array[0].get(); }

        // �e�m�[�h�|�C���^���擾����֐�
        EditorFileNode* getParentNodePtr(EditorFileNode* file_node);

        // �I�����ꂽ�t�H���_�m�[�h��ݒ肷�郁�\�b�h
        void setSelectedFolderNode(EditorFileNode* node) { m_selected_folder_node = node; }

        // �I�����ꂽ�t�H���_�m�[�h���擾���郁�\�b�h
        EditorFileNode* getSelectedFolderNode() const { return m_selected_folder_node; }

        // �G���W���t�@�C���c���[���\�z����֐�
        void buildEngineFileTree();
    };
} // namespace LunarYue
