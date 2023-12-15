#include "simulation.h"
#include <chrono>
#include <numeric>
#include <thread>

namespace core
{

    simulation::simulation()
    {
        // Your constructor implementation
    }

    void simulation::run_one_frame(bool is_active)
    {
        auto current_time     = duration_t(std::chrono::steady_clock::now().time_since_epoch().count());
        auto elapsed_time     = current_time - last_frame_timepoint_;
        last_frame_timepoint_ = current_time;

        frame_++;

        auto elapsed_seconds = static_cast<float>(elapsed_time) / 1e9; // Convert nanoseconds to seconds
        auto fps             = static_cast<std::uint32_t>(1.0f / elapsed_seconds);

        previous_timesteps_.push_back(elapsed_time);
        if (previous_timesteps_.size() > smoothing_step_)
        {
            previous_timesteps_.erase(previous_timesteps_.begin());
        }

        timestep_ = std::accumulate(previous_timesteps_.begin(), previous_timesteps_.end(), duration_t(0)) / smoothing_step_;

        // Calculate time to sleep for achieving desired FPS
        if (!is_active && max_inactive_fps_ > 0 && fps > max_inactive_fps_)
        {
            auto sleep_duration = std::chrono::milliseconds(0);
            sleep_duration      = std::chrono::milliseconds(static_cast<long long>((1.0f / max_inactive_fps_ - elapsed_seconds) * 1e3) + 1);

            if (sleep_duration.count() > 0)
            {
                std::this_thread::sleep_for(sleep_duration);
            }
        }
    }

    void simulation::set_min_fps(std::uint32_t fps) { min_fps_ = fps; }

    void simulation::set_max_fps(std::uint32_t fps) { max_fps_ = fps; }

    void simulation::set_max_inactive_fps(std::uint32_t fps) { max_inactive_fps_ = fps; }

    void simulation::set_time_smoothing_step(std::uint32_t step) { smoothing_step_ = step; }

    simulation::duration_t simulation::get_time_since_launch() const
    {
        return duration_t(std::chrono::steady_clock::now().time_since_epoch().count()) - launch_timepoint_;
    }

    std::uint32_t simulation::get_fps() const { return static_cast<std::uint32_t>(1.0f / (static_cast<float>(timestep_) / 1e9)); }

    float simulation::get_delta_time() const { return static_cast<float>(timestep_) / 1e9; }
} // namespace core
