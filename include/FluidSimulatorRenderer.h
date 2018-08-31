#pragma once

// STD Includes

// External Library Includes
#include <gtkmm/drawingarea.h>
#include <multi_res_graph/GraphNode.h>
#include <units.h>

// Project Includes
#include "ControlVolume.h"
#include "FluidSimulator.h"

// TODO: Rename to `ControlVolumeGraphRenderer`?
class FluidSimulatorRenderer : public Gtk::DrawingArea {
public:
    FluidSimulatorRenderer() = delete;

    FluidSimulatorRenderer(FluidSimulator simulator);

    // TODO: Doc comment
    void set_simulator_to_render(FluidSimulator simulator);

protected:
    // Override default signal handler
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;

    // TODO: Better name
    // TODO: Doc comment
    bool update();

private:

    /**
     * Update the graph based on how much time has passed
     *
     * @param dt TODO?
     */
    void update_graph(units::time::second_t dt);

    // The FluidSimulator we're rendering
    FluidSimulator simulator;
};
