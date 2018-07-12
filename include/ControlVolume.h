#ifndef SIMPLE_CFD_CONTROLVOLUME_H
#define SIMPLE_CFD_CONTROLVOLUME_H

// STD Includes
#include <vector>

// Library Includes
#include <units.h>

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
     * Construct a ControlVolume with a given pressure and velocity
     *
     * @param pressure
     * @param velocity
     */
    ControlVolume(double pressure, Velocity2d velocity);

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
     */
    ControlVolume(std::pair<ControlVolume, Point2d> original_volume_with_point,
                  std::pair<ControlVolume, Point2d> left_neighbour_with_point,
                  std::pair<ControlVolume, Point2d> right_neighbour_with_point,
                  std::pair<ControlVolume, Point2d> top_neighbour_with_point,
                  std::pair<ControlVolume, Point2d> bottom_neighbour_with_point);

    /**
     * Get the current pressure in this ControlVolume
     *
     * @return TODO?
     */
    units::pressure::pascal_t getPressure(){
        return this->pressure;
    }

    /**
     * Get the current velocity in this ControlVolume
     *
     * @return TODO?
     */
    Velocity2d getVelocity(){
        return this->velocity;
    }

  private:
    // TODO: Units?? Maybe indicate on variable name (or typedef)
    // The pressure in this control volume
    units::pressure::pascal_t pressure;

    // TODO: Units? Maybe indicate on variable name (or typedef)
    // The velocity in this control volume
    Velocity2d velocity;
};

#endif // SIMPLE_CFD_CONTROLVOLUME_H
