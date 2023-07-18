#pragma once

#include <core/cmd_line/parser.hpp>
#include <core/common_lib/basetypes.hpp>

namespace runtime
{
    struct app
    {
        virtual ~app() = default;
        //-----------------------------------------------------------------------------
        //  Name : setup (virtual )
        /// <summary>
        /// Setup before engine initialization. this is a chance to eg. modify
        /// the engine parameters.
        /// </summary>
        //-----------------------------------------------------------------------------

        virtual void setup(cmd_line::parser& parser);
        //-----------------------------------------------------------------------------
        //  Name : start (virtual )
        /// <summary>
        ///  setup after engine initialization and before running the main loop
        /// </summary>
        //-----------------------------------------------------------------------------
        virtual void start(cmd_line::parser& parser);

        //-----------------------------------------------------------------------------
        //  Name : stop (virtual )
        /// <summary>
        /// Cleanup after the main loop.
        /// </summary>
        //-----------------------------------------------------------------------------
        virtual void stop();

        //-----------------------------------------------------------------------------
        //  Name : run_one_frame (virtual )
        /// <summary>
        /// Runs one frame loop.
        /// </summary>
        //-----------------------------------------------------------------------------
        virtual void run_one_frame();

        //-----------------------------------------------------------------------------
        //  Name : run ()
        /// <summary>
        /// Initialize the engine and run the main loop, then return the
        /// application exit code.
        /// </summary>
        //-----------------------------------------------------------------------------
        int run(int argc, char* argv[]);

        //-----------------------------------------------------------------------------
        //  Name : quit_with_error ()
        /// <summary>
        /// Show an error message, terminate the main loop, and set failure exit code.
        /// </summary>
        //-----------------------------------------------------------------------------
        void quit_with_error(const std::string&);

        //-----------------------------------------------------------------------------
        //  Name : quit ()
        /// <summary>
        /// Terminate the main loop and exit the app.
        /// </summary>
        //-----------------------------------------------------------------------------
        void quit(int exitcode = 0);

    protected:
        /// exit code of the application
        int  exitcode_ = 0;
        bool running_  = true;
    };
} // namespace runtime
