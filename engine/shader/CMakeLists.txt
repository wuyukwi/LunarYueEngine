# �^�[�Q�b�g����ݒ�
set(TARGET_NAME ${SHADER_COMPILE_TARGET})

# �V�F�[�_�[�t�@�C���̃p�X���擾
file(GLOB_RECURSE SHADER_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/glsl/*.vert"
                                            "${CMAKE_CURRENT_SOURCE_DIR}/glsl/*.frag"
                                            "${CMAKE_CURRENT_SOURCE_DIR}/glsl/*.comp"
                                            "${CMAKE_CURRENT_SOURCE_DIR}/glsl/*.geom"
                                            "${CMAKE_CURRENT_SOURCE_DIR}/glsl/*.tesc"
                                            "${CMAKE_CURRENT_SOURCE_DIR}/glsl/*.tese"
                                            "${CMAKE_CURRENT_SOURCE_DIR}/glsl/*.mesh"
                                            "${CMAKE_CURRENT_SOURCE_DIR}/glsl/*.task"
                                            "${CMAKE_CURRENT_SOURCE_DIR}/glsl/*.rgen"
                                            "${CMAKE_CURRENT_SOURCE_DIR}/glsl/*.rchit"
                                            "${CMAKE_CURRENT_SOURCE_DIR}/glsl/*.rmiss"
                                            "${CMAKE_CURRENT_SOURCE_DIR}/glsl/*.rcall")

# �V�F�[�_�[�t�@�C�����O���[�v��
SOURCE_GROUP("shaders" FILES ${SHADER_FILES})

# �V�F�[�_�[�̃C���N���[�h�t�H���_��ݒ�
set(SHADER_INCLUDE_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/include)

# �������ꂽ�V�F�[�_�[�t�@�C����ۑ�����t�H���_���쐬
set(GENERATED_SHADER_FOLDER "generated")
if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${GENERATED_SHADER_FOLDER})
  file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${GENERATED_SHADER_FOLDER})
endif()

# SPIR-V �o�C�i����ۑ�����t�H���_���쐬
if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${GENERATED_SHADER_FOLDER}/spv)
  file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${GENERATED_SHADER_FOLDER}/spv)
endif()

# ShaderCompile.cmake �t�@�C����ǂݍ��݁A�V�F�[�_�[�̃R���p�C�������s
include(${LunarYue_ROOT_DIR}/cmake/ShaderCompile.cmake)
compile_shader(
  "${SHADER_FILES}"
  "${TARGET_NAME}"
  "${SHADER_INCLUDE_FOLDER}"
  "${GENERATED_SHADER_FOLDER}"
  "${glslangValidator_executable}")

# �^�[�Q�b�g�̃v���p�e�B��ݒ�
set_target_properties("${TARGET_NAME}" PROPERTIES FOLDER "Engine" )