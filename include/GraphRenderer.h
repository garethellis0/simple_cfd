#ifndef SIMPLE_CFD_GRAPHRENDERER_H
#define SIMPLE_CFD_GRAPHRENDERER_H

// STD Includes

// External Library Includes
#include <gtkmm/drawingarea.h>
#include <multi_res_graph/GraphNode.h>

// Project Includes
#include "ControlVolume.h"

// TODO: Rename to `ControlVolumeGraphRenderer`?
class GraphRenderer : public Gtk::DrawingArea {
public:
    GraphRenderer();

    // TODO: Doc comment
    void draw_graph(std::shared_ptr<GraphNode<ControlVolume>> graph);

protected:
    // Override default signal handler
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;

    // TODO: Better name
    // TODO: Doc comment
    bool on_timeout();

private:

    // The top level node for the graph
    std::shared_ptr<GraphNode<ControlVolume>> graph;
};


#endif //SIMPLE_CFD_GRAPHRENDERER_H
