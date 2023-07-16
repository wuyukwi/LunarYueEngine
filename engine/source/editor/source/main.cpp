#include "editor/include/editor.h"
#include "editor/include/editor_launcher.h"
#include <filesystem>
//
// namespace LunarYue
//{
//     int main(int argc, char** argv)
//     {
//         // const std::filesystem::path executable_path(argv[0]);                                                // 実行ファイルのパスを取得
//         // const std::filesystem::path config_file_path = executable_path.parent_path() / "LunarYueEditor.ini"; // 設定ファイルのパスを設定
//
//         // std::shared_ptr engine = std::make_shared<LunarYue::LunarYueEngine>();
//         // LunarYue::LunarYueEngine::startEngine(config_file_path.generic_string()); // エンジンを開始
//         // engine->initialize();                                                     // エンジンを初期化
//
//         std::unique_ptr<LunarYueEditor> editor = std::make_shared<LunarYueEditor>();
//         editor->initialize(engine); // エディタを初期化
//
//         editor->run(); // エディタを起動
//
//         editor->clear(); // エディタのインスタンスをクリア
//
//         engine->clear();          // エンジンのインスタンスをクリア
//         engine->shutdownEngine(); // エンジンをシャットダウン
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
