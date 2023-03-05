# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# set(EXTRA_COMPONENT_DIRS    "../components"
# $ENV{IDF_PATH}/examples/common_components/protocol_examples_common
# )

# add_compile_options(-fdiagnostics-color=always)
project(lvgl_esp8266 VERSION 0.0.1 LANGUAGES C CXX)
configure_file(/main/inc/version.h.in "${PROJECT_SOURCE_DIR}/main/inc/version.h")