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

ControlVolume::ControlVolume() : pressure(0), velocity({0_m / 1_s, 0_m / 1_s}) {}

ControlVolume::ControlVolume(
    std::pair<ControlVolume, Point2d> original_volume_with_point,
    std::pair<ControlVolume, Point2d> left_neighbour_with_point,
    std::pair<ControlVolume, Point2d> right_neighbour_with_point,
    std::pair<ControlVolume, Point2d> top_neighbour_with_point,
    std::pair<ControlVolume, Point2d> bottom_neighbour_with_point,
    second_t dt,
    kg_per_cu_m_t density,
    meters_squared_per_s_t viscosity,
    meters_per_second_t speed_of_sound) {
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

    ControlVolume original_vol, r_neighbour, l_neighbour, t_neighbour, b_neighbour;
    Point2d original_point, r_point, l_point, t_point, b_point;
    std::tie(original_vol, original_point) = original_volume_with_point;
    std::tie(r_neighbour, r_point)         = right_neighbour_with_point;
    std::tie(l_neighbour, l_point)         = left_neighbour_with_point;
    std::tie(t_neighbour, t_point)         = top_neighbour_with_point;
    std::tie(b_neighbour, b_point)         = bottom_neighbour_with_point;

    meters_per_second_t v_old = original_vol.velocity.x;
    meters_per_second_t w_old = original_vol.velocity.y;

    // TODO: Declare explicit types for each of these, will help catch errors
    auto v_dot_x = (r_neighbour.velocity.x - v_old) / (r_point.x - original_point.x);
    auto w_dot_y = (t_neighbour.velocity.y - w_old) / (t_point.y - original_point.y);

    auto p_dot_x =
        (r_neighbour.pressure - original_vol.pressure) / (r_point.x - original_point.x);
    auto p_dot_y =
        (t_neighbour.pressure - original_vol.pressure) / (t_point.y - original_point.y);

    auto v_dotdot_x = (l_neighbour.velocity.x - 2 * v_old + r_neighbour.velocity.x) /
                      ((original_point.x - l_point.x) * (r_point.x - original_point.x));
    auto v_dotdot_y = (b_neighbour.velocity.x - 2 * v_old + t_neighbour.velocity.x) /
                      ((original_point.y - b_point.y) * (t_point.y - original_point.y));

    auto w_dotdot_x = (l_neighbour.velocity.x - 2 * v_old + r_neighbour.velocity.x) /
                      ((original_point.x - l_point.x) * (r_point.x - original_point.x));
    auto w_dotdot_y = (b_neighbour.velocity.y - 2 * w_old + t_neighbour.velocity.y) /
                      ((original_point.y - b_point.y) * (t_point.y - original_point.y));

    auto a = v_old - dt * (v_dot_x + w_dot_y) * v_old;
    auto b = dt / density * p_dot_x;
    auto c = dt * viscosity * (v_dotdot_x + v_dotdot_y);

    auto d = a - b;
    auto e = b + c;
    auto f = a + c;
    auto g = a - b + c;

    this->velocity.x = v_old - dt * (v_dot_x + w_dot_y) * v_old -
                       dt / density * p_dot_x +
                       dt * viscosity * (v_dotdot_x + v_dotdot_y);
    this->velocity.y = w_old - dt * (v_dot_x + w_dot_y) * w_old -
                       dt / density * p_dot_y +
                       dt * viscosity * (w_dotdot_x + w_dotdot_y);
    // TODO: We're just tacking on a bunch of units here.....
    // TODO: this might be correct because "c" is a constant though?????
    this->pressure = original_vol.getPressure() -
                     dt * units::math::pow<2>(speed_of_sound) * (v_dot_x + w_dot_y) * 1_kg / units::math::pow<3>(1_m);
}

ControlVolume::ControlVolume(double pressure, Velocity2d velocity)
  : pressure(pressure), velocity(velocity) {}
