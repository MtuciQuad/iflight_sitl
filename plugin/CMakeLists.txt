cmake_minimum_required(VERSION 3.10.2 FATAL_ERROR)

find_package(gz-cmake4 REQUIRED)

project(Iflight)

gz_find_package(gz-plugin3 REQUIRED COMPONENTS register)
set(GZ_PLUGIN_VER ${gz-plugin3_VERSION_MAJOR})

gz_find_package(gz-sim9 REQUIRED)
set(GZ_SIM_VER ${gz-sim9_VERSION_MAJOR})

gz_find_package(gz-common6 REQUIRED)
set(GZ_COMMON_VER ${gz-common6_VERSION_MAJOR})

gz_find_package(gz-sensors9 REQUIRED)
set(GZ_SENSORS_VER ${gz-sensors9_VERSION_MAJOR})

add_library(Iflight SHARED Iflight)
set_property(TARGET Iflight PROPERTY CXX_STANDARD 17)
target_link_libraries(Iflight
  gz-common${GZ_COMMON_VER}::gz-common${GZ_COMMON_VER}
  gz-plugin${GZ_PLUGIN_VER}::gz-plugin${GZ_PLUGIN_VER}
  gz-sim${GZ_SIM_VER}::gz-sim${GZ_SIM_VER}
  gz-sensors${GZ_SENSORS_VER}::gz-sensors${GZ_SENSORS_VER})