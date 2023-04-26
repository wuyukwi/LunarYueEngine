#pragma once

#include <filesystem>
#include <string>
#include <tuple>
#include <vector>

namespace LunarYue
{
    class Path
    {
    public:
        // �f�B���N�g���ƃt�@�C���p�X���瑊�΃p�X���擾����
        static std::filesystem::path getRelativePath(const std::filesystem::path& directory,
                                                     const std::filesystem::path& file_path);

        // �t�@�C���p�X����p�X�Z�O�����g���擾����
        static std::vector<std::string>
        getPathSegments(const std::filesystem::path& file_path);

        // �t�@�C���p�X����g���q���擾����
        static std::tuple<std::string, std::string, std::string> getFileExtensions(
            const std::filesystem::path& file_path);

        // �t�@�C��������g���q�������������Ȗ��O���擾����
        static std::string getFilePureName(const std::string&);
    };
} // namespace LunarYue