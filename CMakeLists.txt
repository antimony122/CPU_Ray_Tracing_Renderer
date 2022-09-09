cmake_minimum_required(VERSION 3.10)
project(TinyRayTracing)

set(CMAKE_CXX_STANDARD 17)
set(GLM_INCLUDE_DIRS ${PROJECT_SOURCE_DIR})
list( APPEND SUMMARY_DEBUG_INCLUDE ${GLM_INCLUDE_DIRS})
include_directories( ${SUMMARY_DEBUG_INCLUDE})
add_subdirectory(glm)
add_executable(TinyRayTracing main.cpp aabb.hpp BVH.hpp camera.hpp hittable_list.hpp hittable.hpp material.hpp OBJ_Loader.hpp ppmWrite.hpp ray.hpp sphere.hpp texture.hpp triangle.hpp utility.hpp glm/glm.hpp)