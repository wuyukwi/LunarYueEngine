#pragma once

#include <vector>

namespace core
{

    struct simulation
    {
        using duration_t = unsigned long long; // Change the type according to your needs

        simulation();

        void run_one_frame(bool is_active);

        inline const std::uint64_t& get_frame() const { return frame_; }

        void set_min_fps(std::uint32_t fps);

        void set_max_fps(std::uint32_t fps);

        void set_max_inactive_fps(std::uint32_t fps);

        void set_time_smoothing_step(std::uint32_t step);

        duration_t get_time_since_launch() const;

        std::uint32_t get_fps() const;

        float get_delta_time() const;

    protected:
        std::uint32_t           min_fps_          = 0;
        std::uint32_t           max_fps_          = 400;
        std::uint32_t           max_inactive_fps_ = 20;
        std::vector<duration_t> previous_timesteps_;
        duration_t              timestep_             = 0;
        std::uint64_t           frame_                = 0;
        std::uint32_t           smoothing_step_       = 11;
        duration_t              last_frame_timepoint_ = 0;
        duration_t              launch_timepoint_     = 0;
    };
} // namespace core
