#pragma once

#include <functional>
#include <string>

namespace LunarYue
{

    class FileSystem
    {
    public:
        void initializer();
#ifdef _WIN32
        void setIsSaveFile(bool isSave);

        void setTitle(const std::string& title);

        void setInitialDirectory(const std::string& initialDirectory);

        void addFileType(const std::string& filter);

        void show();

        bool hasSucceeded() const;

        std::string getSelectedFileName();

        std::string getSelectedFilePath();

        std::string getErrorInfo();

        bool isFileExisting() const;

        static void ShowInExplorer(const std::string& p_path);

        static void OpenFile(const std::string& p_file, const std::string& p_workingDir = "");

        static void EditFile(const std::string& p_file);

        static void OpenURL(const std::string& p_url);

    private:
        void handleError();

    protected:
        std::string m_dialogTitle;
        std::string m_initialDirectory;
        std::string m_filter;
        std::string m_error;
        std::string m_filename;
        std::string m_filepath;
        bool        m_succeeded = false;
        bool        m_isSave    = false;
    };
#endif
} // namespace LunarYue