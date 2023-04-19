#include <filesystem>
#include <iostream>
#include <string>
#include <thread>
#include <unordered_map>

#include "runtime/engine.h"

#include "editor/include/editor.h"

// https://gcc.gnu.org/onlinedocs/cpp/Stringizing.html
#define LunarYue_XSTR(s) LunarYue_STR(s)
#define LunarYue_STR(s) #s

int main(int argc, char** argv)
{
    std::filesystem::path executable_path(argv[0]);
    std::filesystem::path config_file_path = executable_path.parent_path() / "LunarYueEditor.ini";

    LunarYue::LunarYueEngine* engine = new LunarYue::LunarYueEngine();

    engine->startEngine(config_file_path.generic_string());
    engine->initialize();

    LunarYue::LunarYueEditor* editor = new LunarYue::LunarYueEditor();
    editor->initialize(engine);

    editor->run();

    editor->clear();

    engine->clear();
    engine->shutdownEngine();

    return 0;
}
