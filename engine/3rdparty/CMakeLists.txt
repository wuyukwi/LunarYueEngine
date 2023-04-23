set(third_party_folder "ThirdParty")

# ���L���C�u�������r���h���Ȃ��悤�ɂ���
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# imgui�̃r���h�ƃv���p�e�B�ݒ�
if(NOT TARGET imgui)
    include(imgui.cmake)
    set_target_properties(imgui PROPERTIES FOLDER ${third_party_folder}/imgui)
endif()

# glfw�̃r���h�ƃv���p�e�B�ݒ�
if(NOT TARGET glfw)
    option(GLFW_BUILD_EXAMPLES "" OFF)
    option(GLFW_BUILD_TESTS "" OFF)
    option(GLFW_BUILD_DOCS "" OFF)
    option(GLFW_INSTALL "" OFF)
    add_subdirectory(glfw)
    set_target_properties(glfw PROPERTIES FOLDER ${third_party_folder}/glfw)
    set_target_properties(update_mappings PROPERTIES FOLDER ${third_party_folder}/glfw)
endif()

# spdlog�̃r���h�ƃv���p�e�B�ݒ�
if(NOT TARGET spdlog)
    option(SPDLOG_BUILD_EXAMPLE "" OFF)
    option(SPDLOG_INSTALL "" OFF)
    add_subdirectory(spdlog)
    set_target_properties(spdlog PROPERTIES FOLDER ${third_party_folder}/spdlog)
endif()

# stb�̃r���h�ƃv���p�e�B�ݒ�
if(NOT TARGET stb)
    include(stb.cmake)
    set_target_properties(stb PROPERTIES FOLDER ${third_party_folder}/stb)
endif()

# tinyobjloader�̃r���h�ƃv���p�e�B�ݒ�
if(NOT TARGET tinyobjloader)
    add_subdirectory(tinyobjloader)
    set_target_properties(tinyobjloader PROPERTIES FOLDER ${third_party_folder}/tinyobjloader)
    set_target_properties(uninstall PROPERTIES FOLDER ${third_party_folder}/tinyobjloader)
endif()

# Jolt�̃r���h�ƃv���p�e�B�ݒ�
if(NOT TARGET Jolt)
    option(TARGET_HELLO_WORLD "" OFF)
    option(TARGET_PERFORMANCE_TEST "" OFF)
    option(TARGET_SAMPLES "" OFF)
    option(TARGET_UNIT_TESTS "" OFF)
    option(TARGET_VIEWER "" OFF)

    # �����f�o�b�O�����_�����O���L���ȏꍇ�A�e�X�g�t���[�����[�N���r���h����
    if(ENABLE_PHYSICS_DEBUG_RENDERER)
        option(TARGET_TEST_FRAMEWORK "" ON)
    else()
        option(TARGET_TEST_FRAMEWORK "" OFF)
    endif()

    add_subdirectory(JoltPhysics/Build)

    # �����f�o�b�O�����_�����O���L���ȏꍇ��TestFramework���܂߂�
    if(ENABLE_PHYSICS_DEBUG_RENDERER)
        set_target_properties(Jolt TestFramework
            PROPERTIES 
            FOLDER ${third_party_folder}/JoltPhysics
            MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
    else()
        set_target_properties(Jolt
            PROPERTIES 
            FOLDER ${third_party_folder}/JoltPhysics
            MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
    endif()

endif()

# sol2�̃r���h�ƃv���p�e�B�ݒ�
if(NOT TARGET sol2)
    add_subdirectory(sol2-3.3.0)
endif()

# lua�̃v���p�e�B�ݒ�
if(NOT TARGET lua)
    include(lua.cmake)
    set_target_properties(lua_static PROPERTIES FOLDER ${third_party_folder}/lua)
endif()