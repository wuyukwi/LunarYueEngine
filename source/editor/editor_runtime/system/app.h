#pragma once

#include <runtime/system/app.h>

#include <string>

namespace imguidock
{
    class dockspace;
}
class console_log;
class render_window;

namespace editor
{
    class app final : public runtime::app
    {
    public:
        ~app() override = default;

        void setup(cmd_line::parser& parser) override;

        void start(cmd_line::parser& parser) override;

        void stop() override;

    private:
        void create_docks();
        void register_console_commands();
        void draw_docks(float dt);
        void draw_header(render_window& window);
        void draw_dockspace(render_window& window);
        void draw_footer(render_window& window);
        void draw_start_page(render_window& window);
        void draw_menubar(render_window& window);
        void draw_toolbar();
        void handle_drag_and_drop();

        bool show_start_page_ = true;
        ///
        std::shared_ptr<console_log> console_log_;
        ///
        std::string console_dock_name_;
    };
} // namespace editor
