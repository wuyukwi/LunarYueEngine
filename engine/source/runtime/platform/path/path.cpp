#include "runtime/platform/path/path.h"

using namespace std;

namespace LunarYue
{
    filesystem::path Path::getRelativePath(const filesystem::path& directory, const filesystem::path& file_path)
    {
        return file_path.lexically_relative(directory);
    }

    vector<string> Path::getPathSegments(const filesystem::path& file_path)
    {
        vector<string> segments;
        for (const auto& segment : file_path)
        {
            segments.emplace_back(segment.generic_string());
        }

        return segments;
    }

    tuple<string, string, string> Path::getFileExtensions(const filesystem::path& file_path)
    {
        const auto extension           = file_path.extension().generic_string();
        const auto stem_extension      = file_path.stem().extension().generic_string();
        const auto stem_stem_extension = file_path.stem().stem().extension().generic_string();

        return make_tuple(extension, stem_extension, stem_stem_extension);
    }

    string Path::getFilePureName(const string& file_full_name)
    {
        const auto pos            = file_full_name.find_last_of('.');
        const auto file_pure_name = file_full_name.substr(0, pos);

        return file_pure_name;
    }

    std::string Path::makeWindowsStyle(const std::string& p_path)
    {
        std::string result = p_path;
        std::replace(result.begin(), result.end(), '/', '\\');
        return result;
    }

    std::string Path::makeNonWindowsStyle(const std::string& p_path)
    {
        std::string result = p_path;
        std::replace(result.begin(), result.end(), '\\', '/');
        return result;
    }

    std::string Path::getParentFolder(const filesystem::path& file_path)
    {
        const std::filesystem::path parent = file_path.parent_path().parent_path();
        return parent.string() + "/";
    }

    std::string Path::getElementName(const filesystem::path& file_path)
    {
        const filesystem::path filename = file_path.filename();
        return filename.generic_string();
    }

    std::string Path::getExtension(const std::string& file_path)
    {
        const size_t last_dot_pos = file_path.find_last_of('.');
        if (last_dot_pos != std::string::npos && last_dot_pos < file_path.length() - 1)
        {
            const size_t second_last_dot_pos = file_path.find_last_of('.', last_dot_pos - 1);
            if (second_last_dot_pos != std::string::npos && second_last_dot_pos < file_path.length() - 2)
            {
                std::string extension = file_path.substr(second_last_dot_pos + 1, last_dot_pos - second_last_dot_pos - 1);
                if (extension != "json")
                {
                    return extension;
                }
            }
        }
        else if (last_dot_pos == std::string::npos)
        {
            return "";
        }

        return file_path.substr(last_dot_pos);
    }

    std::string Path::fileTypeToString(EFileType p_fileType)
    {
        switch (p_fileType)
        {
            case EFileType::MODEL:
                return "Model";
            case EFileType::TEXTURE:
                return "Texture";
            case EFileType::MATERIAL:
                return "Material";
            case EFileType::SCRIPT:
                return "Script";
            case EFileType::FONT:
                return "Font";
            case EFileType::OBJECT:
                return "Object";
            case EFileType::LEVEL:
                return "Level";
            case EFileType::WORLD:
                return "world";
        }

        return "Unknown";
    }

    Path::EFileType Path::getFileType(const std::string& p_path)
    {
        std::string ext = getExtension(p_path);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

        if (ext == "fbx" || ext == "obj")
            return EFileType::MODEL;
        if (ext == "png" || ext == "jpeg" || ext == "jpg" || ext == "tga")
            return EFileType::TEXTURE;
        if (ext == "material")
            return EFileType::MATERIAL;
        if (ext == "lua")
            return EFileType::SCRIPT;
        if (ext == "ttf")
            return EFileType::FONT;
        if (ext == "object")
            return EFileType::OBJECT;
        if (ext == "level")
            return EFileType::LEVEL;
        if (ext == "world")
            return EFileType::WORLD;

        return EFileType::UNKNOWN;
    }
} // namespace LunarYue
