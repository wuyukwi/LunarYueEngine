#include <filesystem>
#include <iostream>
#include <thread>

#include "runtime/engine.h"

#include "editor/include/editor.h"

int main(int argc, char** argv)
{
    const std::filesystem::path executable_path(argv[0]);                                                // 実行ファイルのパスを取得
    std::filesystem::path       config_file_path = executable_path.parent_path() / "LunarYueEditor.ini"; // 設定ファイルのパスを設定

    auto* engine = new LunarYue::LunarYueEngine(); // エンジンのインスタンスを生成

    LunarYue::LunarYueEngine::startEngine(config_file_path.generic_string()); // エンジンを開始
    engine->initialize();                                                     // エンジンを初期化

    auto* editor = new LunarYue::LunarYueEditor(); // エディタのインスタンスを生成
    editor->initialize(engine);                    // エディタを初期化

    editor->run(); // エディタを起動

    editor->clear(); // エディタのインスタンスをクリア

    engine->clear();          // エンジンのインスタンスをクリア
    engine->shutdownEngine(); // エンジンをシャットダウン

    return 0;
}
