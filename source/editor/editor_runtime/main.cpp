#include "meta/meta.h"
#include "system/app.h"

#include <core/filesystem/filesystem.h>

int main(int argc, char* argv[])
{
	const fs::path binary_path = fs::executable_path(argv[0]).parent_path();

	// fs::path engine_path         = fs::absolute(fs::path(ENGINE_DIRECTORY));
	// fs::path shader_include_path = fs::absolute(fs::path(SHADER_INCLUDE_DIRECTORY));

	fs::path engine_path = binary_path;
	fs::path shader_include_path = binary_path / "data/src";

	const fs::path engine = engine_path / "data/engine_data";
	const fs::path editor = engine_path / "data/editor_data";
	fs::add_path_protocol("engine:", engine);
	fs::add_path_protocol("editor:", editor);
	fs::add_path_protocol("binary:", binary_path);
	fs::add_path_protocol("shader_include:", shader_include_path);
	editor::app app;

	const int return_code = app.run(argc, argv);

	return return_code;
}
