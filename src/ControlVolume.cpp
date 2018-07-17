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

ControlVolume::ControlVolume() :
        pressure(0),
        velocity({meters_per_second_t(0), meters_per_second_t(0)}),
        density(0),
        viscosity(0),
        speed_of_sound(0) {

}

ControlVolume::ControlVolume(units::pressure::pascal_t pressure, Velocity2d velocity,
                             kg_per_cu_m_t density,
                             meters_squared_per_s_t viscosity,
                             meters_per_second_t speed_of_sound) :
        pressure(pressure),
        velocity(velocity),
        density(density),
        viscosity(viscosity),
        speed_of_sound(speed_of_sound) {}

void ControlVolume::update(
        std::pair<ControlVolume, Point2d> left_neighbour_with_point,
        std::pair<ControlVolume, Point2d> right_neighbour_with_point,
        std::pair<ControlVolume, Point2d> top_neighbour_with_point,
        std::pair<ControlVolume, Point2d> bottom_neighbour_with_point,
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
    Point2d r_point, l_point, t_point, b_point;
    std::tie(r_neighbour, r_point) = right_neighbour_with_point;
    std::tie(l_neighbour, l_point) = left_neighbour_with_point;
    std::tie(t_neighbour, t_point) = top_neighbour_with_point;
    std::tie(b_neighbour, b_point) = bottom_neighbour_with_point;

    meters_per_second_t v_old = this->velocity.x;
    meters_per_second_t w_old = this->velocity.y;

    // TODO: Declare explicit types for each of these, will help catch errors
    auto v_dot_x = (r_neighbour.velocity.x - v_old) / r_point.x;
    auto w_dot_y = (t_neighbour.velocity.y - w_old) / t_point.y;

    auto p_dot_x = (r_neighbour.pressure - this->pressure) / r_point.x;
    auto p_dot_y = (t_neighbour.pressure - this->pressure) / t_point.y;

    auto v_dotdot_x = (l_neighbour.velocity.x - 2 * v_old + r_neighbour.velocity.x) /
                      ((-l_point.x) * r_point.x);
    auto v_dotdot_y = (b_neighbour.velocity.x - 2 * v_old + t_neighbour.velocity.x) /
                      ((-b_point.y) * (t_point.y));

    auto w_dotdot_x = (l_neighbour.velocity.x - 2 * v_old + r_neighbour.velocity.x) /
                      ((-l_point.x) * (r_point.x));
    auto w_dotdot_y = (b_neighbour.velocity.y - 2 * w_old + t_neighbour.velocity.y) /
                      ((-b_point.y) * (t_point.y));

    // TODO: Delete me
//    auto a = v_old - dt * (v_dot_x + w_dot_y) * v_old;
//    auto b = dt / density * p_dot_x;
//    auto c = dt * viscosity * (v_dotdot_x + v_dotdot_y);
//
//    auto d = a - b;
//    auto e = b + c;
//    auto f = a + c;
//    auto g = a - b + c;

    this->velocity.x = v_old - dt * (v_dot_x + w_dot_y) * v_old - dt / density * p_dot_x
                       + dt * viscosity * (v_dotdot_x + v_dotdot_y);
    this->velocity.y = w_old - dt * (v_dot_x + w_dot_y) * w_old - dt / density * p_dot_y
                       + dt * viscosity * (w_dotdot_x + w_dotdot_y);
    // TODO: We're just tacking on a bunch of units here.....
    // TODO: this might be correct because "c" is a constant though?????
    this->pressure -= dt * units::math::pow<2>(speed_of_sound) * (v_dot_x + w_dot_y) *
                      1_kg / units::math::pow<3>(1_m);
}
