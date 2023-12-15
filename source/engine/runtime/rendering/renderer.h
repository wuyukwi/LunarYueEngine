#pragma once
#include "render_window.h"

#include <core/cmd_line/parser.hpp>
#include <core/common_lib/basetypes.hpp>
#include <runtime/system/events.h>

#include <memory>
#include <vector>

namespace runtime
{
    struct renderer
    {
        renderer(cmd_line::parser& parser);
        ~renderer();

        //-----------------------------------------------------------------------------
        //  Name : init_backend ()
        /// <summary>
        ///
        ///
        ///
        /// </summary>
        //-----------------------------------------------------------------------------
        bool init_backend(cmd_line::parser& parser);

        //-----------------------------------------------------------------------------
        //  Name : frame_end ()
        /// <summary>
        ///
        ///
        ///
        /// </summary>
        //-----------------------------------------------------------------------------
        void frame_end(float);

        //-----------------------------------------------------------------------------
        //  Name : get_render_frame ()
        /// <summary>
        ///
        ///
        ///
        /// </summary>
        //-----------------------------------------------------------------------------
        inline std::uint32_t get_render_frame() const { return render_frame_; }

        //-----------------------------------------------------------------------------
        //  Name : register_window ()
        /// <summary>
        ///
        ///
        ///
        /// </summary>
        //-----------------------------------------------------------------------------
        void register_window(std::unique_ptr<render_window> window);

        /// <summary>
        ///
        /// </summary>
        /// <param name="id"></param>
        void remove_window_by_id(uint32_t id);

        bool should_quit() { return should_quit_; }

        //-----------------------------------------------------------------------------
        //  Name : get_windows ()
        /// <summary>
        ///
        ///
        ///
        /// </summary>
        //-----------------------------------------------------------------------------
        const std::vector<std::unique_ptr<render_window>>& get_windows() const;
        const std::unique_ptr<render_window>&              get_window_for_id(std::uint32_t id) const;
        const std::unique_ptr<render_window>&              get_main_window() const;
        void                                               hide_all_secondary_windows();
        void                                               show_all_secondary_windows();
        //-----------------------------------------------------------------------------
        //  Name : get_focused_window ()
        /// <summary>
        ///
        ///
        ///
        /// </summary>
        //-----------------------------------------------------------------------------
        const std::unique_ptr<render_window>& get_focused_window() const;

        void platform_events(const std::vector<SDL_Event>& events);

    protected:
        std::uint32_t render_frame_ = 0;

        /// engine windows
        std::vector<std::unique_ptr<render_window>> windows_;

        bool should_quit_ = false;
    };
} // namespace runtime
