#include "editor/include/editor_launcher.h"

#include <filesystem>

#include "editor/include/editor_global_context.h"

#include "function/ui/Widgets/Buttons/Button.h"
#include "function/ui/Widgets/InputFields/InputText.h"
#include "function/ui/Widgets/Layout/Columns.h"
#include "function/ui/Widgets/Layout/Spacing.h"
#include "function/ui/Widgets/Texts/Text.h"
#include "function/ui/Widgets/Visual/Separator.h"

#include "platform/path/path.h"

#include "runtime/engine.h"

namespace LunarYue
{
    class ProjectHubPanel : public UI::Panels::PanelWindow
    {
    public:
        ProjectHubPanel(bool& p_readyToGo, std::string& p_path, std::string& p_projectName) :
            PanelWindow("erload - Project Hub", true), m_readyToGo(p_readyToGo), m_path(p_path), m_projectName(p_projectName)
        {
            resizable = false;
            movable   = false;
            titleBar  = false;

            std::filesystem::create_directories(std::string(getenv("APPDATA")) + "\\erloadTech\\Editor\\");

            SetSize({1000, 580});
            SetPosition({0.f, 0.f});

            auto& openProjectButton = CreateWidget<UI::Widgets::Buttons::Button>("Open Project");
            auto& newProjectButton  = CreateWidget<UI::Widgets::Buttons::Button>("New Project");
            auto& pathField         = CreateWidget<UI::Widgets::InputFields::InputText>("");
            m_goButton              = &CreateWidget<UI::Widgets::Buttons::Button>("GO");

            pathField.ContentChangedEvent += [this, &pathField](std::string p_content) {
                pathField.content = Path::makeWindowsStyle(p_content);

                if (pathField.content != "" && pathField.content.back() != '\\')
                    pathField.content += '\\';

                UpdateGoButton(pathField.content);
            };

            UpdateGoButton("");

            openProjectButton.idleBackgroundColor = {0.7f, 0.5f, 0.f};
            newProjectButton.idleBackgroundColor  = {0.f, 0.5f, 0.0f};

            // openProjectButton.ClickedEvent += [this] {
            //     Windowing::Dialogs::OpenFileDialog dialog("Open project");
            //     dialog.AddFileType("erload Project", "*.ovproject");
            //     dialog.Show();

            //    std::string ovProjectPath  = dialog.GetSelectedFilePath();
            //    std::string rootFolderPath = Tools::Utils::PathParser::GetContainingFolder(ovProjectPath);

            //    if (dialog.HasSucceeded())
            //    {
            //        RegisterProject(rootFolderPath);
            //        OpenProject(rootFolderPath);
            //    }
            //};

            // newProjectButton.ClickedEvent += [this, &pathField] {
            //     Windowing::Dialogs::SaveFileDialog dialog("New project location");
            //     dialog.DefineExtension("erload Project", "..");
            //     dialog.Show();
            //     if (dialog.HasSucceeded())
            //     {
            //         std::string result = dialog.GetSelectedFilePath();
            //         pathField.content = std::string(result.data(), result.data() + result.size() - std::string("..").size()); // remove auto
            //         extension pathField.content += "\\"; UpdateGoButton(pathField.content);
            //     }
            // };

            m_goButton->ClickedEvent += [this, &pathField] {
                CreateProject(pathField.content);
                RegisterProject(pathField.content);
                OpenProject(pathField.content);
            };

            openProjectButton.lineBreak = false;
            newProjectButton.lineBreak  = false;
            pathField.lineBreak         = false;

            for (uint8_t i = 0; i < 4; ++i)
                CreateWidget<UI::Widgets::Layout::Spacing>();

            CreateWidget<UI::Widgets::Visual::Separator>();

            for (uint8_t i = 0; i < 4; ++i)
                CreateWidget<UI::Widgets::Layout::Spacing>();

            auto& columns = CreateWidget<UI::Widgets::Layout::Columns<2>>();

            columns.widths = {750, 500};

            std::string line;
            // std::ifstream myfile(PROJECTS_FILE);
            // if (myfile.is_open())
            //{
            //     while (getline(myfile, line))
            //     {
            //         if (std::filesystem::exists(line)) // TODO: Delete line from the file
            //         {
            //             auto& text         = columns.CreateWidget<UI::Widgets::Texts::Text>(line);
            //             auto& actions      = columns.CreateWidget<UI::Widgets::Layout::Group>();
            //             auto& openButton   = actions.CreateWidget<UI::Widgets::Buttons::Button>("Open");
            //             auto& deleteButton = actions.CreateWidget<UI::Widgets::Buttons::Button>("Delete");

            //            openButton.idleBackgroundColor   = {0.7f, 0.5f, 0.f};
            //            deleteButton.idleBackgroundColor = {0.5f, 0.f, 0.f};

            //            openButton.ClickedEvent += [this, line] { OpenProject(line); };

            //            std::string toErase = line;
            //            deleteButton.ClickedEvent += [this, &text, &actions, toErase] {
            //                text.Destroy();
            //                actions.Destroy();

            //                std::string   line;
            //                std::ifstream fin(PROJECTS_FILE);
            //                std::ofstream temp("temp");

            //                while (getline(fin, line))
            //                    if (line != toErase)
            //                        temp << line << std::endl;

            //                temp.close();
            //                fin.close();

            //                std::filesystem::remove(PROJECTS_FILE);
            //                std::filesystem::rename("temp", PROJECTS_FILE);
            //            };

            //            openButton.lineBreak = false;
            //            deleteButton.lineBreak;
            //        }
            //    }
            //    myfile.close();
            //}
        }

