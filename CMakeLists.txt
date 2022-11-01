# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("final_project")
set_property(GLOBAL PROPERTY USE_FOLDERS ON )
# Include sub-projects.
add_subdirectory ("final_project")
add_subdirectory ("lua-lib")