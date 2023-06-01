#pragma once

namespace LunarYue
{
    ///**
    // * The Context handle the engine features setup
    // */
    // class Context
    //{
    // public:
    //    /**
    //     * Constructor
    //     * @param p_projectPath
    //     * @param p_projectName
    //     */
    //    Context(const std::string& p_projectPath, const std::string& p_projectName);

    //    /**
    //     * Destructor
    //     */
    //    ~Context();

    //    /**
    //     * Reset project settings ini file
    //     */
    //    void ResetProjectSettings();

    //    /**
    //     * Verify that project settings are complete (No missing key).
    //     * Returns true if the integrity is verified
    //     */
    //    bool IsProjectSettingsIntegrityVerified();

    //    /**
    //     * Apply project settings to the ini file
    //     */
    //    void ApplyProjectSettings();

    // public:
    //     const std::string projectPath;
    //     const std::string projectName;
    //     const std::string projectFilePath;
    //     const std::string engineAssetsPath;
    //     const std::string projectAssetsPath;
    //     const std::string projectScriptsPath;
    //     const std::string editorAssetsPath;

    //    std::unique_ptr<OvWindowing::Context::Device>      device;
    //    std::unique_ptr<OvWindowing::Window>               window;
    //    std::unique_ptr<OvWindowing::Inputs::InputManager> inputManager;
    //    std::unique_ptr<OvRendering::Context::Driver>      driver;
    //    std::unique_ptr<OvCore::ECS::Renderer>             renderer;
    //    std::unique_ptr<OvRendering::Core::ShapeDrawer>    shapeDrawer;
    //    std::unique_ptr<OvUI::Core::UIManager>             uiManager;
    //    std::unique_ptr<OvPhysics::Core::PhysicsEngine>    physicsEngine;
    //    std::unique_ptr<OvAudio::Core::AudioEngine>        audioEngine;
    //    std::unique_ptr<OvAudio::Core::AudioPlayer>        audioPlayer;
    //    std::unique_ptr<OvEditor::Core::EditorResources>   editorResources;

    //    std::unique_ptr<OvCore::Scripting::ScriptInterpreter> scriptInterpreter;
    //    std::unique_ptr<OvRendering::Buffers::UniformBuffer>  engineUBO;

    //    std::unique_ptr<OvRendering::Buffers::ShaderStorageBuffer> lightSSBO;
    //    std::unique_ptr<OvRendering::Buffers::ShaderStorageBuffer> simulatedLightSSBO;

    //    OvCore::SceneSystem::SceneManager sceneManager;

    //    OvCore::ResourceManagement::ModelManager    modelManager;
    //    OvCore::ResourceManagement::TextureManager  textureManager;
    //    OvCore::ResourceManagement::ShaderManager   shaderManager;
    //    OvCore::ResourceManagement::MaterialManager materialManager;
    //    OvCore::ResourceManagement::SoundManager    soundManager;

    //    OvWindowing::Settings::WindowSettings windowSettings;

    //    OvTools::Filesystem::IniFile projectSettings;
    //};
} // namespace LunarYue
