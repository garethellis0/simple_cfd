#include "ControlVolume.h"

using namespace units::literals;
using namespace units::length;
using namespace units::velocity;
using namespace units::acceleration;
using namespace units::time;
using namespace units::pressure;
using namespace units::density;
using namespace units::viscosity;
using namespace units::math;

ControlVolume::ControlVolume()
  : pressure(0),
    velocity({meters_per_second_t(0), meters_per_second_t(0)}),
    // TODO: These should all be 0, should be set by user
    density(1),
    viscosity(1),
    speed_of_sound(1) {}

ControlVolume::ControlVolume(units::pressure::pascal_t pressure,
                             Velocity2d velocity,
                             kg_per_cu_m_t density,
                             meters_squared_per_s_t viscosity,
                             meters_per_second_t speed_of_sound)
  : pressure(pressure),
    velocity(velocity),
    density(density),
    viscosity(viscosity),
    speed_of_sound(speed_of_sound) {}

void ControlVolume::update(
    std::pair<ControlVolume, meter_t> left_neighbour_with_distance,
    std::pair<ControlVolume, meter_t> right_neighbour_with_distance,
    std::pair<ControlVolume, meter_t> top_neighbour_with_distance,
    std::pair<ControlVolume, meter_t> bottom_neighbour_with_distance,
    units::time::second_t dt) {
    // TODO: Should these conventions be somewhere else? Maybe put in README?
    /**
     * CONVENTIONS:
     * "top" is positive y, "right" is positive x
     * velocity: u = <v,w> = <velocity in x direction, velocity in y direction>
     * pressure: p
     * rho: density
     * v_dot: the first derivative of v
     * v_dot_x: the first derivative of v with respect to x
     * v_dotdot_x: the second derivative of v with respect to x
     * (TODO: make more clear)
     * v_dotdot_xy: the second derivative of v with respect to x and y
     */

    ControlVolume r_neighbour, l_neighbour, t_neighbour, b_neighbour;
    meter_t r_distance, l_distance, t_distance, b_distance;
    std::tie(r_neighbour, r_distance) = right_neighbour_with_distance;
    std::tie(l_neighbour, l_distance) = left_neighbour_with_distance;
    std::tie(t_neighbour, t_distance) = top_neighbour_with_distance;
    std::tie(b_neighbour, b_distance) = bottom_neighbour_with_distance;

    //    meters_per_second_t v_old = this->velocity.x;
    //    meters_per_second_t w_old = this->velocity.y;

    // TODO: Declare explicit types for each of these, will help catch errors
    auto v_dot_x = (r_neighbour.velocity.x - this->velocity.x) / r_distance;
    auto w_dot_y = (t_neighbour.velocity.y - this->velocity.y) / t_distance;

    auto p_dot_x = (r_neighbour.pressure - this->pressure) / r_distance;
    auto p_dot_y = (t_neighbour.pressure - this->pressure) / t_distance;

    auto v_dotdot_x =
        (l_neighbour.velocity.x - 2 * this->velocity.x + r_neighbour.velocity.x) /
        (abs(l_distance) * abs(r_distance));
    auto v_dotdot_y =
        (b_neighbour.velocity.x - 2 * this->velocity.x + t_neighbour.velocity.x) /
        (abs(b_distance) * abs(t_distance));

    auto w_dotdot_x =
        (l_neighbour.velocity.y - 2 * this->velocity.y + r_neighbour.velocity.y) /
        (abs(l_distance) * abs(r_distance));
    auto w_dotdot_y =
        (b_neighbour.velocity.y - 2 * this->velocity.y + t_neighbour.velocity.y) /
        (abs(b_distance) * abs(t_distance));

    this->velocity.x = this->velocity.x - dt * (v_dot_x + w_dot_y) * this->velocity.x -
                       dt / density * p_dot_x +
                       dt * viscosity * (v_dotdot_x + v_dotdot_y);
    this->velocity.y = this->velocity.y - dt * (v_dot_x + w_dot_y) * this->velocity.y -
                       dt / density * p_dot_y +
                       dt * viscosity * (w_dotdot_x + w_dotdot_y);
    // TODO: We're just tacking on a bunch of units here.....
    // TODO: this might be correct because "c" is a constant though?????
    this->pressure -= dt * units::math::pow<2>(speed_of_sound) * (v_dot_x + w_dot_y) *
                      1_kg / units::math::pow<3>(1_m);

    // TODO: We should be checking for invalid input values (like distances <= 0) and
    // throwing appropriate exceptions

    // TODO: We should be checking for calculation instabilities
    // (basically absurdly large numbers arising out of stuff like have too large
    // of a time step), as these can cause crashes (maybe throw an exception)?
}
