#include "function/event/event.h"
#include "function/ui/Internal/WidgetContainer.h"
#include "function/ui/Types/Color.h"
#include "function/ui/Widgets/Drags/DragSingleScalar.h"
#include "function/ui/Widgets/Texts/Text.h"
#include "function/ui/Widgets/Visual/Image.h"

namespace LunarYue
{
    /**
     * Provide some helpers to draw UI elements
     */
    class GUIDrawer
    {
    public:
        static const UI::Types::Color TitleColor;
        static const UI::Types::Color ClearButtonColor;

        static const float _MIN_FLOAT;
        static const float _MAX_FLOAT;

        /**
         * Defines the texture to use when there is no texture in a texture resource field
         * @param p_emptyTexture
         */
        // static void ProvideEmptyTexture(Rendering::Resources::Texture& p_emptyTexture);

        /**
         * Draw a title with the title color
         * @param p_root
         * @param p_name
         */
        static void CreateTitle(UI::Internal::WidgetContainer& p_root, const std::string& p_name);

        template<typename T>
        static void DrawScalar(UI::Internal::WidgetContainer& p_root,
                               const std::string&             p_name,
                               T&                             p_data,
                               float                          p_step = 1.f,
                               T                              p_min  = std::numeric_limits<T>::min(),
                               T                              p_max  = std::numeric_limits<T>::max());
        static void DrawBoolean(UI::Internal::WidgetContainer& p_root, const std::string& p_name, bool& p_data);
        static void DrawVec2(UI::Internal::WidgetContainer& p_root,
                             const std::string&             p_name,
                             Vector2&                       p_data,
                             float                          p_step = 1.f,
                             float                          p_min  = _MIN_FLOAT,
                             float                          p_max  = _MAX_FLOAT);
        static void DrawVec3(UI::Internal::WidgetContainer& p_root,
                             const std::string&             p_name,
                             Vector3&                       p_data,
                             float                          p_step = 1.f,
                             float                          p_min  = _MIN_FLOAT,
                             float                          p_max  = _MAX_FLOAT);
        static void DrawVec4(UI::Internal::WidgetContainer& p_root,
                             const std::string&             p_name,
                             Vector4&                       p_data,
                             float                          p_step = 1.f,
                             float                          p_min  = _MIN_FLOAT,
                             float                          p_max  = _MAX_FLOAT);
        static void DrawQuat(UI::Internal::WidgetContainer& p_root,
                             const std::string&             p_name,
                             Quaternion&                    p_data,
                             float                          p_step = 1.f,
                             float                          p_min  = _MIN_FLOAT,
                             float                          p_max  = _MAX_FLOAT);
        static void DrawString(UI::Internal::WidgetContainer& p_root, const std::string& p_name, std::string& p_data);
        static void DrawColor(UI::Internal::WidgetContainer& p_root, const std::string& p_name, UI::Types::Color& p_color, bool p_hasAlpha = false);
        // static UI::Widgets::Texts::Text&   DrawMesh(UI::Internal::WidgetContainer& p_root,
        //                                             const std::string&             p_name,
        //                                             Rendering::Resources::Model*&  p_data,
        //                                             Eventing::Event<>*             p_updateNotifier = nullptr);
        // static UI::Widgets::Visual::Image& DrawTexture(UI::Internal::WidgetContainer&  p_root,
        //                                                const std::string&              p_name,
        //                                                Rendering::Resources::Texture*& p_data,
        //                                                Eventing::Event<>*              p_updateNotifier = nullptr);
        // static UI::Widgets::Texts::Text&   DrawShader(UI::Internal::WidgetContainer& p_root,
        //                                               const std::string&             p_name,
        //                                               Rendering::Resources::Shader*& p_data,
        //                                               Eventing::Event<>*             p_updateNotifier = nullptr);
        // static UI::Widgets::Texts::Text&   DrawMaterial(UI::Internal::WidgetContainer& p_root,
        //                                                 const std::string&             p_name,
        //                                                 Core::Resources::Material*&    p_data,
        //                                                 Eventing::Event<>*             p_updateNotifier = nullptr);
        // static UI::Widgets::Texts::Text&   DrawSound(UI::Internal::WidgetContainer& p_root,
        //                                              const std::string&             p_name,
        //                                              Audio::Resources::Sound*&      p_data,
        //                                              Eventing::Event<>*             p_updateNotifier = nullptr);
        // static UI::Widgets::Texts::Text&   DrawAsset(UI::Internal::WidgetContainer& p_root,
        //                                              const std::string&             p_name,
        //                                              std::string&                   p_data,
        //                                              Eventing::Event<>*             p_updateNotifier = nullptr);

