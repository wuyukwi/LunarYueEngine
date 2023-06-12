#include "editor/include/editor_object_window.h"
#include "editor/include/editor_global_context.h"
#include "editor/include/editor_scene_manager.h"

#include "function/framework/level/level.h"
#include "function/framework/world/world_manager.h"

#include "runtime/core/base/macro.h"

#include "function/ui/Plugins/ContextualMenu.h"
#include "function/ui/Plugins/DDTarget.h"
#include "function/ui/Widgets/InputFields/InputText.h"

namespace LunarYue
{

    class ObjectWindowContextualMenu : public UI::Plugins::ContextualMenu
    {
    public:
        ObjectWindowContextualMenu(ObjectID id, UI::Widgets::Layout::TreeNode& p_treeNode, bool p_panelMenu = false) :
            m_id(id), m_treeNode(p_treeNode)
        {
            const auto object = g_editor_global_context.m_scene_manager->getGObjectFormID(id).lock();
            if (object)
            {
                auto& focusButton = CreateWidget<UI::Widgets::Menu::MenuItem>("Focus");
                focusButton.ClickedEvent += [this] {};

                auto& duplicateButton = CreateWidget<UI::Widgets::Menu::MenuItem>("Duplicate");
                duplicateButton.ClickedEvent += [this] {};

                auto& deleteButton = CreateWidget<UI::Widgets::Menu::MenuItem>("Delete");
                deleteButton.ClickedEvent += [this] { g_editor_global_context.m_scene_manager->onDeleteSelectedGObject(); };
            }

            auto& createActor = CreateWidget<UI::Widgets::Menu::MenuList>("Create...");
        }

        virtual void Execute() override
        {
            if (!m_widgets.empty())
                UI::Plugins::ContextualMenu::Execute();
        }

    private:
        ObjectID                       m_id;
        UI::Widgets::Layout::TreeNode& m_treeNode;
    };

    void ExpandTreeNode(UI::Widgets::Layout::TreeNode& p_toExpand, const UI::Widgets::Layout::TreeNode* p_root)
    {
        p_toExpand.Open();

        if (&p_toExpand != p_root && p_toExpand.HasParent())
        {
            ExpandTreeNode(*static_cast<UI::Widgets::Layout::TreeNode*>(p_toExpand.GetParent()), p_root);
        }
    }

    std::vector<UI::Widgets::Layout::TreeNode*> nodesToCollapse;
    std::vector<UI::Widgets::Layout::TreeNode*> founds;

    void ExpandTreeNodeAndEnable(UI::Widgets::Layout::TreeNode& p_toExpand, const UI::Widgets::Layout::TreeNode* p_root)
    {
        if (!p_toExpand.IsOpened())
        {
            p_toExpand.Open();
            nodesToCollapse.push_back(&p_toExpand);
        }

        p_toExpand.enabled = true;

        if (&p_toExpand != p_root && p_toExpand.HasParent())
        {
            ExpandTreeNodeAndEnable(*static_cast<UI::Widgets::Layout::TreeNode*>(p_toExpand.GetParent()), p_root);
        }
    }

