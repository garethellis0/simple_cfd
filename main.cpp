// STD Includes
#include <iostream>

// Library Includes
#include "multi_res_graph/GraphNode.h"
#include <gtkmm.h>
#include <units.h>

// Project Includes
#include "GraphRenderer.h"

using namespace units::literals;
using namespace units::pressure;

int main(int argc, char** argv) {
    auto app = Gtk::Application::create(argc, argv, "org.gtkmm.examples.base");

    Gtk::Window window;
    window.set_default_size(200, 200);

    GraphRenderer graph_renderer;
    window.add(graph_renderer);
    graph_renderer.show();

    auto graph = std::make_shared<GraphNode<ControlVolume>>(10, 10);

    // Set the pressure of some nodes on the left to a higher value
    auto high_pressure_nodes = graph->getAllNodesThatPassFilter(
        [&](Node<ControlVolume>& n) { return n.getCoordinates().x <= 0.25 /*&& n.getCoordinates().x <= 0.75*/; });
    for (auto& node : high_pressure_nodes) {
        node->containedValue().setPressure(pascal_t(1));
    }

    // auto subnodes = graph->getAllSubNodes();
    // subnodes[0]->convertToGraphNode(5);
    // Velocity2d v = {40_m / 1_s, 40_m / 1_s};
    // subnodes[1]->containedValue() = ControlVolume(10, v);

    graph_renderer.draw_graph(graph);

    return app->run(window);
}
