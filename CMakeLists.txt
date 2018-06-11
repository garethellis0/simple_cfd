cmake_minimum_required(VERSION 3.10)
project(simple_cfd)

set(CMAKE_CXX_STANDARD 17)

##### GTK ######
find_package(PkgConfig REQUIRED)
pkg_check_modules("GTKMM" REQUIRED "gtkmm-3.0")
include_directories("${GTKMM_INCLUDE_DIRS}")
link_directories("${GTKMM_LIBRARY_DIRS}")

add_executable(simple_cfd main.cpp)
target_link_libraries(simple_cfd ${GTKMM_LIBRARIES})