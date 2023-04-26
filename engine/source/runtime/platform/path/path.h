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
        // ディレクトリとファイルパスから相対パスを取得する
        static std::filesystem::path getRelativePath(const std::filesystem::path& directory,
                                                     const std::filesystem::path& file_path);

        // ファイルパスからパスセグメントを取得する
        static std::vector<std::string>
        getPathSegments(const std::filesystem::path& file_path);

        // ファイルパスから拡張子を取得する
        static std::tuple<std::string, std::string, std::string> getFileExtensions(
            const std::filesystem::path& file_path);

        // ファイル名から拡張子を除いた純粋な名前を取得する
        static std::string getFilePureName(const std::string&);
    };
} // namespace LunarYue