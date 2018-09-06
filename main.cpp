// STD Includes
#include <iostream>

// Library Includes
#include <multi_res_graph/GraphNode.h>
#include <multi_res_graph/Rectangle.h>
#include <multi_res_graph/Circle.h>
#include <gtkmm.h>
#include <units.h>

// Project Includes
#include "FluidSimulatorRenderer.h"

using namespace units::literals;
using namespace units::pressure;
using namespace units::density;
using namespace units::viscosity;
using namespace units::velocity;
using namespace units::length;

int main(int argc, char** argv) {
    auto app = Gtk::Application::create(argc, argv, "org.gtkmm.examples.base");

    Gtk::Window window;
    window.set_default_size(200, 200);

    FluidSimulator simulator(kg_per_cu_m_t(1), meters_squared_per_s_t(1), meters_per_second_t(100), meter_t(1), 15);
    auto control_volume_graph = simulator.getControlVolumeGraph();

    // Set the pressure of some nodes on the left to a higher value
//    auto high_pressure_nodes = control_volume_graph->getAllNodesThatPassFilter(
//        [&](Node<ControlVolume>& n) { return n.getCoordinates().x <= 0.25 /*&& n.getCoordinates().x <= 0.75*/; });
//    for (auto& node : high_pressure_nodes) {
//        node->containedValue().setPressure(pascal_t(100));
//    }

    // Add a few obstacles to the simulator
//    auto obstacle1 = std::make_shared<Rectangle<ControlVolume>>(1, 7, (Coordinates){1.5, 2});
//    auto obstacle1 = std::make_shared<Circle<ControlVolume>>(1, (Coordinates){2, 3.5});
//    simulator.addObstacle(obstacle1);

    FluidSimulatorRenderer graph_renderer(simulator);
    window.add(graph_renderer);
    graph_renderer.show();

    return app->run(window);
}