    ObjectWindow::ObjectWindow(const std::string& p_title, bool p_opened, const UI::Settings::PanelWindowSettings& p_windowSettings) :
        PanelWindow(p_title, p_opened, p_windowSettings)
    {
        auto& searchBar = CreateWidget<UI::Widgets::InputFields::InputText>();
        searchBar.ContentChangedEvent += [this](const std::string& p_content) {
            founds.clear();
            auto content = p_content;
            std::transform(content.begin(), content.end(), content.begin(), ::tolower);

            for (auto& [actor, item] : m_widgetActorLink)
            {
                if (!p_content.empty())
                {
                    auto itemName = item->name;
                    std::transform(itemName.begin(), itemName.end(), itemName.begin(), ::tolower);

                    if (itemName.find(content) != std::string::npos)
                    {
                        founds.push_back(item);
                    }

                    item->enabled = false;
                }
                else
                {
                    item->enabled = true;
                }
            }

            for (const auto node : founds)
            {
                node->enabled = true;

                if (node->HasParent())
                {
                    ExpandTreeNodeAndEnable(*static_cast<UI::Widgets::Layout::TreeNode*>(node->GetParent()), m_sceneRoot);
                }
            }

            if (p_content.empty())
            {
                for (const auto node : nodesToCollapse)
                {
                    node->Close();
                }

                nodesToCollapse.clear();
            }
        };

        const auto active_level = g_runtime_global_context.m_world_manager->getCurrentActiveLevel().lock();

        m_sceneRoot = &CreateWidget<UI::Widgets::Layout::TreeNode>("World", true);
        m_sceneRoot->Open();
        m_sceneRoot->AddPlugin<UI::Plugins::DDTarget<std::pair<ObjectID, UI::Widgets::Layout::TreeNode*>>>("Actor").DataReceivedEvent +=
            [active_level, this](std::pair<ObjectID, UI::Widgets::Layout::TreeNode*> p_element) {
                if (p_element.second->HasParent())
                    p_element.second->GetParent()->UnconsiderWidget(*p_element.second);

                m_sceneRoot->ConsiderWidget(*p_element.second);

                if (active_level)
                {
                    const auto object = active_level->getGObjectByID(p_element.first).lock();
                    if (object)
                    {
                        object->detachFromParent();
                    }
                }
            };
        m_sceneRoot->AddPlugin<ObjectWindowContextualMenu>(k_invalid_object_id, *m_sceneRoot);
    }

    void ObjectWindow::Clear()
    {
        g_editor_global_context.m_scene_manager->setSelectedObjectID(k_invalid_object_id);

        m_sceneRoot->RemoveAllWidgets();
        m_widgetActorLink.clear();
    }

    void ObjectWindow::UnselectActorsWidgets()
    {
        for (const auto& widget : m_widgetActorLink)
            widget.second->selected = false;
    }

    void ObjectWindow::SelectActorByID(ObjectID id)
    {
        if (const auto result = m_widgetActorLink.find(id); result != m_widgetActorLink.end())
            if (result->second)
                SelectActorByWidget(*result->second);
    }

    void ObjectWindow::SelectActorByWidget(UI::Widgets::Layout::TreeNode& p_widget)
    {
        UnselectActorsWidgets();

        p_widget.selected = true;

        if (p_widget.HasParent())
        {
            ExpandTreeNode(*static_cast<UI::Widgets::Layout::TreeNode*>(p_widget.GetParent()), m_sceneRoot);
        }
    }

    void ObjectWindow::AttachActorToParent(ObjectID id)
    {
        const auto actorWidget = m_widgetActorLink.find(id);

        if (actorWidget != m_widgetActorLink.end())
        {
            auto widget = actorWidget->second;

            if (widget->HasParent())
                widget->GetParent()->UnconsiderWidget(*widget);

            const auto object = g_editor_global_context.m_scene_manager->getGObjectFormID(id).lock();
            if (object->hasParent())
            {
                const auto parentWidget = m_widgetActorLink.at(object->getParentID());
                parentWidget->leaf      = false;
                parentWidget->ConsiderWidget(*widget);
            }
        }
    }

    void ObjectWindow::DetachFromParent(ObjectID id)
    {
        if (const auto actorWidget = m_widgetActorLink.find(id); actorWidget != m_widgetActorLink.end())
        {
            const auto object = g_editor_global_context.m_scene_manager->getGObjectFormID(id).lock();

            if (object->hasParent() && object->getParent().lock()->getChildren().size() == 1)
            {
                if (const auto parentWidget = m_widgetActorLink.find(object->getParentID()); parentWidget != m_widgetActorLink.end())
                {
                    parentWidget->second->leaf = true;
                }
            }

            const auto widget = actorWidget->second;

            if (widget->HasParent())
                widget->GetParent()->UnconsiderWidget(*widget);

            m_sceneRoot->ConsiderWidget(*widget);
        }
    }

    void ObjectWindow::DeleteActorByID(ObjectID id)
    {
        if (const auto result = m_widgetActorLink.find(id); result != m_widgetActorLink.end())
        {
            if (result->second)
            {
                result->second->Destroy();
            }

            m_widgetActorLink.erase(result);
        }
    }

    void ObjectWindow::AddActorByInstance(std::weak_ptr<Object> object) {}
} // namespace LunarYue