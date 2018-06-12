cmake_minimum_required(VERSION 3.10)
project(simple_cfd)

##### Setup ######
set(CMAKE_CXX_STANDARD 17)

find_package(PkgConfig)

# GTest Setup
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS} include)

# GMock Setup
pkg_check_modules(GMOCK REQUIRED)
include_directories(${GMOCK_INCLUDE_DIRS})

# General Testing Setup
set(TESTING_LIBS ${GTEST_BOTH_LIBRARIES} pthread)
enable_testing()

##### GTK ######
find_package(PkgConfig REQUIRED)
pkg_check_modules("GTKMM" REQUIRED "gtkmm-3.0")
include_directories("${GTKMM_INCLUDE_DIRS}")
link_directories("${GTKMM_LIBRARY_DIRS}")

##### Executables #####
add_executable(simple_cfd main.cpp)
target_link_libraries(simple_cfd ${GTKMM_LIBRARIES})

##### Tests #####
add_executable(PressureField_test
        test/PressureField_test.cpp
        src/PressureField.cpp
        include/PressureField.h
        )
target_link_libraries(PressureField_test ${TESTING_LIBS})

add_executable(ControlVolume_test
        test/ControlVolume_test.cpp
        src/ControlVolume.cpp
        include/ControlVolume.h
        )
target_link_libraries(ControlVolume_test ${TESTING_LIBS})
