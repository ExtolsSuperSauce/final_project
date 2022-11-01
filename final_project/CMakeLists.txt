# CMakeList.txt : CMake project for final_project, include source and define
# project specific logic here.
project (final_project)
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable (final_project "final_project.cpp" "final_project.h")
target_link_libraries( final_project PUBLIC LuaLib )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET final_project PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
