
#include "function/framework/object/object.h"
#include "function/ui/Panels/PanelWindow.h"
#include "function/ui/Widgets/InputFields/InputText.h"
#include "function/ui/Widgets/Layout/Group.h"
#include "function/ui/Widgets/Selection/ComboBox.h"

namespace LunarYue
{
    class DetailWindow : public UI::Panels::PanelWindow
    {
    public:
        /**
         * Constructor
         */
        DetailWindow(const std::string& p_title, bool p_opened, const UI::Settings::PanelWindowSettings& p_windowSettings);

        /**
         * Destructor
         */
        ~DetailWindow();

        /**
         * Focus the given actor
         */
        void FocusActor(const std::weak_ptr<Object>& target);

        /**
         * Unfocus the currently targeted actor
         */
        void UnFocus();

        /**
         * Unfocus the currently targeted actor without removing listeners attached to this actor
         */
        void SoftUnFocus();

        /**
         * Returns the currently selected actor
         */
        const std::weak_ptr<Object>& GetTargetActor() const;

        /**
         * Create the actor inspector for the given actor
         */
        void CreateActorInspector(const std::weak_ptr<Object>& target);

        /**
         * Draw the given component in inspector
         */
        void DrawComponent(Reflection::ReflectionInstance& instance);

        /**
         * Draw the given behaviour in inspector
         */
        void DrawBehaviour(Reflection::ReflectionInstance& instance);

        /**
         * Refresh the inspector
         */
        void Refresh();

    private:
        std::weak_ptr<Object>                m_targetObject;
        UI::Widgets::Layout::Group*          m_actorInfo;
        UI::Widgets::Layout::Group*          m_inspectorHeader;
        UI::Widgets::Selection::ComboBox*    m_componentSelectorWidget;
        UI::Widgets::InputFields::InputText* m_scriptSelectorWidget;

        uint64_t m_componentAddedListener   = 0;
        uint64_t m_componentRemovedListener = 0;
        uint64_t m_behaviourAddedListener   = 0;
        uint64_t m_behaviourRemovedListener = 0;
        uint64_t m_destroyedListener        = 0;
    };
} // namespace LunarYue