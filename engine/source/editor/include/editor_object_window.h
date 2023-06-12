#pragma once
#include <string>

#include "function/framework/object/object.h"
#include "function/ui/Panels/PanelWindow.h"
#include "function/ui/Widgets/Layout/TreeNode.h"

namespace LunarYue
{
    class ObjectWindow : public UI::Panels::PanelWindow
    {
    public:
        ObjectWindow(const std::string& p_title, bool p_opened, const UI::Settings::PanelWindowSettings& p_windowSettings);

        /**
         * Clear hierarchy nodes
         */
        void Clear();

        /**
         * Unselect every widgets
         */
        void UnselectActorsWidgets();
        void SelectActorByID(ObjectID id);

        /**
         * Select the widget corresponding to the given actor
         * @param object
         */
        void SelectActorByInstance(std::weak_ptr<Object> object);

        /**
         * Select the widget
         * @param p_actor
         */
        void SelectActorByWidget(UI::Widgets::Layout::TreeNode& p_widget);

        /**
         * Attach the given actor linked widget to its parent widget
         * @param p_actor
         */
        void AttachActorToParent(ObjectID id);

        /**
         * Detach the given actor linked widget from its parent widget
         * @param p_actor
         */
        void DetachFromParent(ObjectID id);

        /**
         * Delete the widget referencing the given actor
         * @param p_actor
         */
        void DeleteActorByID(ObjectID id);

        /**
         * Add a widget referencing the given actor
         * @param p_actor
         */
        void AddActorByInstance(std::weak_ptr<Object> object);

    public:
        Eventing::Event<std::weak_ptr<Object>> ActorSelectedEvent;
        Eventing::Event<std::weak_ptr<Object>> ActorUnselectedEvent;

    private:
        UI::Widgets::Layout::TreeNode* m_sceneRoot;

        std::unordered_map<ObjectID, UI::Widgets::Layout::TreeNode*> m_widgetActorLink;
    };
} // namespace LunarYue