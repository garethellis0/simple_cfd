// STD Includes
#include <cmath>
#include <ctime>
#include <iostream>

// External Library Includes
#include <cairomm/context.h>
#include <glibmm/main.h>
#include <sigc++/signal.h>

// Project Includes
#include "GraphRenderer.h"

GraphRenderer::GraphRenderer() : graph(std::make_shared<GraphNode<ControlVolume>>(1)) {
    //    Glib::signal_timeout().connect(sigc::mem_fun(*this,
    //    &GraphRenderer::on_timeout),
    //                                   1000);
}

bool GraphRenderer::on_draw(const Cairo::RefPtr<Cairo::Context>& ctx) {
    Gtk::Allocation window_allocation = get_allocation();
    const int window_width            = window_allocation.get_width();
    const int window_height           = window_allocation.get_height();

    // Draw the background
    ctx->save();
    //    ctx->set_source_rgba(0, 0, 0, 0.9);
    //    ctx->paint();
    //    ctx->restore();

    // Draw all the nodes in the graph
    ctx->set_line_width(1);

    std::vector<std::shared_ptr<RealNode<ControlVolume>>> all_nodes =
        this->graph->getAllSubNodes();

    // The graph should fit the smaller of the width and height
    const int graph_size  = std::min(window_width, window_height);
    double scaling_factor = graph_size / this->graph->getScale();

    for (const std::shared_ptr<RealNode<ControlVolume>>& node : all_nodes) {
        // Reset drawing stuff
        ctx->move_to(0, 0);

        double scaled_node_pos_x = node->getCoordinates().x * scaling_factor;
        double scaled_node_pos_y = node->getCoordinates().y * scaling_factor;
        double scaled_node_scale = node->getScale() * scaling_factor;

        // Draw the Node itself
        ctx->rectangle(
            scaled_node_pos_x, scaled_node_pos_y, scaled_node_scale, scaled_node_scale);

        // Indicate the pressure by coloring the Node
        double pressure = node->containedValue().getPressure();
        ctx->set_source_rgba(1.0 * pressure, 0, 0, 0.8);
        ctx->fill_preserve();
        //ctx->restore();
        //ctx->stroke_preserve();
        //ctx->clip();


        // Draw the velocity as a line
        Vector2d velocity = node->containedValue().getVelocity();
        ctx->move_to(scaled_node_pos_x + scaled_node_scale / 2,
                     scaled_node_pos_y + scaled_node_scale / 2);
        ctx->line_to(scaled_node_pos_x + scaled_node_scale / 2 + velocity.x,
                     scaled_node_pos_y + scaled_node_scale / 2 + velocity.y);

        // Indicate the pressure by coloring the cell
        // TODO

        ctx->set_source_rgba(1.0, 1.0, 1.0, 0.8);
        ctx->set_line_width(1);
        ctx->stroke();
    }

    ctx->fill_preserve();
    ctx->restore();
    ctx->stroke_preserve();
    ctx->clip();

    // Draw the Graph
    // cr->arc(0, 0, 100, 0, 2 * M_PI);
    // cr->save();
    // cr->set_source_rgba(1.0, 1.0, 1.0, 0.8);
    // cr->fill_preserve();
    // cr->restore();
    // cr->stroke_preserve();
    // cr->clip();

    return true;
}

bool GraphRenderer::on_timeout() {
    auto window = get_window();
    if (window) {
        Gdk::Rectangle r(
            0, 0, get_allocation().get_width(), get_allocation().get_height());
        window->invalidate_rect(r, false);
    }
}

void GraphRenderer::draw_graph(std::shared_ptr<GraphNode<ControlVolume>> graph) {
    // Save the graph to draw
    this->graph = graph;

    // Clear the graph and force a redraw
    // this->on_timeout();
}
