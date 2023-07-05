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
        enum class EFileType
        {
            UNKNOWN,
            MODEL,
            TEXTURE,
            MATERIAL,
            SCRIPT,
            FONT,
            OBJECT,
            LEVEL,
            WORLD
        };

        static std::filesystem::path getRelativePath(const std::filesystem::path& directory, const std::filesystem::path& file_path);
        static std::filesystem::path getRelativePath(const std::filesystem::path& file_path);

        static std::vector<std::string> getPathSegments(const std::filesystem::path& file_path);

        static std::tuple<std::string, std::string, std::string> getFileExtensions(const std::filesystem::path& file_path);

        static std::string getFilePureName(const std::string&);

        /**
         * Disabled constructor
         */
        Path() = delete;

        /**
         * Returns the windows style version of the given path ('/' replaced by '\')
         * @param p_path
         */
        static std::string makeWindowsStyle(const std::string& p_path);

        /**
         * Returns the non-windows style version of the given path ('\' replaced by '/')
         * @param p_path
         */
        static std::string makeNonWindowsStyle(const std::string& p_path);

        /**
         * Returns the containing folder of the file or folder identified by the given path
         * @param p_path
         */
        static std::string getParentFolder(const std::filesystem::path& file_path);

        /**
         * Returns the name of the file or folder identified by the given path
         * @param p_path
         */
        static std::string getElementName(const std::filesystem::path& file_path);

        /**
         * Returns the extension of the file or folder identified by the given path
         * @param p_path
         */
        static std::string getExtension(const std::string& file_path);

        /**
         * Convert the EFileType value to a string
         * @param p_fileType
         */
        static std::string fileTypeToString(EFileType p_fileType);

        /**
         * Returns the file type of the file identified by the given path
         * @param p_path
         */
        static EFileType getFileType(const std::string& p_path);
    };
} // namespace LunarYue