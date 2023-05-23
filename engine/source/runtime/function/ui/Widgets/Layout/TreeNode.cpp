/**
 * @project: Overload
 * @author: Overload Tech.
 * @licence: MIT
 */

#include "function/ui/Widgets/Layout/TreeNode.h"

LunarYue::UI::Widgets::Layout::TreeNode::TreeNode(const std::string& p_name, bool p_arrowClickToOpen) :
    DataWidget(name), name(p_name), m_arrowClickToOpen(p_arrowClickToOpen)
{
    m_autoExecutePlugins = false;
}

void LunarYue::UI::Widgets::Layout::TreeNode::Open()
{
    m_shouldOpen  = true;
    m_shouldClose = false;
}

void LunarYue::UI::Widgets::Layout::TreeNode::Close()
{
    m_shouldClose = true;
    m_shouldOpen  = false;
}

bool LunarYue::UI::Widgets::Layout::TreeNode::IsOpened() const { return m_opened; }

void LunarYue::UI::Widgets::Layout::TreeNode::_Draw_Impl()
{
    bool prevOpened = m_opened;

    if (m_shouldOpen)
    {
        ImGui::SetNextItemOpen(true);
        m_shouldOpen = false;
    }
    else if (m_shouldClose)
    {
        ImGui::SetNextItemOpen(false);
        m_shouldClose = false;
    }

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None;
    if (m_arrowClickToOpen)
        flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    if (selected)
        flags |= ImGuiTreeNodeFlags_Selected;
    if (leaf)
        flags |= ImGuiTreeNodeFlags_Leaf;

    bool opened = ImGui::TreeNodeEx((name + m_widgetID).c_str(), flags);

    if (ImGui::IsItemClicked() && (ImGui::GetMousePos().x - ImGui::GetItemRectMin().x) > ImGui::GetTreeNodeToLabelSpacing())
    {
        ClickedEvent.Invoke();

        if (ImGui::IsMouseDoubleClicked(0))
        {
            DoubleClickedEvent.Invoke();
        }
    }

    if (opened)
    {
        if (!prevOpened)
            OpenedEvent.Invoke();

        m_opened = true;

        ExecutePlugins(); // Manually execute plugins to make plugins considering the TreeNode and no childs

        DrawWidgets();

        ImGui::TreePop();
    }
    else
    {
        if (prevOpened)
            ClosedEvent.Invoke();

        m_opened = false;

        ExecutePlugins(); // Manually execute plugins to make plugins considering the TreeNode and no childs
    }
}
