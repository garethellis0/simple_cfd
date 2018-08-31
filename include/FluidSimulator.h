#pragma once

// STD Includes
#include <memory>

// Library Includes
#include <multi_res_graph/Area.h>
#include <multi_res_graph/GraphNode.h>
#include <units.h>

// Project Includes
#include "ControlVolume.h"

// TODO: Descriptive comment here
class FluidSimulator {
  public:
    // Don't allow use of the default constructor
    FluidSimulator() = delete;

    /**
     * Construct a ControlVolumeGraph for a fluid with the given properties
     *
     * @param density TODO?
     * @param viscosity TODO?
     * @param speed_of_sound TODO?
     * @param simulation_size TODO
     * @param initial_simulation_resolution TODO
     */
    FluidSimulator(units::density::kg_per_cu_m_t density,
                   units::viscosity::meters_squared_per_s_t viscosity,
                   units::velocity::meters_per_second_t speed_of_sound,
                   units::length::meter_t simulation_size,
                   int initial_simulation_resolution);

    /**
     * Update all the control volumes based on their current values
     *
     * @param dt TODO
     */
    void updateControlVolumes(units::time::second_t dt);

    // TODO: This should return a COPY, but we need to implement deep copy for multi-res
    // graphs first
    /**
     * Gets the multi resolution graph of all the control volumes
     *
     * @return the multi resolution graph of all the control volumes
     */
    std::shared_ptr<GraphNode<ControlVolume>> getControlVolumeGraph();

    // TODO: We should be passing in an object, not a pointer, but we need to implement deep copy for multi-res graphs first
    /**
     * Sets the multi resolution graph of all the control volumes
     */
    void setControlVolumeGraph(std::shared_ptr<GraphNode<ControlVolume>> graph);

  private:
    // The density of the fluid
    units::density::kg_per_cu_m_t density;

    // The viscosity of the fluid
    units::viscosity::meters_squared_per_s_t viscosity;

    // The speed of sound in the fluid
    units::velocity::meters_per_second_t speed_of_sound;

    // The actual simulator the holds all the control volumes
    std::shared_ptr<GraphNode<ControlVolume>> control_volume_graph;

    // TODO: Better comment here
    // Solid obstacles that may overlap control volume(s)
    std::vector<std::shared_ptr<Area<ControlVolume>>> obstacles;
};
