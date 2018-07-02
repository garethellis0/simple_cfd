#ifndef SIMPLE_CFD_CONTROLVOLUME_H
#define SIMPLE_CFD_CONTROLVOLUME_H

// STD Includes
#include <vector>

// Project Includes
// TODO: Should we be including this just for the `Coordinates` struct
#include <multi_res_graph/Node.h>

// TODO: Better name?
struct Vector2d {
    double x;
    double y;
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
    ControlVolume(double pressure, Vector2d velocity);

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
    ControlVolume(std::pair<ControlVolume, Coordinates> original_volume_with_coord,
                  std::pair<ControlVolume, Coordinates> left_neighbour_with_coord,
                  std::pair<ControlVolume, Coordinates> right_neighbour_with_coord,
                  std::pair<ControlVolume, Coordinates> top_neighbour_with_coord,
                  std::pair<ControlVolume, Coordinates> bottom_neighbour_with_coord);

    /**
     * Get the current pressure in this ControlVolume
     *
     * @return TODO?
     */
    double getPressure(){
        return this->pressure;
    }

    /**
     * Get the current velocity in this ControlVolume
     *
     * @return TODO?
     */
    Vector2d getVelocity(){
        return this->velocity;
    }

  private:
    // TODO: Units?? Maybe indicate on variable name (or typedef)
    // The pressure in this control volume
    double pressure;

    // TODO: Units? Maybe indicate on variable name (or typedef)
    // The velocity in this control volume
    Vector2d velocity;
};

#endif // SIMPLE_CFD_CONTROLVOLUME_H
