#include <iostream>
#include <gtkmm.h>
#include "multi_res_graph/GraphNode.h"
#include "GraphRenderer.h"

int main(int argc, char** argv){

    auto app = Gtk::Application::create(argc, argv, "org.gtkmm.examples.base");

    Gtk::Window window;
    window.set_default_size(200, 200);

    GraphRenderer graph_renderer;
    window.add(graph_renderer);
    graph_renderer.show();

    auto graph = std::make_shared<GraphNode<ControlVolume>>(4, 200);
    auto subnodes = graph->getAllSubNodes();
    subnodes[0]->convertToGraphNode(5);
    subnodes[1]->containedValue() = ControlVolume(10, {40, 40});

    graph_renderer.draw_graph(graph);

    return app->run(window);
}
