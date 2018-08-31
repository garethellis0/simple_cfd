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
using namespace units::viscosity;
using namespace units::density;
using namespace units::pressure;
using namespace units::velocity;

GraphRenderer::GraphRenderer() :
graph(std::make_shared<GraphNode<ControlVolume>>(1))
{
    // We will update and re-draw the graph whenever there is nothing going on
    // (and so presumably when the last update loop has finished)
    Glib::signal_idle().connect(sigc::mem_fun(*this, &GraphRenderer::update));
}

bool GraphRenderer::on_draw(const Cairo::RefPtr<Cairo::Context>& ctx) {
    Gtk::Allocation window_allocation = get_allocation();
    const int window_width            = window_allocation.get_width();
    const int window_height           = window_allocation.get_height();

    ctx->save();

    // Draw all the nodes in the graph
    ctx->set_line_width(1);

    std::vector<std::shared_ptr<RealNode<ControlVolume>>> all_nodes =
        this->graph->getAllSubNodes();

    // The graph should fit the smaller of the width and height
    const int graph_size  = std::min(window_width, window_height);
    double scaling_factor = graph_size / this->graph->getScale();

    // Figure out the appropriate scale for the temperatures
    double max_pressure = all_nodes[0]->containedValue().getPressure().to<double>();
    for (const auto& node : all_nodes) {
        double pressure = node->containedValue().getPressure().to<double>();
        max_pressure = std::max(pressure, max_pressure);
    }

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
        ctx->set_source_rgba(pressure / max_pressure, 0, 0, 0.8);
        ctx->fill_preserve();
        // ctx->restore();
        // ctx->stroke_preserve();
        // ctx->clip();

        // Draw the velocity as a line
        Velocity2d velocity = node->containedValue().getVelocity();
        auto velocity_magnitude =
            units::math::pow<2>(velocity.x) + units::math::pow<2>(velocity.y);
        if (velocity_magnitude.to<double>() != 0) {
            velocity.x = velocity.x / velocity_magnitude.to<double>();
            velocity.y = velocity.y / velocity_magnitude.to<double>();
        }
        ctx->move_to(scaled_node_pos_x + scaled_node_scale / 2,
                     scaled_node_pos_y + scaled_node_scale / 2);
        ctx->line_to(
            scaled_node_pos_x + scaled_node_scale / 2 + unit_cast<double>(velocity.x),
            scaled_node_pos_y + scaled_node_scale / 2 + unit_cast<double>(velocity.y));

        ctx->set_source_rgba(1.0, 1.0, 1.0, 0.8);
        ctx->set_line_width(1);
        ctx->stroke();
    }

    ctx->fill_preserve();
    ctx->restore();
    ctx->stroke_preserve();
    ctx->clip();

    return true;
}

bool GraphRenderer::update() {
    update_graph(second_t(0.00001));

    // Invalidate the entire window to force a full re-draw
    auto window = get_window();
    if (window) {
        Gdk::Rectangle r(
            0, 0, get_allocation().get_width(), get_allocation().get_height());
        window->invalidate_rect(r, false);
    }
}

void GraphRenderer::set_graph(std::shared_ptr<GraphNode<ControlVolume>> graph) {
    // Save the graph to draw
    this->graph = std::move(graph);
}

void GraphRenderer::update_graph(units::time::second_t dt) {
    auto nodes = graph->getAllSubNodes();

    // A edge volume
    ControlVolume edge_volume(pascal_t(0),
                              Velocity2d({0_m / 1_s, 0_m / 1_s}),
                              kg_per_cu_m_t(1),
                              meters_squared_per_s_t(1),
                              meters_per_second_t(343));
    meter_t edge_volume_displacement =
        meter_t(graph->getScale() / graph->getResolution());

    // NOTE: This is a bit of a hack, but it'll become irrelevent eventually anyhow
    for (auto& node : nodes) {
        node->containedValue().new_pressure = node->containedValue().getPressure();
        node->containedValue().new_velocity = node->containedValue().getVelocity();
    }

    for (auto& node : nodes) {
        std::shared_ptr<RealNode<ControlVolume>> left_neighbour_ptr =
            node->getLeftNeighbour();
        std::shared_ptr<RealNode<ControlVolume>> right_neighbour_ptr =
            node->getRightNeighbour();
        std::shared_ptr<RealNode<ControlVolume>> top_neighbour_ptr =
            node->getTopNeighbour();
        std::shared_ptr<RealNode<ControlVolume>> bottom_neighbour_ptr =
            node->getBottomNeighbour();

        ControlVolume left_neighbour, right_neighbour, top_neighbour, bottom_neighbour;
        meter_t left_neighbour_distance, right_neighbour_distance,
            top_neighbour_distance, bottom_neighbour_distance;

        // TODO: This is gross, clean it up
        // If we couldn't find a neighbour, then it's an edge case
        if (left_neighbour_ptr) {
            left_neighbour          = left_neighbour_ptr->containedValue();
            left_neighbour_distance = meter_t(
                abs(left_neighbour_ptr->getCoordinates().x - node->getCoordinates().x));
        } else {
            left_neighbour          = edge_volume;
            left_neighbour_distance = edge_volume_displacement;
        }
        if (right_neighbour_ptr) {
            right_neighbour          = right_neighbour_ptr->containedValue();
            right_neighbour_distance = meter_t(abs(
                right_neighbour_ptr->getCoordinates().x - node->getCoordinates().x));
        } else {
            right_neighbour          = edge_volume;
            right_neighbour_distance = edge_volume_displacement;
        }
        if (top_neighbour_ptr) {
            top_neighbour          = top_neighbour_ptr->containedValue();
            top_neighbour_distance = meter_t(
                abs(top_neighbour_ptr->getCoordinates().y - node->getCoordinates().y));
        } else {
            top_neighbour          = edge_volume;
            top_neighbour_distance = edge_volume_displacement;
        }
        if (bottom_neighbour_ptr) {
            bottom_neighbour          = bottom_neighbour_ptr->containedValue();
            bottom_neighbour_distance = meter_t(abs(
                bottom_neighbour_ptr->getCoordinates().y - node->getCoordinates().y));
        } else {
            bottom_neighbour          = edge_volume;
            bottom_neighbour_distance = edge_volume_displacement;
        }

        // Figure out new values for the volume
        ControlVolume new_volume = node->containedValue();
        new_volume.update(std::make_pair(left_neighbour, left_neighbour_distance),
                          std::make_pair(right_neighbour, right_neighbour_distance),
                          std::make_pair(top_neighbour, top_neighbour_distance),
                          std::make_pair(bottom_neighbour, bottom_neighbour_distance),
                          dt);
        node->containedValue().new_pressure = new_volume.getPressure();
        node->containedValue().new_velocity = new_volume.getVelocity();

    }

    // After figuring out new values for every control volume, update them all
    for (auto& node : nodes) {
        node->containedValue().setPressure(node->containedValue().new_pressure);
        node->containedValue().setVelocity(node->containedValue().new_velocity);
    }
}
