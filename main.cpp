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

    auto graph = std::make_shared<GraphNode<ControlVolume>>(20, 10);

    // Set the pressure of some nodes on the left to a higher value
    auto high_pressure_nodes = graph->getAllNodesThatPassFilter(
        [&](Node<ControlVolume>& n) { return n.getCoordinates().x <= 0.25 /*&& n.getCoordinates().x <= 0.75*/; });
    for (auto& node : high_pressure_nodes) {
        node->containedValue().setPressure(pascal_t(1));
    }

    graph_renderer.set_graph(graph);
    graph_renderer.show();

    return app->run(window);
}
