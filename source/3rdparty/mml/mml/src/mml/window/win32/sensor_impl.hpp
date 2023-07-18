#ifndef MML_SENSORIMPLWIN32_HPP
#define MML_SENSORIMPLWIN32_HPP

#include <array>
#include <mml/window/sensor.hpp>

namespace mml
{
namespace priv
{
////////////////////////////////////////////////////////////
/// \brief Windows implementation of sensors
///
////////////////////////////////////////////////////////////
class sensor_impl
{
public:

    ////////////////////////////////////////////////////////////
    /// \brief Perform the global initialization of the sensor module
    ///
    ////////////////////////////////////////////////////////////
    static void initialize();

    ////////////////////////////////////////////////////////////
    /// \brief Perform the global cleanup of the sensor module
    ///
    ////////////////////////////////////////////////////////////
    static void cleanup();

    ////////////////////////////////////////////////////////////
    /// \brief Check if a sensor is available
    ///
    /// \param sensor sensor to check
    ///
    /// \return True if the sensor is available, false otherwise
    ///
    ////////////////////////////////////////////////////////////
    static bool is_available(sensor::type sensor);

    ////////////////////////////////////////////////////////////
    /// \brief Open the sensor
    ///
    /// \param sensor Type of the sensor
    ///
    /// \return True on success, false on failure
    ///
    ////////////////////////////////////////////////////////////
    bool open(sensor::type sensor);

    ////////////////////////////////////////////////////////////
    /// \brief Close the sensor
    ///
    ////////////////////////////////////////////////////////////
    void close();

    ////////////////////////////////////////////////////////////
    /// \brief Update the sensor and get its new value
    ///
    /// \return sensor value
    ///
    ////////////////////////////////////////////////////////////
    std::array<float, 3> update();

    ////////////////////////////////////////////////////////////
    /// \brief Enable or disable the sensor
    ///
    /// \param enabled True to enable, false to disable
    ///
    ////////////////////////////////////////////////////////////
    void set_enabled(bool enabled);
};

} // namespace priv

} // namespace mml


#endif // MML_SENSORIMPLWIN32_HPP
