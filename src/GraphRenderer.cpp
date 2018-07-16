// STD Includes
#include <cmath>
#include <ctime>
#include <experimental/optional>
#include <iostream>

// External Library Includes
#include <cairomm/context.h>
#include <glibmm/main.h>
#include <sigc++/signal.h>
#include <units.h>

// Project Includes
#include "GraphRenderer.h"

using namespace units;
using namespace units::literals;
using namespace units::time;
using namespace units::length;

GraphRenderer::GraphRenderer() : graph(std::make_shared<GraphNode<ControlVolume>>(1)) {
    Glib::signal_timeout().connect(sigc::mem_fun(*this, &GraphRenderer::on_timeout),
                                   1000);
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
        double pressure = unit_cast<double>(node->containedValue().getPressure());
        ctx->set_source_rgba(0.001 * pressure, 0, 0, 0.8);
        ctx->fill_preserve();
        // ctx->restore();
        // ctx->stroke_preserve();
        // ctx->clip();

        // Draw the velocity as a line
        Velocity2d velocity = node->containedValue().getVelocity();
        auto velocity_magnitude = units::math::pow<2>(velocity.x) + units::math::pow<2>(velocity.y);
        if (velocity_magnitude.to<double>() != 0){
            velocity.x = velocity.x / velocity_magnitude.to<double>();
            velocity.y = velocity.y / velocity_magnitude.to<double>();
        }
        ctx->move_to(scaled_node_pos_x + scaled_node_scale / 2,
                     scaled_node_pos_y + scaled_node_scale / 2);
        ctx->line_to(
            scaled_node_pos_x + scaled_node_scale / 2 + unit_cast<double>(velocity.x),
            scaled_node_pos_y + scaled_node_scale / 2 + unit_cast<double>(velocity.y));

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
    update_graph(second_t(0.00001));

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

void GraphRenderer::update_graph(units::time::second_t dt) {
    auto nodes = graph->getAllSubNodes();

    // A edge node
    ControlVolume edge_volume(0, Velocity2d({0_m / 1_s, 0_m / 1_s}));

    // TODO: this is all a bit of a hack
    for (auto& node : nodes) {
        auto neighbours = node->getNeighbours();

        // Look for neighbours
        std::experimental::optional<ControlVolume> left_neighbour_op,
            right_neighbour_op, top_neighbour_op, bottom_neighbour_op;
        Point2d left_neighbour_point, right_neighbour_point, top_neighbour_point,
            bottom_neighbour_point;
        for (auto& neighbour : neighbours) {
            Point2d neighbour_point({
                meter_t(neighbour->getCoordinates().x),
                meter_t(neighbour->getCoordinates().y),
            });

            if (neighbour->getCoordinates().x < node->getCoordinates().x) {
                left_neighbour_op    = ControlVolume(neighbour->containedValue());
                left_neighbour_point = neighbour_point;
            }

            if (neighbour->getCoordinates().x > node->getCoordinates().x) {
                right_neighbour_op    = ControlVolume(neighbour->containedValue());
                right_neighbour_point = neighbour_point;
            }

            if (neighbour->getCoordinates().y < node->getCoordinates().y) {
                bottom_neighbour_op    = ControlVolume(neighbour->containedValue());
                bottom_neighbour_point = neighbour_point;
            }

            if (neighbour->getCoordinates().y > node->getCoordinates().y) {
                top_neighbour_op    = ControlVolume(neighbour->containedValue());
                top_neighbour_point = neighbour_point;
            }
        }

        // If we couldn't find a neighbour, then it's an edge case
        ControlVolume left_neighbour, right_neighbour, top_neighbour, bottom_neighbour;
        left_neighbour   = left_neighbour_op ? *left_neighbour_op : edge_volume;
        right_neighbour  = right_neighbour_op ? *right_neighbour_op : edge_volume;
        bottom_neighbour = bottom_neighbour_op ? *bottom_neighbour_op : edge_volume;
        top_neighbour    = top_neighbour_op ? *top_neighbour_op : edge_volume;

        ControlVolume original_volume(node->containedValue());
        Point2d original_volume_point({
                                              meter_t(node->getCoordinates().x), meter_t(node->getCoordinates().y),
                                      });

        // Take a guess for where the boundry nodes should be
        if (!left_neighbour_op){
            left_neighbour_point = original_volume_point;
            left_neighbour_point.x -= meter_t(graph->getScale()/graph->getResolution());
        }
        if (!right_neighbour_op){
            right_neighbour_point = original_volume_point;
            right_neighbour_point.x += meter_t(graph->getScale()/graph->getResolution());
        }
        if (!bottom_neighbour_op){
            bottom_neighbour_point = original_volume_point;
            bottom_neighbour_point.y -= meter_t(graph->getScale()/graph->getResolution());
        }
        if (!top_neighbour_op){
            top_neighbour_point = original_volume_point;
            top_neighbour_point.y += meter_t(graph->getScale()/graph->getResolution());
        }

        // Figure out new values for the volume
        ControlVolume new_volume(std::make_pair(original_volume, original_volume_point),
                                 std::make_pair(left_neighbour, left_neighbour_point),
                                 std::make_pair(right_neighbour, right_neighbour_point),
                                 std::make_pair(top_neighbour, top_neighbour_point),
                                 std::make_pair(bottom_neighbour, bottom_neighbour_point),
                                 units::time::second_t(0.1),
                                 units::density::kg_per_cu_m_t(1000),
                                 units::viscosity::meters_squared_per_s_t(1.8),
                                 units::velocity::meters_per_second_t(343));
        node->containedValue().new_pressure = new_volume.getPressure();
        node->containedValue().new_velocity = new_volume.getVelocity();
    }

    // After figuring out new values for every volume, update them all
    for (auto& node : nodes) {
        node->containedValue().setPressure(node->containedValue().new_pressure);
        node->containedValue().setVelocity(node->containedValue().new_velocity);
    }

}
