#include <fstream>
#include <iostream>

#include "editor/include/editor_asset_browser.h"

#include "editor/include/editor_global_context.h"

#include "function/ui/Plugins/ContextualMenu.h"
#include "function/ui/Plugins/DDSource.h"
#include "function/ui/Plugins/DDTarget.h"
#include "function/ui/Widgets/Buttons/Button.h"
#include "function/ui/Widgets/InputFields/InputText.h"
#include "function/ui/Widgets/Texts/TextClickable.h"
#include "function/ui/Widgets/Visual/Image.h"
#include "function/ui/Widgets/Visual/Separator.h"

#include "platform/file_service/file_service.h"
#include "platform/path/path.h"

#include "resource/asset_manager/asset_manager.h"
#include "resource/config_manager/config_manager.h"

#include "runtime/core/base/macro.h"

namespace LunarYue
{
    using namespace UI::Panels;
    using namespace UI::Widgets;

    const std::string AssetBrowser::FILENAMES_CHARS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ.-_=+ 0123456789()[]";

    std::string GetAssociatedMetaFile(const std::string& p_assetPath) { return p_assetPath + ".meta"; }

    void RenameAsset(const std::string& p_prev, const std::string& p_new)
    {
        std::filesystem::rename(p_prev, p_new);

        if (const std::string previousMetaPath = GetAssociatedMetaFile(p_prev); std::filesystem::exists(previousMetaPath))
        {
            if (const std::string newMetaPath = GetAssociatedMetaFile(p_new); !std::filesystem::exists(newMetaPath))
            {
                std::filesystem::rename(previousMetaPath, newMetaPath);
            }
            else
            {
                LOG_FATAL(newMetaPath + " is already existing, .meta creation failed");
            }
        }
    }

    void RemoveAsset(const std::string& p_toDelete)
    {
        std::filesystem::remove(p_toDelete);

        if (const std::string metaPath = GetAssociatedMetaFile(p_toDelete); std::filesystem::exists(metaPath))
        {
            std::filesystem::remove(metaPath);
        }
    }

    class TexturePreview : public UI::Plugins::IPlugin
    {
    public:
        TexturePreview() : image(0, {80, 80}) {}

        // void SetPath(const std::string& p_path) { texture = Core::Global::ServiceLocator::Get<Core::ResourceManagement::TextureManager>()[p_path];
        // }

        void Execute() override
        {
            if (ImGui::IsItemHovered())
            {
                // if (texture)
                //     image.textureID.id = texture->id;

                ImGui::BeginTooltip();
                image.Draw();
                ImGui::EndTooltip();
            }
        }

        // Rendering::Resources::Texture* texture = nullptr;
        Visual::Image image;
    };

    class BrowserItemContextualMenu : public UI::Plugins::ContextualMenu
    {
    public:
        BrowserItemContextualMenu(std::string p_filePath, bool p_protected = false) : m_protected(p_protected), filePath(std::move(p_filePath)) {}

        virtual void CreateList()
        {
            if (!m_protected)
            {
                auto& renameMenu   = CreateWidget<Menu::MenuList>("Rename to...");
                auto& deleteAction = CreateWidget<Menu::MenuItem>("Delete");

                auto& nameEditor            = renameMenu.CreateWidget<InputFields::InputText>("");
                nameEditor.selectAllOnClick = true;

                renameMenu.ClickedEvent += [this, &nameEditor] {
                    nameEditor.content = Path::getElementName(filePath);

                    if (!std::filesystem::is_directory(filePath))
                        if (size_t pos = nameEditor.content.rfind('.'); pos != std::string::npos)
                            nameEditor.content = nameEditor.content.substr(0, pos);
                };

                deleteAction.ClickedEvent += [this] { DeleteItem(); };

                nameEditor.EnterPressedEvent += [this](std::string p_newName) {
                    if (!std::filesystem::is_directory(filePath))
                        p_newName += '.' + Path::getExtension(filePath);

                    /* Clean the name (Remove special chars) */
                    p_newName.erase(std::remove_if(p_newName.begin(),
                                                   p_newName.end(),
                                                   [](auto& c) {
                                                       return std::find(AssetBrowser::FILENAMES_CHARS.begin(),
                                                                        AssetBrowser::FILENAMES_CHARS.end(),
                                                                        c) == AssetBrowser::FILENAMES_CHARS.end();
                                                   }),
                                    p_newName.end());

                    std::string containingFolderPath = Path::getParentFolder(filePath);
                    std::string newPath              = containingFolderPath + p_newName;
                    std::string oldPath              = filePath;

                    if (filePath != newPath && !std::filesystem::exists(newPath))
                        filePath = newPath;

                    if (std::filesystem::is_directory(oldPath))
                        filePath += '/';

                    RenamedEvent.Invoke(oldPath, newPath);
                };
            }
        }

        void Execute() override
        {
            if (m_widgets.size() > 0)
                ContextualMenu::Execute();
        }

        virtual void DeleteItem() = 0;

    public:
        bool                                      m_protected;
        std::string                               filePath;
        Eventing::Event<std::string>              DestroyedEvent;
        Eventing::Event<std::string, std::string> RenamedEvent;
    };

