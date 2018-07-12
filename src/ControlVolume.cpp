#include "ControlVolume.h"

using namespace units::literals;

ControlVolume::ControlVolume() : pressure(0), velocity({0_m / 1_s, 0_m / 1_s}) {}

ControlVolume::ControlVolume(
    std::pair<ControlVolume, Point2d> original_volume_with_point,
    std::pair<ControlVolume, Point2d> left_neighbour_with_point,
    std::pair<ControlVolume, Point2d> right_neighbour_with_point,
    std::pair<ControlVolume, Point2d> top_neighbour_with_point,
    std::pair<ControlVolume, Point2d> bottom_neighbour_with_point) {
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

    // TODO: Declare explicit types for each of these, will help catch errors
    auto v_dot_x = (r_neighbour.velocity.x - original_vol.velocity.x) /
                     (r_point.x - original_point.x);
    auto w_dot_w = (t_neighbour.velocity.y - original_vol.velocity.y) /
                     (t_point.y - original_point.y);
    auto rho_dot_x =
        (r_neighbour.pressure - original_vol.pressure) / (r_point.x - original_point.x);
    auto rho_dot_y =
        (t_neighbour.pressure - original_vol.pressure) / (t_point.y - original_point.y);
    auto v_dotdot_x =
        (l_neighbour.velocity.x - 2 * original_vol.velocity.x +
         r_neighbour.velocity.x) /
        ((original_point.x - l_point.x) * (r_point.x - original_point.x));
    auto w_dotdot_y =
        (b_neighbour.velocity.y - 2 * original_vol.velocity.y +
         t_neighbour.velocity.y) /
        ((original_point.y - l_point.y) * (r_point.y - original_point.y));
}

ControlVolume::ControlVolume(double pressure, Velocity2d velocity)
  : pressure(pressure), velocity(velocity) {}
