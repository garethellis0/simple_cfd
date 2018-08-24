#ifndef SIMPLE_CFD_CONTROLVOLUME_H
#define SIMPLE_CFD_CONTROLVOLUME_H

// STD Includes
#include <vector>

// Library Includes
#include <units.h>

// Custom Units
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

    // TODO: Since all neighbours are treated as being divergent from this volume in only one dimension, should we pass in a Pair<ControlVolume, distance> instead of Pair<ControlVolume, Point>?
    /**
     * Construct a ControlVolume with a given pressure and velocity *
     * @param pressure
     * @param velocity
     */
    ControlVolume(units::pressure::pascal_t pressure, Velocity2d velocity,
                  units::density::kg_per_cu_m_t density,
                  units::viscosity::meters_squared_per_s_t viscosity,
                  units::velocity::meters_per_second_t speed_of_sound);

    /**
     * Update this ControlVolume based on the value of it's neighbours and how much time
     * has passed
     *
     * The arguments supplied are the neighbouring ControlVolume's with their *absolute*
     * distance to this ControlVolume
     *
     * TODO: ASCII diagram here to show where each "volume" is (and +x, +y)
     *
     * @param left_neighbour_with_distance
     * @param right_neighbour_with_distance
     * @param top_neighbour_with_distance
     * @param bottom_neighbour_with_distance
     * @param dt TODO
     */
    void update(
            std::pair<ControlVolume, units::length::meter_t> left_neighbour_with_distance,
            std::pair<ControlVolume, units::length::meter_t> right_neighbour_with_distance,
            std::pair<ControlVolume, units::length::meter_t> top_neighbour_with_distance,
            std::pair<ControlVolume, units::length::meter_t> bottom_neighbour_with_distance,
            units::time::second_t dt
    );


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
    void setPressure(units::pressure::pascal_t pressure) { this->pressure = pressure; }

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
    // The density of the fluid in this ControlVolume
    units::density::kg_per_cu_m_t density;

    // The viscosity of the fluid in this ControlVolume
    units::viscosity::meters_squared_per_s_t viscosity;

    // The speed of sound in the fluid in this ControlVolume
    units::velocity::meters_per_second_t speed_of_sound;

    // The pressure in this control volume
    units::pressure::pascal_t pressure;

    // The velocity in this control volume
    Velocity2d velocity;

    // TODO: These should really be more universal values, but we
};

#endif // SIMPLE_CFD_CONTROLVOLUME_H