        void UpdateGoButton(const std::string& p_path)
        {
            bool validPath                  = p_path != "";
            m_goButton->idleBackgroundColor = validPath ? UI::Types::Color {0.f, 0.5f, 0.0f} : UI::Types::Color {0.1f, 0.1f, 0.1f};
            m_goButton->disabled            = !validPath;
        }

        void CreateProject(const std::string& p_path)
        {
            if (!std::filesystem::exists(p_path))
            {
                std::filesystem::create_directory(p_path);
                std::filesystem::create_directory(p_path + "Assets\\");
                std::filesystem::create_directory(p_path + "Scripts\\");
                /*  std::ofstream projectFile(p_path + '\\' + Path::getElementName(std::string(p_path.data(), p_path.data() + p_path.size() - 1)) +
                                            ".ovproject");*/
            }
        }

        void RegisterProject(const std::string& p_path)
        {
            bool pathAlreadyRegistered = false;

            //{
            //    std::string   line;
            //    std::ifstream myfile(PROJECTS_FILE);
            //    if (myfile.is_open())
            //    {
            //        while (getline(myfile, line))
            //        {
            //            if (line == p_path)
            //            {
            //                pathAlreadyRegistered = true;
            //                break;
            //            }
            //        }
            //        myfile.close();
            //    }
            //}

            // if (!pathAlreadyRegistered)
            //{
            //     std::ofstream projectsFile(PROJECTS_FILE, std::ios::app);
            //     projectsFile << p_path << std::endl;
            // }
        }

        void OpenProject(const std::string& p_path)
        {
            m_readyToGo = std::filesystem::exists(p_path);
            if (!m_readyToGo)
            {}
            else
            {
                m_path        = p_path;
                m_projectName = Path::getElementName(m_path);
                Close();
            }
        }

        void Draw() override
        {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {50.f, 50.f});
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);

            UI::Panels::PanelWindow::Draw();

            ImGui::PopStyleVar(2);
        }

    private:
        bool&                         m_readyToGo;
        std::string&                  m_path;
        std::string&                  m_projectName;
        UI::Widgets::Buttons::Button* m_goButton = nullptr;
    };

    EditorLauncher::EditorLauncher() {}

    void EditorLauncher::initialize()
    {
        m_mainPanel = std::make_unique<ProjectHubPanel>(m_readyToGo, m_projectPath, m_projectName);

        g_editor_global_context.m_ui_manager->SetCanvas(m_canvas);

        m_canvas.AddPanel(*m_mainPanel);
    }

    void EditorLauncher::preRender() { g_editor_global_context.m_ui_manager->Render(); }
} // namespace LunarYue