    class FolderContextualMenu : public BrowserItemContextualMenu
    {
    public:
        FolderContextualMenu(const std::string& p_filePath, bool p_protected = false) : BrowserItemContextualMenu(p_filePath, p_protected) {}

        bool ImportAssetAtLocation(const std::string& p_destination) const
        {
            g_runtime_global_context.m_file_system->show();
            if (g_runtime_global_context.m_file_system->hasSucceeded())
            {
                std::string source      = g_runtime_global_context.m_file_system->getSelectedFilePath();
                std::string destination = p_destination + g_runtime_global_context.m_file_system->getSelectedFileName();

                if (!std::filesystem::exists(destination))
                {
                    std::filesystem::copy(source, destination, std::filesystem::copy_options::skip_existing);
                    LOG_INFO("Asset \"" + destination + "\" imported");
                    return true;
                }
            }

            return false;
        }

        void CreateList() override
        {
            auto& showInExplorer = CreateWidget<Menu::MenuItem>("Show in explorer");
            showInExplorer.ClickedEvent += [this] { FileSystem::ShowInExplorer(filePath); };

            if (!m_protected)
            {
                auto& importAssetHere = CreateWidget<Menu::MenuItem>("Import Here...");
                importAssetHere.ClickedEvent += [this] {
                    if (ImportAssetAtLocation(filePath))
                    {
                        auto* pluginOwner = static_cast<Layout::TreeNode*>(userData);
                        pluginOwner->Open();
                    }
                };

                auto& createMenu = CreateWidget<Menu::MenuList>("Create..");

                auto& createFolderMenu   = createMenu.CreateWidget<Menu::MenuList>("Folder");
                auto& createLevelMenu    = createMenu.CreateWidget<Menu::MenuList>("Level");
                auto& createObjectMenu   = createMenu.CreateWidget<Menu::MenuList>("Object");
                auto& createMaterialMenu = createMenu.CreateWidget<Menu::MenuList>("Material");

                auto& createEmptyMaterialMenu = createMaterialMenu.CreateWidget<Menu::MenuList>("Empty");

                auto& createFolder = createFolderMenu.CreateWidget<InputFields::InputText>("");
                auto& createLevel  = createLevelMenu.CreateWidget<InputFields::InputText>("");
                auto& createObject = createObjectMenu.CreateWidget<InputFields::InputText>("");

                auto& createEmptyMaterial = createEmptyMaterialMenu.CreateWidget<InputFields::InputText>("");

                createFolderMenu.ClickedEvent += [&createFolder] { createFolder.content = ""; };
                createLevelMenu.ClickedEvent += [&createLevel] { createLevel.content = ""; };
                createObjectMenu.ClickedEvent += [&createObject] { createObject.content = ""; };
                createEmptyMaterialMenu.ClickedEvent += [&createEmptyMaterial] { createEmptyMaterial.content = ""; };

                createFolder.EnterPressedEvent += [this](std::string newFolderName) {
                    size_t      fails = 0;
                    std::string finalPath;

                    do
                    {
                        finalPath = filePath + (!fails ? newFolderName : newFolderName + " (" + std::to_string(fails) + ')');

                        ++fails;
                    } while (std::filesystem::exists(finalPath));

                    std::filesystem::create_directory(finalPath);

                    ItemAddedEvent.Invoke(finalPath);
                    Close();
                };

                createLevel.EnterPressedEvent += [this](std::string newLevelName) {
                    size_t      fails = 0;
                    std::string finalPath;

                    do
                    {
                        finalPath = filePath + (!fails ? newLevelName : newLevelName + " (" + std::to_string(fails) + ')') + ".level";

                        ++fails;
                    } while (std::filesystem::exists(finalPath));

                    std::ofstream outfile(finalPath);
                    outfile << R"({" gravity ":{" x " : 0, " y " : 0, " z " : -15}} )" << std::endl;
                    ItemAddedEvent.Invoke(finalPath);
                    Close();
                };

                createEmptyMaterial.EnterPressedEvent += [this](std::string materialName) {
                    size_t      fails = 0;
                    std::string finalPath;

                    do
                    {
                        finalPath = filePath + (!fails ? materialName : materialName + " (" + std::to_string(fails) + ')') + ".material";

                        ++fails;
                    } while (std::filesystem::exists(finalPath));

                    {
                        std::ofstream outfile(finalPath);
                        outfile << R"({
                                "base_colour_texture_file" : "asset/textures/gold.tga",
                                "metallic_roughness_texture_file" : "asset/textures/mr.tga",
                                "normal_texture_file" : "asset/textures/n.tga",
                                "occlusion_texture_file" : "",
                                 "emissive_texture_file" : ""}
                        )" << std::endl; // Empty material content
                    }

                    ItemAddedEvent.Invoke(finalPath);

                    /*  if (auto instance = EDITOR_CONTEXT(materialManager)[EDITOR_EXEC(GetResourcePath(finalPath))])
                      {
                          auto& materialEditor = EDITOR_PANEL(MaterialEditor, "Material Editor");
                          materialEditor.SetTarget(*instance);
                          materialEditor.Open();
                          materialEditor.Focus();
                          materialEditor.Preview();
                      }*/
                    Close();
                };

