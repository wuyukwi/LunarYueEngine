#include "editor/include/editor.h"
#include "editor/include/editor_launcher.h"
#include <filesystem>
//
// namespace LunarYue
//{
//     int main(int argc, char** argv)
//     {
//         // const std::filesystem::path executable_path(argv[0]);                                                // ���s�t�@�C���̃p�X���擾
//         // const std::filesystem::path config_file_path = executable_path.parent_path() / "LunarYueEditor.ini"; // �ݒ�t�@�C���̃p�X��ݒ�
//
//         // std::shared_ptr engine = std::make_shared<LunarYue::LunarYueEngine>();
//         // LunarYue::LunarYueEngine::startEngine(config_file_path.generic_string()); // �G���W�����J�n
//         // engine->initialize();                                                     // �G���W����������
//
//         std::unique_ptr<LunarYueEditor> editor = std::make_shared<LunarYueEditor>();
//         editor->initialize(engine); // �G�f�B�^��������
//
//         editor->run(); // �G�f�B�^���N��
//
//         editor->clear(); // �G�f�B�^�̃C���X�^���X���N���A
//
//         engine->clear();          // �G���W���̃C���X�^���X���N���A
//         engine->shutdownEngine(); // �G���W�����V���b�g�_�E��
//         return 0;
//     }
// } // namespace LunarYue

int _main_(int _argc, char** _argv)
{
    // std::shared_ptr<LunarYue::EditorLauncher> launcher = std::make_shared<LunarYue::EditorLauncher>();
    // entry::runApp(launcher.get(), _argc, _argv);
    // return 0;
    std::shared_ptr<LunarYue::LunarYueEditor> editor = std::make_shared<LunarYue::LunarYueEditor>();
    return entry::runApp(editor.get(), _argc, _argv);
}
