cmake_minimum_required(VERSION 3.16)

# Reuse components shared by the collector and central-hub applications.
set(EXTRA_COMPONENT_DIRS "${CMAKE_CURRENT_LIST_DIR}/../components")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(scesm_collector)
