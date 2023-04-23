#PackageProject.cmake���g�p���āA�^�[�Q�b�g���C���X�g�[���\�ɂ��܂�
set(TARGET_NAME LunarYueRuntime)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#json11��ǉ�
set(JSON_INCLUDE ${THIRD_PARTY_DIR}/json11)
add_library(json11 ${JSON_INCLUDE}/json11.cpp)
set_target_properties(json11 PROPERTIES FOLDER "ThirdParty/json11")

#---- �\�[�X�t�@�C����ǉ� ----
#���ӁF�t�@�C�����������o�ł��Ȃ����߁A�t�@�C����ύX����ꍇ�͖����I�ɂ����ɋL�q���Ă��������B
file(GLOB_RECURSE HEADER_FILES "*.h")
file(GLOB_RECURSE SOURCE_FILES "*.cpp")

#�\�[�X�t�@�C�����O���[�v��
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${HEADER_FILES} ${SOURCE_FILES})

# ---- ���C�u�������쐬���� ----

# �w�b�_�[�t�@�C���݂̂̏ꍇ�A���ׂĂ�PUBLIC�t���O��INTERFACE�ɕύX���āA�C���^�[�t�F�[�X�^�[�Q�b�g���쐬���܂�
# target: add_library(${TARGET_NAME} INTERFACE)
add_library(${TARGET_NAME} ${HEADER_FILES} ${SOURCE_FILES})

set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD 17)
set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "Engine")

# �N���X�v���b�g�t�H�[���^�[�Q�b�g�ł��邽�߁AMSVC�ł̕W��������������܂�
target_compile_options(${TARGET_NAME} PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/permissive->")
target_compile_options(${TARGET_NAME} PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/WX->")

# �ˑ��֌W�������N����    
target_link_libraries(${TARGET_NAME} PUBLIC spdlog::spdlog)
target_link_libraries(${TARGET_NAME} PRIVATE tinyobjloader stb)
target_link_libraries(${TARGET_NAME} PUBLIC glfw)
target_link_libraries(${TARGET_NAME} PUBLIC imgui)
target_link_libraries(${TARGET_NAME} PUBLIC Jolt)
target_link_libraries(${TARGET_NAME} PUBLIC lua_static sol2)
target_link_libraries(${TARGET_NAME} PUBLIC ${vulkan_lib})
target_link_libraries(${TARGET_NAME} PRIVATE $<BUILD_INTERFACE:json11>)

if(ENABLE_PHYSICS_DEBUG_RENDERER)
  # �����f�o�b�O�����_�����O��L���ɂ���ꍇ�ɃR���p�C����`��ǉ�
  add_compile_definitions(ENABLE_PHYSICS_DEBUG_RENDERER)
  # TestFramework��d3d12.lib�Ashcore.lib�������N����
  target_link_libraries(${TARGET_NAME} PUBLIC TestFramework d3d12.lib shcore.lib)
endif()

# �r���h�C���^�[�t�F�[�X�f�B���N�g����vulkan_include��ǉ�����
target_include_directories(
  ${TARGET_NAME}
  PUBLIC $<BUILD_INTERFACE:${vulkan_include}>)

# SHADER_COMPILE_TARGET�̈ˑ��֌W��ǉ����AENGINE_ROOT_DIR/shader/generated/cpp���C���^�[�t�F�[�X�f�B���N�g���ɒǉ�����
add_dependencies(${TARGET_NAME} ${SHADER_COMPILE_TARGET})
target_include_directories(
  ${TARGET_NAME}
  PUBLIC $<BUILD_INTERFACE:${ENGINE_ROOT_DIR}/shader/generated/cpp>)

# �r���h�C���^�[�t�F�[�X�f�B���N�g����ENGINE_ROOT_DIR/source��include/TARGET_NAME-PROJECT_VERSION��ǉ�����
target_include_directories(
  ${TARGET_NAME} 
  PUBLIC $<BUILD_INTERFACE:${ENGINE_ROOT_DIR}/source>
  $<INSTALL_INTERFACE:include/${TARGET_NAME}-${PROJECT_VERSION}>
)

# �r���h�C���^�[�t�F�[�X�f�B���N�g����JSON_INCLUDE��ǉ�����
target_include_directories(
  ${TARGET_NAME} 
  PUBLIC $<BUILD_INTERFACE:${JSON_INCLUDE}>
)

# �r���h�C���^�[�t�F�[�X�f�B���N�g����THIRD_PARTY_DIR/vulkanmemoryallocator/include��ǉ�����
target_include_directories(
  ${TARGET_NAME} 
  PUBLIC $<BUILD_INTERFACE:${THIRD_PARTY_DIR}/vulkanmemoryallocator/include>
)

# �r���h�C���^�[�t�F�[�X�f�B���N�g����ENGINE_ROOT_DIR/source/runtime��include/TARGET_NAME-PROJECT_VERSION��ǉ�����
target_include_directories(
  ${TARGET_NAME} 
  PUBLIC $<BUILD_INTERFACE:${ENGINE_ROOT_DIR}/source/runtime>
  $<INSTALL_INTERFACE:include/${TARGET_NAME}-${PROJECT_VERSION}>
)

# �r���h�C���^�[�t�F�[�X�f�B���N�g����ENGINE_ROOT_DIR/source/runtime/function/render/include��include/TARGET_NAME-PROJECT_VERSION��ǉ�����
target_include_directories(
  ${TARGET_NAME} 
  PUBLIC $<BUILD_INTERFACE:${ENGINE_ROOT_DIR}/source/runtime/function/render/include>
  $<INSTALL_INTERFACE:include/${TARGET_NAME}-${PROJECT_VERSION}>
)

# ����ɂ��A���[�U�[�� `find_package()` ���g�p���ă��C�u�������C���X�g�[�����Č����ł���悤�ɂȂ�܂��B

# �v���W�F�N�g�̃o�[�W�����w�b�_�[���z�u�����ꏊ�́A�v���W�F�N�g�̒ʏ�̃w�b�_�[�p�X�Ɉ�v����K�v������܂�
string(TOLOWER ${TARGET_NAME}/version.h VERSION_HEADER_LOCATION)

### ���O�R���p�C��
# ���O�R���p�C���Ŏg�p�����O���[�o���ϐ���ݒ肵�܂�
set(LunarYue_RUNTIME_HEADS "${HEADER_FILES}" PARENT_SCOPE)