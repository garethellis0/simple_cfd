#ifndef SIMPLE_CFD_CONTROLVOLUME_H
#define SIMPLE_CFD_CONTROLVOLUME_H

// STD Includes
#include <vector>

// Project Includes
#include "PressureField.h"


// TODO: This is intentionally the the same as the one in the multi-res graph stuff, we should unify them
struct Coordinates {
    double x;
    double y;
};

// TODO: Descriptive comment here
template <typename FieldType>
class ControlVolume {
public:

    /**
     * Construct a ControlVolume from other ControlVolumes at given Coordinates
     *
     * @param control_volumes a list of ControlVolumes and associated Coordinates
     */
    ControlVolume(std::vector<std::pair<ControlVolume<FieldType>, Coordinates>> control_volumes);

private:

    // The Field contained in this ControlVolume
    FieldType field;
};


#endif //SIMPLE_CFD_CONTROLVOLUME_H
