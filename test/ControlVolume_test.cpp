#include "ControlVolume.h"
#include <gtest/gtest.h>

using namespace units::literals;
using namespace units::length;
using namespace units::velocity;
using namespace units::acceleration;
using namespace units::time;
using namespace units::pressure;
using namespace units::density;
using namespace units::viscosity;
using namespace units::math;

class ControlVolumeTest : public testing::Test {
  public:
    /**
     * Updates the center volume with the left, right, top, bottom volumes
     *
     * Uses the given displacements for each volume and the given update interval
     *
     * @param dt
     * @param d_left
     * @param d_right
     * @param d_top
     * @param d_bottom
     */
    void updateCenterVolume(
        second_t dt = second_t(0.1), meter_t d_left = meter_t(0.1), meter_t d_right = meter_t(0.1), meter_t d_top = meter_t(0.1), meter_t d_bottom = meter_t(0.1)) {
        center_volume.update(std::make_pair(left_volume, d_left),
                             std::make_pair(right_volume, d_right),
                             std::make_pair(top_volume, d_top),
                             std::make_pair(bottom_volume, d_bottom),
                             dt);
    }

  protected:
    virtual void SetUp() {
        default_volume =
            ControlVolume(pascal_t(0),
                          Velocity2d{meters_per_second_t(0), meters_per_second_t(0)},
                          kg_per_cu_m_t(1),
                          meters_squared_per_s_t(1),
                          meters_per_second_t(1));
        left_volume   = default_volume;
        right_volume  = default_volume;
        top_volume    = default_volume;
        center_volume = default_volume;
    }

    // TODO: Make `default_volume` const somehow?
    ControlVolume default_volume, left_volume, right_volume, top_volume, bottom_volume,
        center_volume;
};

// Test updating a Volume with:
// - non-zero pressure on right, zero pressure everywhere else
// - zero velocity everywhere
TEST_F(ControlVolumeTest, update_right_volume_high_pressure) {
    right_volume.setPressure(pascal_t(10));

    updateCenterVolume();

    EXPECT_LT(center_volume.getVelocity().x.to<double>(), 0);
    EXPECT_DOUBLE_EQ(0, center_volume.getVelocity().y.to<double>());
    EXPECT_DOUBLE_EQ(0, center_volume.getPressure().to<double>());

    updateCenterVolume();

    EXPECT_GT(center_volume.getVelocity().x.to<double>(), 0);
    EXPECT_DOUBLE_EQ(0, center_volume.getVelocity().y.to<double>());
    EXPECT_LT(center_volume.getPressure().to<double>(), 0);
}

// Test updating a Volume with:
// - non-zero pressure on left, zero pressure everywhere else
// - zero velocity everywhere
// TODO

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
