file(
	GLOB 
	IMGUI_SOURCES 
	imgui/*.cpp 
	imgui/*.h 
	imgui_user/*.cpp 
	imgui_user/*.h 	
	imguizmo/*.cpp 
	imguizmo/*.h 
)

add_library (imgui ${IMGUI_SOURCES})

if(MSVC)
set(imgui_flags
_CRT_SECURE_NO_WARNINGS
_CRT_SECURE_NO_DEPRECATE)
endif()

set(imgui_flags ${imgui_flags}
IMGUI_DISABLE_OBSOLETE_FUNCTIONS)

target_compile_definitions(imgui PUBLIC ${imgui_flags})
target_include_directories (imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(imgui PROPERTIES FOLDER ${THIRD_PARTY_FOLDER})
