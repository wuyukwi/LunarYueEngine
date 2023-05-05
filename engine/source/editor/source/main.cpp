#include <filesystem>
#include <iostream>
#include <thread>

#include "runtime/engine.h"

#include "editor/include/editor.h"

int main(int argc, char** argv)
{
    const std::filesystem::path executable_path(argv[0]);                                                // ���s�t�@�C���̃p�X���擾
    std::filesystem::path       config_file_path = executable_path.parent_path() / "LunarYueEditor.ini"; // �ݒ�t�@�C���̃p�X��ݒ�

    auto* engine = new LunarYue::LunarYueEngine(); // �G���W���̃C���X�^���X�𐶐�

    LunarYue::LunarYueEngine::startEngine(config_file_path.generic_string()); // �G���W�����J�n
    engine->initialize();                                                     // �G���W����������

    auto* editor = new LunarYue::LunarYueEditor(); // �G�f�B�^�̃C���X�^���X�𐶐�
    editor->initialize(engine);                    // �G�f�B�^��������

    editor->run(); // �G�f�B�^���N��

    editor->clear(); // �G�f�B�^�̃C���X�^���X���N���A

    engine->clear();          // �G���W���̃C���X�^���X���N���A
    engine->shutdownEngine(); // �G���W�����V���b�g�_�E��

    return 0;
}
