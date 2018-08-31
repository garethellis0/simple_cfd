#pragma once

// STD Includes

// External Library Includes
#include <gtkmm/drawingarea.h>
#include <multi_res_graph/GraphNode.h>
#include <units.h>

// Project Includes
#include "ControlVolume.h"

// TODO: Rename to `ControlVolumeGraphRenderer`?
class GraphRenderer : public Gtk::DrawingArea {
public:
    GraphRenderer();

    // TODO: Doc comment
    void set_graph(std::shared_ptr<GraphNode<ControlVolume>> graph);

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

    // The top level node for the graph
    std::shared_ptr<GraphNode<ControlVolume>> graph;
};
