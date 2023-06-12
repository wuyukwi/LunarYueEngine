#include "runtime/platform/file_service/file_service.h"

#include <filesystem>

#ifdef _WIN32
#include <Windows.h>

namespace LunarYue
{
    void FileSystem::initializer()
    {
        m_filter += "Object (*.json)";
        m_filter += "Texture (*.png;*.jpeg;*.jpg;*.tga;)";
        m_filter += "Model (*.fbx;*.obj;)";

        m_dialogTitle = "Select an asset";
    }

    void FileSystem::setIsSaveFile(bool isSave) { m_isSave = isSave; }

    void FileSystem::setTitle(const std::string& title) { m_dialogTitle = title; }

    void FileSystem::setInitialDirectory(const std::string& initialDirectory) { m_initialDirectory = initialDirectory; }

    void FileSystem::addFileType(const std::string& filter) { m_filter += filter; }

    void FileSystem::show()
    {
        OPENFILENAME ofn;

        if (!m_initialDirectory.empty())
            m_filepath = m_initialDirectory;

        m_filepath.resize(MAX_PATH);

        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner   = nullptr; // If you have a window to center over, put its HANDLE here
        ofn.lpstrFilter = m_filter.c_str();
        ofn.lpstrFile   = m_filepath.data();
        ofn.nMaxFile    = MAX_PATH;
        ofn.lpstrTitle  = m_dialogTitle.c_str();

        if (!m_initialDirectory.empty())
            ofn.lpstrInitialDir = m_initialDirectory.c_str();

        ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR;

        if (m_isSave)
        {
            m_succeeded = GetSaveFileNameA(&ofn);
        }
        else
        {
            m_succeeded = GetOpenFileNameA(&ofn);
        }

        if (!m_succeeded)
            handleError();
        else
            m_filepath = m_filepath.c_str();

        /* Extract filename from filepath */
        m_filename.clear();
        for (auto it = m_filepath.rbegin(); it != m_filepath.rend() && *it != '\\' && *it != '/'; ++it)
            m_filename += *it;
        std::reverse(m_filename.begin(), m_filename.end());
    }

    bool FileSystem::hasSucceeded() const { return m_succeeded; }

    std::string FileSystem::getSelectedFileName() { return m_filename; }

    std::string FileSystem::getSelectedFilePath() { return m_filepath; }

    std::string FileSystem::getErrorInfo() { return m_error; }

    bool FileSystem::isFileExisting() const { return std::filesystem::exists(m_filepath); }

    void FileSystem::ShowInExplorer(const std::string& p_path) { ShellExecuteA(nullptr, "open", p_path.c_str(), nullptr, nullptr, SW_SHOWNORMAL); }

    void FileSystem::OpenFile(const std::string& p_file, const std::string& p_workingDir)
    {
        ShellExecuteA(nullptr, "open", p_file.c_str(), nullptr, p_workingDir.empty() ? nullptr : p_workingDir.c_str(), SW_SHOWNORMAL);
    }

    void FileSystem::EditFile(const std::string& p_file)
    {
        ShellExecuteW(nullptr, nullptr, std::wstring(p_file.begin(), p_file.end()).c_str(), nullptr, nullptr, SW_NORMAL);
    }

    void FileSystem::OpenURL(const std::string& p_url) { ShellExecute(nullptr, nullptr, p_url.c_str(), nullptr, nullptr, SW_SHOW); }

    void FileSystem::handleError()
    {
        switch (CommDlgExtendedError())
        {
            case CDERR_DIALOGFAILURE:
                m_error = "CDERR_DIALOGFAILURE";
                break;
            case CDERR_FINDRESFAILURE:
                m_error = "CDERR_FINDRESFAILURE";
                break;
            case CDERR_INITIALIZATION:
                m_error = "CDERR_INITIALIZATION";
                break;
            case CDERR_LOADRESFAILURE:
                m_error = "CDERR_LOADRESFAILURE";
                break;
            case CDERR_LOADSTRFAILURE:
                m_error = "CDERR_LOADSTRFAILURE";
                break;
            case CDERR_LOCKRESFAILURE:
                m_error = "CDERR_LOCKRESFAILURE";
                break;
            case CDERR_MEMALLOCFAILURE:
                m_error = "CDERR_MEMALLOCFAILURE";
                break;
            case CDERR_MEMLOCKFAILURE:
                m_error = "CDERR_MEMLOCKFAILURE";
                break;
            case CDERR_NOHINSTANCE:
                m_error = "CDERR_NOHINSTANCE";
                break;
            case CDERR_NOHOOK:
                m_error = "CDERR_NOHOOK";
                break;
            case CDERR_NOTEMPLATE:
                m_error = "CDERR_NOTEMPLATE";
                break;
            case CDERR_STRUCTSIZE:
                m_error = "CDERR_STRUCTSIZE";
                break;
            case FNERR_BUFFERTOOSMALL:
                m_error = "FNERR_BUFFERTOOSMALL";
                break;
            case FNERR_INVALIDFILENAME:
                m_error = "FNERR_INVALIDFILENAME";
                break;
            case FNERR_SUBCLASSFAILURE:
                m_error = "FNERR_SUBCLASSFAILURE";
                break;
            default:
                m_error = "You cancelled.";
        }
    }

} // namespace LunarYue

#endif