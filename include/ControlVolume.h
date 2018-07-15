#ifndef SIMPLE_CFD_CONTROLVOLUME_H
#define SIMPLE_CFD_CONTROLVOLUME_H

// STD Includes
#include <vector>

// Library Includes
#include <units.h>

namespace units {
namespace viscosity {
    using meters               = units::length::meters;
    using seconds              = units::time::seconds;
    using meters_squared_per_s = compound_unit<squared<meters>, inverse<seconds>>;
    using meters_squared_per_s_t =
        units::unit_t<meters_squared_per_s, double, units::linear_scale>;
}
namespace density {
    using kg_per_cu_m_t =
        units::unit_t<units::density::kg_per_cu_m, double, units::linear_scale>;
}
}

struct Point2d {
    units::length::meter_t x;
    units::length::meter_t y;
};

struct Velocity2d {
    units::velocity::meters_per_second_t x;
    units::velocity::meters_per_second_t y;
};

// TODO: Descriptive comment here
class ControlVolume {
  public:
    ControlVolume();

    /**
     * Construct a ControlVolume with a given pressure and velocity *
     * @param pressure
     * @param velocity
     */
    ControlVolume(double pressure, Velocity2d velocity);

    // TODO: Make this into an `update` function and add setters for constant values?
    // TODO: We'll probably need to change this to work with arbitrary neighbours
    // TODO: Make args more succient?
    /**
     * Construct a new ControlVolume from a given ControlVolume and it's neighbours
     *
     * TODO: ASCII diagram here to show where each "volume" is
     *
     * @param original_volume_with_coord
     * @param left_neighbour_with_coord
     * @param right_neighbour_with_coord
     * @param top_neighbour_with_coord
     * @param bottom_neighbour_with_coord
     * @param dt TODO
     */
    ControlVolume(std::pair<ControlVolume, Point2d> original_volume_with_point,
                  std::pair<ControlVolume, Point2d> left_neighbour_with_point,
                  std::pair<ControlVolume, Point2d> right_neighbour_with_point,
                  std::pair<ControlVolume, Point2d> top_neighbour_with_point,
                  std::pair<ControlVolume, Point2d> bottom_neighbour_with_point,
                  units::time::second_t dt,
                  units::density::kg_per_cu_m_t density,
                  units::viscosity::meters_squared_per_s_t viscosity,
                  units::velocity::meters_per_second_t speed_of_sound);

    /**
     * Get the current pressure in this ControlVolume
     *
     * @return TODO?
     */
    units::pressure::pascal_t getPressure() { return this->pressure; }

    /**
     * Sets the current pressure in this ControlVolume
     *
     * @param pressure  TODO?
     */
    void setPressure(units::pressure::pascal_t pressure) {this->pressure = pressure;}

    /**
     * Get the current velocity in this ControlVolume
     *
     * @return TODO?
     */
    Velocity2d getVelocity() { return this->velocity; }

    /**
     * Set the current velocity in this ControlVolume
     *
     * @param velocity  TODO?
     */
    void setVelocity(Velocity2d velocity) { this->velocity = velocity; }

    // TODO: REMOVE THIS
    Velocity2d new_velocity;
    units::pressure::pascal_t new_pressure;

  private:
    // The pressure in this control volume
    units::pressure::pascal_t pressure;

    // The velocity in this control volume
    Velocity2d velocity;

    // TODO: Do we want to keep things like viscosity, density, etc. as member variables
};

#endif // SIMPLE_CFD_CONTROLVOLUME_H
