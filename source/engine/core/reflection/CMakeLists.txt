add_library(reflection INTERFACE)
target_link_libraries(reflection INTERFACE rttr_core_lib)
target_add_ide_support(reflection ${ENGINE_CORE_FOLDER})