        template<typename T>
        static void DrawScalar(UI::Internal::WidgetContainer& p_root,
                               const std::string&             p_name,
                               std::function<T(void)>         p_gatherer,
                               std::function<void(T)>         p_provider,
                               float                          p_step = 1.f,
                               T                              p_min  = std::numeric_limits<T>::min(),
                               T                              p_max  = std::numeric_limits<T>::max());
        static void DrawBoolean(UI::Internal::WidgetContainer& p_root,
                                const std::string&             p_name,
                                std::function<bool(void)>      p_gatherer,
                                std::function<void(bool)>      p_provider);
        static void DrawVec2(UI::Internal::WidgetContainer& p_root,
                             const std::string&             p_name,
                             std::function<Vector2(void)>   p_gatherer,
                             std::function<void(Vector2)>   p_provider,
                             float                          p_step = 1.f,
                             float                          p_min  = _MIN_FLOAT,
                             float                          p_max  = _MAX_FLOAT);
        static void DrawVec3(UI::Internal::WidgetContainer& p_root,
                             const std::string&             p_name,
                             std::function<Vector3(void)>   p_gatherer,
                             std::function<void(Vector3)>   p_provider,
                             float                          p_step = 1.f,
                             float                          p_min  = _MIN_FLOAT,
                             float                          p_max  = _MAX_FLOAT);
        static void DrawVec4(UI::Internal::WidgetContainer& p_root,
                             const std::string&             p_name,
                             std::function<Vector4(void)>   p_gatherer,
                             std::function<void(Vector4)>   p_provider,
                             float                          p_step = 1.f,
                             float                          p_min  = _MIN_FLOAT,
                             float                          p_max  = _MAX_FLOAT);
        static void DrawQuat(UI::Internal::WidgetContainer&  p_root,
                             const std::string&              p_name,
                             std::function<Quaternion(void)> p_gatherer,
                             std::function<void(Quaternion)> p_provider,
                             float                           p_step = 1.f,
                             float                           p_min  = _MIN_FLOAT,
                             float                           p_max  = _MAX_FLOAT);
        static void DrawDDString(UI::Internal::WidgetContainer&   p_root,
                                 const std::string&               p_name,
                                 std::function<std::string(void)> p_gatherer,
                                 std::function<void(std::string)> p_provider,
                                 const std::string&               p_identifier);
        static void DrawString(UI::Internal::WidgetContainer&   p_root,
                               const std::string&               p_name,
                               std::function<std::string(void)> p_gatherer,
                               std::function<void(std::string)> p_provider);
        static void DrawColor(UI::Internal::WidgetContainer&        p_root,
                              const std::string&                    p_name,
                              std::function<UI::Types::Color(void)> p_gatherer,
                              std::function<void(UI::Types::Color)> p_provider,
                              bool                                  p_hasAlpha = false);

        template<typename T>
        static ImGuiDataType_ GetDataType();

        template<typename T>
        static std::string GetFormat();

    private:
        // static Rendering::Resources::Texture* __EMPTY_TEXTURE;
    };

    template<typename T>
    ImGuiDataType_ GUIDrawer::GetDataType()
    {
        if constexpr (std::is_same<T, float>::value)
            return ImGuiDataType_Float;
        else if constexpr (std::is_same<T, double>::value)
            return ImGuiDataType_Double;
        else if constexpr (std::is_same<T, uint8_t>::value)
            return ImGuiDataType_U8;
        else if constexpr (std::is_same<T, uint16_t>::value)
            return ImGuiDataType_U16;
        else if constexpr (std::is_same<T, uint32_t>::value)
            return ImGuiDataType_U32;
        else if constexpr (std::is_same<T, uint64_t>::value)
            return ImGuiDataType_U64;
        else if constexpr (std::is_same<T, int8_t>::value)
            return ImGuiDataType_S8;
        else if constexpr (std::is_same<T, int16_t>::value)
            return ImGuiDataType_S16;
        else if constexpr (std::is_same<T, int32_t>::value)
            return ImGuiDataType_S32;
        else if constexpr (std::is_same<T, int64_t>::value)
            return ImGuiDataType_S64;
    }

    template<typename T>
    std::string GUIDrawer::GetFormat()
    {
        if constexpr (std::is_same<T, double>::value)
            return "%.5f";
        else if constexpr (std::is_same<T, float>::value)
            return "%.3f";
        else
            return "%d";
    }

    template<typename T>
    void GUIDrawer::DrawScalar(UI::Internal::WidgetContainer& p_root, const std::string& p_name, T& p_data, float p_step, T p_min, T p_max)
    {
        static_assert(std::is_scalar<T>::value, "T must be a scalar");

        CreateTitle(p_root, p_name);
        auto& widget =
            p_root.CreateWidget<UI::Widgets::Drags::DragSingleScalar<T>>(GetDataType<T>(), p_min, p_max, p_data, p_step, "", GetFormat<T>());
        auto& dispatcher = widget.AddPlugin<UI::Plugins::DataDispatcher<T>>();
        dispatcher.RegisterReference(p_data);
    }

    template<typename T>
    void GUIDrawer::DrawScalar(UI::Internal::WidgetContainer& p_root,
                               const std::string&             p_name,
                               std::function<T(void)>         p_gatherer,
                               std::function<void(T)>         p_provider,
                               float                          p_step,
                               T                              p_min,
                               T                              p_max)
    {
        static_assert(std::is_scalar<T>::value, "T must be a scalar");

        CreateTitle(p_root, p_name);
        auto& widget = p_root.CreateWidget<UI::Widgets::Drags::DragSingleScalar<T>>(
            GetDataType<T>(), p_min, p_max, static_cast<T>(0), p_step, "", GetFormat<T>());
        auto& dispatcher = widget.AddPlugin<UI::Plugins::DataDispatcher<T>>();
        dispatcher.RegisterGatherer(p_gatherer);
        dispatcher.RegisterProvider(p_provider);
    }
} // namespace LunarYue
