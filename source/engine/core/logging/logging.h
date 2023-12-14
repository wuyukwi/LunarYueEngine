#pragma once

#include <spdlog/spdlog.h>

#include "../common_lib/platform/config.hpp"

#if ETH_ON(ETH_PLATFORM_WINDOWS)
#include <spdlog/sinks/msvc_sink.h>
#include <spdlog/sinks/wincolor_sink.h>
namespace spdlog
{
    namespace sinks
    {
        using platform_sink_mt = wincolor_stdout_sink_mt;
        using platform_sink_st = wincolor_stdout_sink_st;
    } // namespace sinks
} // namespace spdlog
#elif ETH_ON(ETH_PLATFORM_LINUX) || ETH_ON(ETH_PLATFORM_APPLE)
#include <spdlog/sinks/ansicolor_sink.h>
namespace spdlog
{
    namespace sinks
    {
        using platform_sink_mt = ansicolor_stdout_sink_mt;
        using platform_sink_st = ansicolor_stdout_sink_st;
    } // namespace sinks
} // namespace spdlog
#elif ETH_ON(ETH_PLATFORM_ANDROID)
#include <spdlog/sinks/android_sink.h>
namespace spdlog
{
    namespace sinks
    {
        using platform_sink_mt = android_sink_mt;
        using platform_sink_st = android_sink_st;
    } // namespace sinks
} // namespace spdlog
#else
#include <spdlog/sinks/ansicolor_sink.h>
namespace spdlog
{
    namespace sinks
    {
        using platform_sink_mt = stdout_sink_mt;
        using platform_sink_st = stdout_sink_st;
    } // namespace sinks
} // namespace spdlog
#endif

#include <spdlog/sinks/basic_file_sink.h>
namespace logging
{
    using namespace spdlog;

#define APPLOG "Log"
#define APPLOG_INFO(...) spdlog::get(APPLOG)->info(__VA_ARGS__)
#define APPLOG_TRACE(...) spdlog::get(APPLOG)->trace(__VA_ARGS__)
#define APPLOG_ERROR(...) spdlog::get(APPLOG)->error(__VA_ARGS__)
#define APPLOG_WARNING(...) spdlog::get(APPLOG)->warn(__VA_ARGS__)
#define APPLOG_SEPARATOR() APPLOG_INFO("-----------------------------")
} // namespace logging
