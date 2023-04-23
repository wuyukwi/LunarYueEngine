# �r���h�Ώۂ̖��O��ݒ肵�܂�
set(TARGET_NAME LunarYueParser)

# �t�H���_�[���g�p����悤�ɃO���[�o���v���p�e�B��ݒ肵�܂�
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# �S�Ẵw�b�_�[�t�@�C�����������A���X�g�A�b�v���܂�
file(GLOB_RECURSE HEADERS "*.h")

# �S�Ẵ\�[�X�t�@�C�����������A���X�g�A�b�v���܂�
file(GLOB_RECURSE SOURCES "*.cpp")

# �\�[�X�O���[�v��ݒ肵�܂�
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${HEADERS} ${SOURCES})

# LLVM�̃C���N���[�h�f�B���N�g����ݒ肵�܂�
set(LLVM_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/3rd_party/LLVM/include ${CMAKE_CURRENT_SOURCE_DIR}/3rd_party/mustache ${CMAKE_CURRENT_SOURCE_DIR})

# �����[�X�r���h���̏o�͐��ݒ肵�܂�
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${ENGINE_ROOT_DIR}/bin)

# �f�o�b�O�r���h���̏o�͐��ݒ肵�܂�
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${ENGINE_ROOT_DIR}/bin)

# LLVM�̃C���N���[�h�f�B���N�g����ǉ����܂�
include_directories(${LLVM_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/parser)

# �w�b�_�[�t�@�C���͕K�v����܂��񂪁A�ꕔ��IDE�iVisual Studio�j�ł́A
# �\�[�X���X�g�ɒǉ�����Ȃ����߁A�ǉ�����K�v������܂�
add_executable(${TARGET_NAME} ${HEADERS} ${SOURCES})

# �^�[�Q�b�g�v���p�e�B��ݒ肵�܂�
set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD 17)
set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "Tools")

if (CMAKE_HOST_WIN32) # Windows�̏ꍇ
    set(LLVM_LIBRARY_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/3rd_party/LLVM/lib/x64)
    set(LLVM_SHARED_LIBRARY_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/3rd_party/LLVM/bin/x64)
    set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} /O2 /Ob2")
    target_link_libraries(${TARGET_NAME}  ${LLVM_LIBRARY_DIR}/libclang.lib)
elseif(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux") # Linux�̏ꍇ
    set(LLVM_LIBRARY_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/3rd_party/LLVM/lib/Linux)
    set(LLVM_SHARED_LIBRARY_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/3rd_party/LLVM/bin/Linux)
    set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O3")
    target_link_libraries(${TARGET_NAME}  ${LLVM_SHARED_LIBRARY_DIR}/libclang.so.12)
elseif(${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "arm64") # arm64�̏ꍇ
    # XCode Toolchains' universal binary libclang.dylib �Ƀ����N
    set(LLVM_LIBRARY_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/3rd_party/LLVM/lib/macOS)
    set(LLVM_SHARED_LIBRARY_DIR  ${OSX_DEVELOPER_ROOT}/Toolchains/XcodeDefault.xctoolchain/usr/lib)
    set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O3")
    target_link_libraries(${TARGET_NAME}  ${LLVM_SHARED_LIBRARY_DIR}/libclang.dylib)    
else() # macOS�̏ꍇ
    set(LLVM_LIBRARY_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/3rd_party/LLVM/lib/macOS)
    set(LLVM_SHARED_LIBRARY_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/3rd_party/LLVM/bin/macOS)
    set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O3")
    target_link_libraries(${TARGET_NAME}  ${LLVM_SHARED_LIBRARY_DIR}/libclang.dylib)
endif()



#Boost��LibClang��ÓI�Ƀ����N����
add_definitions(-DTIXML_USE_STL)

#�r���h��A���\�[�X���R�s�[����
add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
# mustache �e���v���[�g�f�B���N�g��
COMMAND ${CMAKE_COMMAND} -E copy_directory
"${LLVM_SHARED_LIBRARY_DIR}"
$<TARGET_FILE_DIR:${TARGET_NAME}>
)