                BrowserItemContextualMenu::CreateList();
            }
        }

        void DeleteItem() override {}

    public:
        Eventing::Event<std::string> ItemAddedEvent;
    };

    class ScriptFolderContextualMenu : public FolderContextualMenu
    {
    public:
        ScriptFolderContextualMenu(const std::string& p_filePath, bool p_protected = false) : FolderContextualMenu(p_filePath, p_protected) {}

        void CreateScript(const std::string& p_name, const std::string& p_path)
        {
            std::string fileContent = "local " + p_name + " =\n{\n}\n\nfunction " + p_name + ":OnStart()\nend\n\nfunction " + p_name +
                                      ":OnUpdate(deltaTime)\nend\n\nreturn " + p_name;

            std::ofstream outfile(p_path);
            outfile << fileContent << std::endl; // Empty scene content

            ItemAddedEvent.Invoke(p_path);
            Close();
        }

        void CreateList() override
        {
            FolderContextualMenu::CreateList();

            auto& newScriptMenu = CreateWidget<Menu::MenuList>("New script...");
            auto& nameEditor    = newScriptMenu.CreateWidget<InputFields::InputText>("");

            newScriptMenu.ClickedEvent += [this, &nameEditor] { nameEditor.content = Path::getElementName(""); };

            nameEditor.EnterPressedEvent += [this](std::string p_newName) {
                /* Clean the name (Remove special chars) */
                p_newName.erase(std::remove_if(p_newName.begin(),
                                               p_newName.end(),
                                               [](auto& c) {
                                                   return std::find(AssetBrowser::FILENAMES_CHARS.begin(), AssetBrowser::FILENAMES_CHARS.end(), c) ==
                                                          AssetBrowser::FILENAMES_CHARS.end();
                                               }),
                                p_newName.end());

                const std::string newPath = filePath + p_newName + ".lua";

                if (!std::filesystem::exists(newPath))
                {
                    CreateScript(p_newName, newPath);
                }
            };
        }
    };

    class FileContextualMenu : public BrowserItemContextualMenu
    {
    public:
        FileContextualMenu(const std::string& p_filePath, bool p_protected = false) : BrowserItemContextualMenu(p_filePath, p_protected) {}

        void CreateList() override
        {
            auto& editAction = CreateWidget<Menu::MenuItem>("Open");

            // editAction.ClickedEvent += [this] { Utils::SystemCalls::OpenFile(filePath); };

            if (!m_protected)
            {
                auto& duplicateAction = CreateWidget<Menu::MenuItem>("Duplicate");

                duplicateAction.ClickedEvent += [this] {
                    std::string filePathWithoutExtension = filePath;

                    if (size_t pos = filePathWithoutExtension.rfind('.'); pos != std::string::npos)
                        filePathWithoutExtension = filePathWithoutExtension.substr(0, pos);

                    std::string extension = "." + Path::getExtension(filePath);

                    auto filenameAvailable = [&extension](const std::string& target) { return !std::filesystem::exists(target + extension); };

                    // const auto newNameWithoutExtension = Utils::String::GenerateUnique(filePathWithoutExtension, filenameAvailable);

                    // std::string finalPath = newNameWithoutExtension + extension;
                    // std::filesystem::copy(filePath, finalPath);

                    // DuplicateEvent.Invoke(finalPath);
                };
            }

            BrowserItemContextualMenu::CreateList();

            auto& editMetadata = CreateWidget<Menu::MenuItem>("Properties");

            // editMetadata.ClickedEvent += [this] {
            //     auto&       panel        = EDITOR_PANEL(AssetProperties, "Asset Properties");
            //     std::string resourcePath = EDITOR_EXEC(GetResourcePath(filePath, m_protected));
            //     panel.SetTarget(resourcePath);
            //     panel.Open();
            //     panel.Focus();
            // };
        }

        void DeleteItem() override
        {
            // using namespace Windowing::Dialogs;
            // MessageBox message("Delete file",
            //                    "Deleting a file is irreversible, are you sure that you want to delete \"" + filePath + "\"?",
            //                    MessageBox::EMessageType::WARNING,
            //                    MessageBox::EButtonLayout::YES_NO);

            // if (message.GetUserAction() == MessageBox::EUserAction::YES)
            //{
            //     RemoveAsset(filePath);
            //     DestroyedEvent.Invoke(filePath);
            //     EDITOR_EXEC(PropagateFileRename(filePath, "?"));
            // }
        }

    public:
        Eventing::Event<std::string> DuplicateEvent;
    };

    template<typename Resource, typename ResourceLoader>
    class PreviewableContextualMenu : public FileContextualMenu
    {
    public:
        PreviewableContextualMenu(const std::string& p_filePath, bool p_protected = false) : FileContextualMenu(p_filePath, p_protected) {}

        void CreateList() override
        {
            auto& previewAction = CreateWidget<Menu::MenuItem>("Preview");

            // previewAction.ClickedEvent += [this] {
            //     Resource* resource  = Core::Global::ServiceLocator::Get<ResourceLoader>()[EDITOR_EXEC(GetResourcePath(filePath, m_protected))];
            //     auto&     assetView = EDITOR_PANEL(AssetView, "Asset View");
            //     assetView.SetResource(resource);
            //     assetView.Open();
            //     assetView.Focus();
            // };

            FileContextualMenu::CreateList();
        }
    };

    class ShaderContextualMenu : public FileContextualMenu
    {
    public:
        ShaderContextualMenu(const std::string& p_filePath, bool p_protected = false) : FileContextualMenu(p_filePath, p_protected) {}

        void CreateList() override
        {
            FileContextualMenu::CreateList();

            auto& compileAction = CreateWidget<Menu::MenuItem>("Compile");

            // compileAction.ClickedEvent += [this] {
            //     auto&       shaderManager = OVSERVICE(Core::ResourceManagement::ShaderManager);
            //     std::string resourcePath  = EDITOR_EXEC(GetResourcePath(filePath, m_protected));
            //     if (shaderManager.IsResourceRegistered(resourcePath))
            //     {
            //         /* Trying to recompile */
            //         Rendering::Resources::Loaders::ShaderLoader::Recompile(*shaderManager[resourcePath], filePath);
            //     }
            //     else
            //     {
            //         /* Trying to compile */
            //         Rendering::Resources::Shader* shader = OVSERVICE(Core::ResourceManagement::ShaderManager)[resourcePath];
            //         if (shader)
            //             OVLOG_INFO("[COMPILE] \"" + filePath + "\": Success!");
            //     }
            // };
        }
    };

    // class ModelContextualMenu : public PreviewableContextualMenu<Rendering::Resources::Model, Core::ResourceManagement::ModelManager>
    //{
    // public:
    //     ModelContextualMenu(const std::string& p_filePath, bool p_protected = false) : PreviewableContextualMenu(p_filePath, p_protected) {}

    //    virtual void CreateList() override
    //    {
    //        auto& reloadAction = CreateWidget<UI::Widgets::Menu::MenuItem>("Reload");

    //        reloadAction.ClickedEvent += [this] {
    //            auto&       modelManager = OVSERVICE(Core::ResourceManagement::ModelManager);
    //            std::string resourcePath = EDITOR_EXEC(GetResourcePath(filePath, m_protected));
    //            if (modelManager.IsResourceRegistered(resourcePath))
    //            {
    //                modelManager.AResourceManager::ReloadResource(resourcePath);
    //            }
    //        };

    //        if (!m_protected)
    //        {
    //            auto& generateMaterialsMenu = CreateWidget<UI::Widgets::Menu::MenuList>("Generate materials...");

    //            generateMaterialsMenu.CreateWidget<UI::Widgets::Menu::MenuItem>("Standard").ClickedEvent += [this] {
    //                auto&       modelManager = OVSERVICE(Core::ResourceManagement::ModelManager);
    //                std::string resourcePath = EDITOR_EXEC(GetResourcePath(filePath, m_protected));
    //                if (auto model = modelManager.GetResource(resourcePath))
    //                {
    //                    for (const std::string& materialName : model->GetMaterialNames())
    //                    {
    //                        size_t      fails = 0;
    //                        std::string finalPath;

    //                        do
    //                        {
    //                            finalPath = Path::GetContainingFolder(filePath) +
    //                                        (!fails ? materialName : materialName + " (" + std::to_string(fails) + ')') + ".ovmat";

    //                            ++fails;
    //                        } while (std::filesystem::exists(finalPath));

    //                        {
    //                            std::ofstream outfile(finalPath);
    //                            outfile << "<root><shader>:Shaders\\Standard.glsl</shader></root>" << std::endl; // Empty standard material content
    //                        }

    //                        DuplicateEvent.Invoke(finalPath);
    //                    }
    //                }
    //            };

    //            generateMaterialsMenu.CreateWidget<UI::Widgets::Menu::MenuItem>("StandardPBR").ClickedEvent += [this] {
    //                auto&       modelManager = OVSERVICE(Core::ResourceManagement::ModelManager);
    //                std::string resourcePath = EDITOR_EXEC(GetResourcePath(filePath, m_protected));
    //                if (auto model = modelManager.GetResource(resourcePath))
    //                {
    //                    for (const std::string& materialName : model->GetMaterialNames())
    //                    {
    //                        size_t      fails = 0;
    //                        std::string finalPath;

    //                        do
    //                        {
    //                            finalPath = Path::GetContainingFolder(filePath) +
    //                                        (!fails ? materialName : materialName + " (" + std::to_string(fails) + ')') + ".ovmat";

    //                            ++fails;
    //                        } while (std::filesystem::exists(finalPath));

    //                        {
    //                            std::ofstream outfile(finalPath);
    //                            outfile << "<root><shader>:Shaders\\StandardPBR.glsl</shader></root>" << std::endl; // Empty standard material
    //                            content
    //                        }

    //                        DuplicateEvent.Invoke(finalPath);
    //                    }
    //                }
    //            };

    //            generateMaterialsMenu.CreateWidget<UI::Widgets::Menu::MenuItem>("Unlit").ClickedEvent += [this] {
    //                auto&       modelManager = OVSERVICE(Core::ResourceManagement::ModelManager);
    //                std::string resourcePath = EDITOR_EXEC(GetResourcePath(filePath, m_protected));
    //                if (auto model = modelManager.GetResource(resourcePath))
    //                {
    //                    for (const std::string& materialName : model->GetMaterialNames())
    //                    {
    //                        size_t      fails = 0;
    //                        std::string finalPath;

    //                        do
    //                        {
    //                            finalPath = Path::GetContainingFolder(filePath) +
    //                                        (!fails ? materialName : materialName + " (" + std::to_string(fails) + ')') + ".ovmat";

    //                            ++fails;
    //                        } while (std::filesystem::exists(finalPath));

    //                        {
    //                            std::ofstream outfile(finalPath);
    //                            outfile << "<root><shader>:Shaders\\Unlit.glsl</shader></root>" << std::endl; // Empty standard material content
    //                        }

    //                        DuplicateEvent.Invoke(finalPath);
    //                    }
    //                }
    //            };

    //            generateMaterialsMenu.CreateWidget<UI::Widgets::Menu::MenuItem>("Lambert").ClickedEvent += [this] {
    //                auto&       modelManager = OVSERVICE(Core::ResourceManagement::ModelManager);
    //                std::string resourcePath = EDITOR_EXEC(GetResourcePath(filePath, m_protected));
    //                if (auto model = modelManager.GetResource(resourcePath))
    //                {
    //                    for (const std::string& materialName : model->GetMaterialNames())
    //                    {
    //                        size_t      fails = 0;
    //                        std::string finalPath;

    //                        do
    //                        {
    //                            finalPath = Path::GetContainingFolder(filePath) +
    //                                        (!fails ? materialName : materialName + " (" + std::to_string(fails) + ')') + ".ovmat";

    //                            ++fails;
    //                        } while (std::filesystem::exists(finalPath));

    //                        {
    //                            std::ofstream outfile(finalPath);
    //                            outfile << "<root><shader>:Shaders\\Lambert.glsl</shader></root>" << std::endl; // Empty standard material content
    //                        }

    //                        DuplicateEvent.Invoke(finalPath);
    //                    }
    //                }
    //            };
    //        }

    //        PreviewableContextualMenu::CreateList();
    //    }
    //};

    // class TextureContextualMenu : public PreviewableContextualMenu<Rendering::Resources::Texture, Core::ResourceManagement::TextureManager>
    //{
    // public:
    //     TextureContextualMenu(const std::string& p_filePath, bool p_protected = false) : PreviewableContextualMenu(p_filePath, p_protected) {}

    //    virtual void CreateList() override
    //    {
    //        auto& reloadAction = CreateWidget<UI::Widgets::Menu::MenuItem>("Reload");

    //        reloadAction.ClickedEvent += [this] {
    //            auto&       textureManager = OVSERVICE(Core::ResourceManagement::TextureManager);
    //            std::string resourcePath   = EDITOR_EXEC(GetResourcePath(filePath, m_protected));
    //            if (textureManager.IsResourceRegistered(resourcePath))
    //            {
    //                /* Trying to recompile */
    //                textureManager.AResourceManager::ReloadResource(resourcePath);
    //                EDITOR_PANEL(MaterialEditor, "Material Editor").Refresh();
    //            }
    //        };

    //        PreviewableContextualMenu::CreateList();
    //    }
    //};

    class SceneContextualMenu : public FileContextualMenu
    {
    public:
        SceneContextualMenu(const std::string& p_filePath, bool p_protected = false) : FileContextualMenu(p_filePath, p_protected) {}

        void CreateList() override
        {
            auto& editAction = CreateWidget<Menu::MenuItem>("Edit");

            // editAction.ClickedEvent += [this] { EDITOR_EXEC(LoadSceneFromDisk(EDITOR_EXEC(GetResourcePath(filePath)))); };

            FileContextualMenu::CreateList();
        }
    };

    /*  class MaterialContextualMenu : public PreviewableContextualMenu<Core::Resources::Material, Core::ResourceManagement::MaterialManager>
      {
      public:
          MaterialContextualMenu(const std::string& p_filePath, bool p_protected = false) : PreviewableContextualMenu(p_filePath, p_protected) {}

          virtual void CreateList() override
          {
              auto& editAction = CreateWidget<UI::Widgets::Menu::MenuItem>("Edit");

              editAction.ClickedEvent += [this] {
                  Core::Resources::Material* material =
                      OVSERVICE(Core::ResourceManagement::MaterialManager)[EDITOR_EXEC(GetResourcePath(filePath, m_protected))];
                  if (material)
                  {
                      auto& materialEditor = EDITOR_PANEL(MaterialEditor, "Material Editor");
                      materialEditor.SetTarget(*material);
                      materialEditor.Open();
                      materialEditor.Focus();

                      Core::Resources::Material* resource  =
      Core::Global::ServiceLocator::Get<Core::ResourceManagement::MaterialManager>()[EDITOR_EXEC( GetResourcePath(filePath, m_protected))]; auto&
      assetView = EDITOR_PANEL(AssetView, "Asset View"); assetView.SetResource(resource); assetView.Open(); assetView.Focus();
                  }
              };

              auto& reload = CreateWidget<UI::Widgets::Menu::MenuItem>("Reload");
              reload.ClickedEvent += [this] {
                  auto                       materialManager = OVSERVICE(Core::ResourceManagement::MaterialManager);
                  auto                       resourcePath    = EDITOR_EXEC(GetResourcePath(filePath, m_protected));
                  Core::Resources::Material* material        = materialManager[resourcePath];
                  if (material)
                  {
                      materialManager.AResourceManager::ReloadResource(resourcePath);
                      EDITOR_PANEL(MaterialEditor, "Material Editor").Refresh();
                  }
              };

              PreviewableContextualMenu::CreateList();
          }
      };*/

    AssetBrowser::AssetBrowser(const std::string& title, bool opened, const UI::Settings::PanelWindowSettings& windowSettings) :
        PanelWindow(title, opened, windowSettings)
    {
        m_engineAssetFolder   = g_runtime_global_context.m_config_manager->getAssetFolder();
        m_projectAssetFolder  = g_runtime_global_context.m_config_manager->getAssetFolder() / "project";
        m_projectScriptFolder = g_runtime_global_context.m_config_manager->getAssetFolder() / "script";

        if (!exists(m_engineAssetFolder))
        {
            std::filesystem::create_directories(m_projectAssetFolder);

            LOG_FATAL("Asset folder not found");
        }

        auto& refreshButton = CreateWidget<Buttons::Button>("Rescan assets");
        refreshButton.ClickedEvent += [this] { refresh(); };
        refreshButton.lineBreak           = false;
        refreshButton.idleBackgroundColor = {0.f, 0.5f, 0.0f};

        auto& importButton = CreateWidget<Buttons::Button>("Import asset");
        // importButton.ClickedEvent += EDITOR_BIND(ImportAsset, m_projectAssetFolder);
        importButton.idleBackgroundColor = {0.7f, 0.5f, 0.0f};

        m_assetList = &CreateWidget<Layout::Group>();

        fill();
    }

    void AssetBrowser::fill()
    {
        m_assetList->CreateWidget<Visual::Separator>();
        considerItem(nullptr, std::filesystem::directory_entry(m_engineAssetFolder), true);
        m_assetList->CreateWidget<UI::Widgets::Visual::Separator>();
        considerItem(nullptr, std::filesystem::directory_entry(m_projectAssetFolder), false);
        m_assetList->CreateWidget<UI::Widgets::Visual::Separator>();
        considerItem(nullptr, std::filesystem::directory_entry(m_projectScriptFolder), false, false, true);
    }

    void AssetBrowser::clear() const { m_assetList->RemoveAllWidgets(); }

    void AssetBrowser::refresh()
    {
        clear();
        fill();
    }

    void AssetBrowser::parseFolder(Layout::TreeNode& root, const std::filesystem::directory_entry& directory, bool isEngineItem, bool scriptFolder)
    {
        /* Iterates another time to display list files */
        for (auto& item : std::filesystem::directory_iterator(directory))
            if (item.is_directory())
                considerItem(&root, item, isEngineItem, false, scriptFolder);

        /* Iterates another time to display list files */
        for (auto& item : std::filesystem::directory_iterator(directory))
            if (!item.is_directory())
                considerItem(&root, item, isEngineItem, false, scriptFolder);
    }

    void AssetBrowser::considerItem(Layout::TreeNode*                       root,
                                    const std::filesystem::directory_entry& entry,
                                    bool                                    isEngineItem,
                                    bool                                    autoOpen,
                                    bool                                    scriptFolder)
    {
        const bool  isDirectory = entry.is_directory();
        std::string item_name   = entry.path().filename().generic_string();

        std::string path = entry.path().generic_string();
        if (isDirectory && path.back() != '/') // Add '\\' if is directory and backslash is missing
            path += '/';
        std::string resourceFormatPath = g_runtime_global_context.m_asset_manager->getRelativePath(path);
        bool        protectedItem      = !root || isEngineItem;

        const Path::EFileType fileType = Path::getFileType(item_name);

        // Unknown file, so we skip it
        if (fileType == Path::EFileType::UNKNOWN && !isDirectory)
        {
            return;
        }

        /* If there is a given treenode (p_root) we attach the new widget to it */
        auto& itemGroup = root ? root->CreateWidget<Layout::Group>() : m_assetList->CreateWidget<Layout::Group>();

        /* Find the icon to apply to the item */
        // uint32_t iconTextureID = 0;

        // itemGroup.CreateWidget<Visual::Image>(iconTextureID, Vector2 {16, 16}).lineBreak = false;

        /* If the entry is a directory, the content must be a tree node, otherwise (= is a file), a text will suffice */
        if (isDirectory)
        {
            auto& treeNode = itemGroup.CreateWidget<Layout::TreeNode>(item_name);

            if (autoOpen)
                treeNode.Open();

            auto& ddSource = treeNode.AddPlugin<UI::Plugins::DDSource<std::pair<std::string, Layout::Group*>>>(
                "Folder", resourceFormatPath, std::make_pair(resourceFormatPath, &itemGroup));

            if (!root || scriptFolder)
                treeNode.RemoveAllPlugins();

            auto& contextMenu    = !scriptFolder ? treeNode.AddPlugin<FolderContextualMenu>(path, protectedItem && !resourceFormatPath.empty()) :
                                                   treeNode.AddPlugin<ScriptFolderContextualMenu>(path, protectedItem && !resourceFormatPath.empty());
            contextMenu.userData = static_cast<void*>(&treeNode);

            contextMenu.ItemAddedEvent += [this, &treeNode, path, isEngineItem, scriptFolder](std::string p_string) {
                treeNode.Open();
                treeNode.RemoveAllWidgets();
                parseFolder(treeNode, std::filesystem::directory_entry(Path::getParentFolder(p_string)), isEngineItem, scriptFolder);
            };

            if (!scriptFolder)
            {
                if (!isEngineItem)
                /* Prevent engine item from being DDTarget (Can't Drag and drop to engine folder) */
                {
                    treeNode.AddPlugin<UI::Plugins::DDTarget<std::pair<std::string, Layout::Group*>>>("Folder").DataReceivedEvent +=
                        [this, &treeNode, path, isEngineItem](std::pair<std::string, Layout::Group*> p_data) {
                            if (!p_data.first.empty())
                            {
                                std::string folderReceivedPath = g_runtime_global_context.m_asset_manager->getRelativePath(p_data.first);

                                std::string folderName  = Path::getElementName(folderReceivedPath) + '\\';
                                std::string prevPath    = folderReceivedPath;
                                std::string correctPath = m_pathUpdate.find(&treeNode) != m_pathUpdate.end() ? m_pathUpdate.at(&treeNode) : path;
                                std::string newPath     = correctPath + folderName;

                                if (newPath.find(prevPath) == std::string::npos || prevPath == newPath)
                                {
                                    if (!std::filesystem::exists(newPath))
                                    {
                                        bool isEngineFolder = p_data.first.at(0) == ':';

                                        if (isEngineFolder) /* Copy dd folder from Engine resources */
                                            std::filesystem::copy(prevPath, newPath, std::filesystem::copy_options::recursive);
                                        else
                                        {
                                            RenameAsset(prevPath, newPath);
                                            // EDITOR_EXEC(PropagateFolderRename(prevPath, newPath));
                                        }

                                        treeNode.Open();
                                        treeNode.RemoveAllWidgets();
                                        parseFolder(treeNode, std::filesystem::directory_entry(correctPath), isEngineItem);

                                        if (!isEngineFolder)
                                            p_data.second->Destroy();
                                    }
                                    else if (prevPath == newPath)
                                    {
                                        // Ignore
                                    }
                                    else
                                    {
                                        LOG_ERROR("Folder already exists",
                                                  "You can't move this folder to this location because the name is already taken");
                                    }
                                }
                                else
                                {
                                    LOG_ERROR("WTF")
                                }
                            }
                        };

                    treeNode.AddPlugin<UI::Plugins::DDTarget<std::pair<std::string, Layout::Group*>>>("File").DataReceivedEvent +=
                        [this, &treeNode, path, isEngineItem](std::pair<std::string, Layout::Group*> p_data) {
                            if (!p_data.first.empty())
                            {
                                std::string fileReceivedPath = g_runtime_global_context.m_asset_manager->getFullPath(p_data.first).generic_string();

                                std::string fileName    = Path::getElementName(fileReceivedPath);
                                std::string prevPath    = fileReceivedPath;
                                std::string correctPath = m_pathUpdate.find(&treeNode) != m_pathUpdate.end() ? m_pathUpdate.at(&treeNode) : path;
                                std::string newPath     = correctPath + fileName;

                                if (!std::filesystem::exists(newPath))
                                {
                                    bool isEngineFile = p_data.first.at(0) == ':';

                                    if (isEngineFile) /* Copy dd file from Engine resources */
                                        std::filesystem::copy_file(prevPath, newPath);
                                    else
                                    {
                                        RenameAsset(prevPath, newPath);
                                        // EDITOR_EXEC(PropagateFileRename(prevPath, newPath));
                                    }

                                    treeNode.Open();
                                    treeNode.RemoveAllWidgets();
                                    parseFolder(treeNode, std::filesystem::directory_entry(correctPath), isEngineItem);

                                    if (!isEngineFile)
                                        p_data.second->Destroy();
                                }
                                else if (prevPath == newPath)
                                {
                                    // Ignore
                                }
                                else
                                {
                                    LOG_ERROR("Folder already exists",
                                              "You can't move this folder to this location because the name is already taken");
                                }
                            }
                        };
                }

                contextMenu.DestroyedEvent += [&itemGroup](std::string p_deletedPath) { itemGroup.Destroy(); };

                contextMenu.RenamedEvent += [this, &treeNode, path, &ddSource, isEngineItem](std::string p_prev, std::string p_newPath) {
                    p_newPath += '\\';

                    if (!std::filesystem::exists(p_newPath)) // Do not rename a folder if it already exists
                    {
                        RenameAsset(p_prev, p_newPath);
                        // EDITOR_EXEC(PropagateFolderRename(p_prev, p_newPath));
                        std::string elementName = Path::getElementName(p_newPath);
                        std::string data        = Path::getParentFolder(ddSource.data.first) + elementName + "\\";
                        ddSource.data.first     = data;
                        ddSource.tooltip        = data;
                        treeNode.name           = elementName;
                        treeNode.Open();
                        treeNode.RemoveAllWidgets();
                        parseFolder(treeNode, std::filesystem::directory_entry(p_newPath), isEngineItem);
                        m_pathUpdate[&treeNode] = p_newPath;
                    }
                    else
                    {
                        LOG_ERROR("Folder already exists", "You can't move this folder to this location because the name is already taken");
                    }
                };

                contextMenu.ItemAddedEvent += [this, &treeNode, isEngineItem](std::string p_path) {
                    treeNode.RemoveAllWidgets();
                    parseFolder(treeNode, std::filesystem::directory_entry(Path::getParentFolder(p_path)), isEngineItem);
                };
            }

            contextMenu.CreateList();

            treeNode.OpenedEvent += [this, &treeNode, path, isEngineItem, scriptFolder] {
                treeNode.RemoveAllWidgets();
                std::string updatedPath = Path::getParentFolder(path) + treeNode.name;
                parseFolder(treeNode, std::filesystem::directory_entry(updatedPath), isEngineItem, scriptFolder);
            };

            treeNode.ClosedEvent += [this, &treeNode] { treeNode.RemoveAllWidgets(); };
        }
        else
        {
            auto& clickableText = itemGroup.CreateWidget<Texts::TextClickable>(item_name);

            FileContextualMenu* contextMenu = nullptr;

            switch (fileType)
            {
                // case Path::EFileType::MODEL:
                //     contextMenu = &clickableText.AddPlugin<ModelContextualMenu>(path, protectedItem);
                //     break;
                // case Path::EFileType::TEXTURE:
                //     contextMenu = &clickableText.AddPlugin<TextureContextualMenu>(path, protectedItem);
                //     break;
                // case Path::EFileType::MATERIAL:
                //     contextMenu = &clickableText.AddPlugin<MaterialContextualMenu>(path, protectedItem);
                //     break;
                default:
                    contextMenu = &clickableText.AddPlugin<FileContextualMenu>(path, protectedItem);
                    break;
            }

            contextMenu->CreateList();

            contextMenu->DestroyedEvent += [&itemGroup](std::string p_deletedPath) {
                itemGroup.Destroy();

                // if (EDITOR_CONTEXT(sceneManager).GetCurrentSceneSourcePath() ==
                //     p_deletedPath) // Modify current scene source path if the renamed file is the current scene
                //     EDITOR_CONTEXT(sceneManager).ForgetCurrentSceneSourcePath();
            };

            auto& ddSource = clickableText.AddPlugin<UI::Plugins::DDSource<std::pair<std::string, Layout::Group*>>>(
                "File", resourceFormatPath, std::make_pair(resourceFormatPath, &itemGroup));

            contextMenu->RenamedEvent += [&ddSource, &clickableText, scriptFolder](std::string p_prev, std::string p_newPath) {
                if (p_newPath != p_prev)
                {
                    if (!std::filesystem::exists(p_newPath))
                    {
                        RenameAsset(p_prev, p_newPath);
                        std::string elementName = Path::getElementName(p_newPath);
                        ddSource.data.first     = Path::getParentFolder(ddSource.data.first) + elementName;

                        // if (!p_scriptFolder)
                        //{
                        //     EDITOR_EXEC(PropagateFileRename(p_prev, p_newPath));
                        //     if (EDITOR_CONTEXT(sceneManager).GetCurrentSceneSourcePath() ==
                        //         p_prev) // Modify current scene source path if the renamed file is the current scene
                        //         EDITOR_CONTEXT(sceneManager).StoreCurrentSceneSourcePath(p_newPath);
                        // }
                        // else
                        //{
                        //     EDITOR_EXEC(PropagateScriptRename(p_prev, p_newPath));
                        // }

                        clickableText.content = elementName;
                    }
                    else
                    {
                        LOG_ERROR("Folder already exists", "You can't move this folder to this location because the name is already taken");
                    }
                }
            };

            contextMenu->DuplicateEvent += [this, &clickableText, root, path, isEngineItem](std::string newItem) {
                // EDITOR_EXEC(DelayAction(
                //     std::bind(&AssetBrowser::considerItem, this, p_root, std::filesystem::directory_entry {newItem}, p_isEngineItem, false, false),
                //     0));
            };

            if (fileType == Path::EFileType::TEXTURE)
            {
                auto& texturePreview = clickableText.AddPlugin<TexturePreview>();
                // texturePreview.SetPath(resourceFormatPath);
            }
        }
    }
} // namespace LunarYue
