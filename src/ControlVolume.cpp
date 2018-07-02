#include "ControlVolume.h"

ControlVolume::ControlVolume() : pressure(0), velocity({0, 0}) {}

ControlVolume::ControlVolume(
    std::pair<ControlVolume, Coordinates> original_volume_with_coord,
    std::pair<ControlVolume, Coordinates> left_neighbour_with_coord,
    std::pair<ControlVolume, Coordinates> right_neighbour_with_coord,
    std::pair<ControlVolume, Coordinates> top_neighbour_with_coord,
    std::pair<ControlVolume, Coordinates> bottom_neighbour_with_coord) {
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
    Coordinates original_coord, r_coord, l_coord, t_coord, b_coord;
    std::tie(original_vol, original_coord) = original_volume_with_coord;
    std::tie(r_neighbour, r_coord)         = right_neighbour_with_coord;
    std::tie(l_neighbour, l_coord)         = left_neighbour_with_coord;
    std::tie(t_neighbour, t_coord)         = top_neighbour_with_coord;
    std::tie(b_neighbour, b_coord)         = bottom_neighbour_with_coord;

    double v_dot_x = (r_neighbour.velocity.x - original_vol.velocity.x) /
                     (r_coord.x - original_coord.x);
    double w_dot_w = (t_neighbour.velocity.y - original_vol.velocity.y) /
                     (t_coord.y - original_coord.y);
    double rho_dot_x =
        (r_neighbour.pressure - original_vol.pressure) / (r_coord.x - original_coord.x);
    double rho_dot_y =
        (t_neighbour.pressure - original_vol.pressure) / (t_coord.y - original_coord.y);
    double v_dotdot_x =
        (l_neighbour.velocity.x - 2 * original_vol.velocity.x +
         r_neighbour.velocity.x) /
        ((original_coord.x - l_coord.x) * (r_coord.x - original_coord.x));
    double w_dotdot_y =
        (b_neighbour.velocity.y - 2 * original_vol.velocity.y +
         t_neighbour.velocity.y) /
        ((original_coord.y - l_coord.y) * (r_coord.y - original_coord.y));
}

ControlVolume::ControlVolume(double pressure, Vector2d velocity)
  : pressure(pressure), velocity(velocity) {}
