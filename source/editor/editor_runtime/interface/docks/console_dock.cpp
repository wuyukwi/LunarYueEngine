#include "console_dock.h"
#include "editor_runtime/console/console_log.h"
#include <editor_core/gui/gui.h>

console_dock::console_dock(const std::string& title, const std::shared_ptr<console_log>& log) : console_log_(log) {}

void console_dock::render()
{
    if (!console_log_)
        return;

    gui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
    static ImGuiTextFilter filter;
    filter.Draw("FILTER", 180);
    gui::PopStyleVar();
    // TODO: display items starting from the bottom
    gui::SameLine();
    if (gui::SmallButton("CLEAR"))
    {
        console_log_->clear_log();
    }
    gui::Separator();

    // Display every line as a separate entry so we can change their color or add custom widgets. If you only
    // want raw text you can use gui::TextUnformatted(log.begin(), log.end());
    // NB- if you have thousands of entries this approach may be too inefficient. You can seek and display
    // only the lines that are visible - CalcListClipping() is a helper to compute this information.
    // If your items are of variable size you may want to implement code similar to what CalcListClipping()
    // does. Or split your data into fixed height items to allow random-seeking into your list.
    gui::BeginChild(
        "ScrollingRegion", ImVec2(0, -ImGui::GetStyle().ItemSpacing.y - gui::GetFrameHeightWithSpacing()), ImGuiWindowFlags_HorizontalScrollbar);
    if (gui::BeginPopupContextWindow())
    {
        if (gui::Selectable("Clear"))
            console_log_->clear_log();
        gui::EndPopup();
    }
    gui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing

    const auto items = console_log_->get_items();

    for (const auto& pair_msg : items)
    {
        const char* item_cstr = pair_msg.first.data();
        if (!filter.PassFilter(item_cstr))
            continue;
        const auto& colorization = console_log_->get_level_colorization(pair_msg.second);
        ImVec4      col          = {colorization[0], colorization[1], colorization[2], colorization[3]};
        gui::PushStyleColor(ImGuiCol_Text, col);
        gui::TextWrapped("%s", item_cstr);
        gui::PopStyleColor();
    }
    if (console_log_->has_new_entries())
        gui::SetScrollHereY();

    console_log_->set_has_new_entries(false);

    gui::PopStyleVar();
    gui::EndChild();
    gui::Separator();

    // Command-line
    static std::array<char, 64> input_buff;
    input_buff.fill(0);

    gui::PushItemWidth(gui::GetContentRegionAvail().x * 0.5f);
    if (gui::InputText("ENTER COMMAND", input_buff.data(), input_buff.size(), ImGuiInputTextFlags_EnterReturnsTrue))
    {
        // copy from c_str to remove trailing zeros
        std::string command   = input_buff.data();
        std::string error_msg = console_log_->process_input(command);
        if (error_msg != "")
        {
            APPLOG_WARNING(error_msg.c_str());
        }
        // Demonstrate keeping auto focus on the input box
        if (gui::IsItemHovered() ||
            (gui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && !gui::IsAnyItemActive() && !gui::IsMouseClicked(0)))
            gui::SetKeyboardFocusHere(-1); // Auto focus previous widget
    }

    gui::PopItemWidth();
}
