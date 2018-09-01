// STD Includes
#include <math.h>
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
#include "FluidSimulatorRenderer.h"
#include "ControlVolume.h"

using namespace units;
using namespace units::literals;
using namespace units::time;
using namespace units::length;
using namespace units::viscosity;
using namespace units::density;
using namespace units::pressure;
using namespace units::velocity;

FluidSimulatorRenderer::FluidSimulatorRenderer(FluidSimulator simulator) :
simulator(std::move(simulator)){
    // We will update and re-draw the simulator whenever there is nothing going on
    // (and so presumably when the last update loop has finished)
    Glib::signal_idle().connect(sigc::mem_fun(*this, &FluidSimulatorRenderer::update));
}

bool FluidSimulatorRenderer::on_draw(const Cairo::RefPtr<Cairo::Context>& ctx) {
    Gtk::Allocation window_allocation = get_allocation();
    const int window_width            = window_allocation.get_width();
    const int window_height           = window_allocation.get_height();

    ctx->save();

    // Draw all the nodes in the simulator
    ctx->set_line_width(1);

    std::shared_ptr<GraphNode<ControlVolume>> control_volume_graph =
        simulator.getControlVolumeGraph();

    std::vector<std::shared_ptr<RealNode<ControlVolume>>> all_nodes =
        control_volume_graph->getAllSubNodes();

    std::vector<std::shared_ptr<Area<ControlVolume>>> obstacles =
            simulator.getObstacles();

    // The simulator should fit the smaller of the width and height
    const int graph_size  = std::min(window_width, window_height);
    double scaling_factor = graph_size / control_volume_graph->getScale();

    // Figure out the appropriate scale for the temperatures
    double max_pressure = all_nodes[0]->containedValue().getPressure().to<double>();
    for (const auto& node : all_nodes) {
        double pressure = node->containedValue().getPressure().to<double>();
        max_pressure    = std::max(pressure, max_pressure);
    }

    for (const std::shared_ptr<RealNode<ControlVolume>>& node : all_nodes) {

        // Check if this node is within an obstacle
        bool is_obstacle = false;
        for (auto& obstacle : obstacles) {
            if (obstacle->overlapsNode(*node)){
                is_obstacle = true;
                break;
            }
        }

        // Reset drawing stuff
        ctx->move_to(0, 0);

        double scaled_node_pos_x = node->getCoordinates().x * scaling_factor;
        double scaled_node_pos_y = node->getCoordinates().y * scaling_factor;
        double scaled_node_scale = node->getScale() * scaling_factor;

        // Draw the Node itself
        ctx->rectangle(
            scaled_node_pos_x, scaled_node_pos_y, scaled_node_scale, scaled_node_scale);

        // Indicate the pressure (or that this is an obstacle) by coloring the Node
        if (is_obstacle){
            ctx->set_source_rgba(0, 1, 0, 0.5);
        } else {
            auto pressure = unit_cast<double>(node->containedValue().getPressure());
            ctx->set_source_rgba(pressure / max_pressure, 0, 0, 0.8);
        }
        ctx->fill_preserve();

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

bool FluidSimulatorRenderer::update() {
    simulator.updateControlVolumes(second_t(0.00001));

    // Invalidate the entire window to force a full re-draw
    auto window = get_window();
    if (window) {
        Gdk::Rectangle r(
            0, 0, get_allocation().get_width(), get_allocation().get_height());
        window->invalidate_rect(r, false);
    }
}

void FluidSimulatorRenderer::set_simulator_to_render(FluidSimulator simulator) {
    // Save the simulator to draw
    this->simulator = std::move(simulator);
}
