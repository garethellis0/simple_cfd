
#include <FluidSimulator.h>

#include "FluidSimulator.h"

using namespace units;
using namespace units::literals;
using namespace units::time;
using namespace units::length;
using namespace units::viscosity;
using namespace units::density;
using namespace units::pressure;
using namespace units::velocity;
using namespace units::math;

FluidSimulator::FluidSimulator(units::density::kg_per_cu_m_t density,
                               units::viscosity::meters_squared_per_s_t viscosity,
                               units::velocity::meters_per_second_t speed_of_sound,
                               units::length::meter_t simulation_size,
                               int initial_simulation_resolution)
  : density(density),
    viscosity(viscosity),
    speed_of_sound(speed_of_sound),
    control_volume_graph(std::make_shared<GraphNode<ControlVolume>>(
        initial_simulation_resolution, simulation_size.to<double>())) {
    // TODO: This is a sub-ideal way to do things... we should really just set these
    // on every ControlVolume when they are constructed with the graph, but we need
    // to add the capability to multi_res_graph for non-default constructors for
    // contained types before this will be possible
    std::vector<std::shared_ptr<RealNode<ControlVolume>>> all_nodes =
        control_volume_graph->getAllSubNodes();
    for (const std::shared_ptr<RealNode<ControlVolume>>& node : all_nodes) {
        node->containedValue() = ControlVolume(pascal_t(0),
                                               Velocity2d({0_m / 1_s, 0_m / 1_s}),
                                               density,
                                               viscosity,
                                               speed_of_sound);
    }
}

void FluidSimulator::updateControlVolumes(units::time::second_t dt) {
    auto nodes = control_volume_graph->getAllSubNodes();

    // TODO: Define as class member constants?
    // A edge volume (currently just a static wall)
    ControlVolume edge_volume(pascal_t(0),
                              Velocity2d({0_m / 1_s, 0_m / 1_s}),
                              density,
                              viscosity,
                              speed_of_sound);
    auto edge_volume_displacement = meter_t(control_volume_graph->getScale() /
                                            control_volume_graph->getResolution());

    // NOTE: This is a bit of a hack, but it'll become irrelevant eventually anyhow
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
            left_neighbour_distance = meter_t(std::abs(
                left_neighbour_ptr->getCoordinates().x - node->getCoordinates().x));
        } else {
            left_neighbour          = edge_volume;
            left_neighbour_distance = edge_volume_displacement;
            // TODO: Delete me
            left_neighbour.setVelocity({meters_per_second_t(1), meters_per_second_t(0)});
        }
        if (right_neighbour_ptr) {
            right_neighbour          = right_neighbour_ptr->containedValue();
            right_neighbour_distance = meter_t(std::abs(
                right_neighbour_ptr->getCoordinates().x - node->getCoordinates().x));
        } else {
            right_neighbour          = edge_volume;
            right_neighbour_distance = edge_volume_displacement;
            // TODO: Delete me
            right_neighbour.setVelocity({meters_per_second_t(2), meters_per_second_t(0)});
        }
        if (top_neighbour_ptr) {
            top_neighbour          = top_neighbour_ptr->containedValue();
            top_neighbour_distance = meter_t(std::abs(
                top_neighbour_ptr->getCoordinates().y - node->getCoordinates().y));
            top_neighbour.setVelocity({meters_per_second_t(0), meters_per_second_t(1)});
        } else {
            top_neighbour          = edge_volume;
            top_neighbour_distance = edge_volume_displacement;
        }
        if (bottom_neighbour_ptr) {
            bottom_neighbour          = bottom_neighbour_ptr->containedValue();
            bottom_neighbour_distance = meter_t(std::abs(
                bottom_neighbour_ptr->getCoordinates().y - node->getCoordinates().y));
        } else {
            bottom_neighbour          = edge_volume;
            bottom_neighbour_distance = edge_volume_displacement;
            bottom_neighbour.setVelocity({meters_per_second_t(0), meters_per_second_t(2)});
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

    // Set fluid velocity and pressure to 0 for all control volumes within obstacles
    for (auto& node : nodes) {
        for (auto& obstacle : obstacles) {
            if (obstacle->overlapsNode(*node)) {
                ControlVolume& control_volume = node->containedValue();
                // TODO: Make 0 X/Y velocity a constant somewhere?
                control_volume.setVelocity(
                    {meters_per_second_t(0), meters_per_second_t(0)});
                control_volume.setPressure(pascal_t(0));
                break;
            }
        }
    }
}

std::shared_ptr<GraphNode<ControlVolume>> FluidSimulator::getControlVolumeGraph() {
    return control_volume_graph;
}

void FluidSimulator::setControlVolumeGraph(
    std::shared_ptr<GraphNode<ControlVolume>> graph) {
    control_volume_graph = std::move(graph);
}

void FluidSimulator::addObstacle(std::shared_ptr<Area<ControlVolume>> obstacle) {
    obstacles.emplace_back(std::shared_ptr(obstacle->clone()));
}

std::vector<std::shared_ptr<Area<ControlVolume>>> FluidSimulator::getObstacles() {
    std::vector<std::shared_ptr<Area<ControlVolume>>> obstacles_copy;

    for (const auto& obstacle : obstacles) {
        obstacles_copy.emplace_back(std::shared_ptr(obstacle->clone()));
    }

    return obstacles_copy;
}

std::vector<Point2d> FluidSimulator::getStreamLinePoints(
    Point2d start_point, meter_t line_length, meter_t distance_between_points) {
    int num_points = ceil(line_length / distance_between_points);

    std::vector<Point2d> points;
    Point2d curr_point = start_point;

    for (int point_index = 0; point_index < num_points; point_index++) {
        points.emplace_back(curr_point);

        // Figure out what control volume we're currently in
        std::shared_ptr<RealNode<ControlVolume>> control_volume_node =
            *(control_volume_graph->getClosestNodeToCoordinates(
                {curr_point.x.to<double>(), curr_point.y.to<double>()}));
        ControlVolume& control_volume = control_volume_node->containedValue();

        // Figure out the direction the particle should be traveling in
        Velocity2d velocity = control_volume.getVelocity();
        double velocity_magnitude =
            sqrt(pow<2>(velocity.x) + pow<2>(velocity.y)).to<double>();
        if (velocity_magnitude == 0) {
            // TODO: Comment here or do this more elegantly,
            // we basically just add the same point a bunch of times to the return list
            continue;
        }
        // TODO: Better variable names here
        double velocity_x_unit = velocity.x.to<double>() / velocity_magnitude;
        double velocity_y_unit = velocity.y.to<double>() / velocity_magnitude;

        // Update the current point
        curr_point.x += velocity_x_unit * distance_between_points;
        curr_point.y += velocity_y_unit * distance_between_points;
    }

    return points;
}
