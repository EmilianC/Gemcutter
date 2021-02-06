cmake_minimum_required(VERSION 3.16)

project(Gemcutter LANGUAGES C CXX)

find_package(OpenGL REQUIRED)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/external/scaffold")
include(scaffold)
sf_is_project_top_level(IS_TOP_LEVEL)

if (NOT IS_TOP_LEVEL)
	set(ASSET_WORKSPACE_DIRECTORY "${CMAKE_SOURCE_DIR}/Workspace" CACHE PATH "")
	option(ASSET_WORKSPACE_DIRECTORY "Where to find raw game assets.")
endif()

add_subdirectory(external)
add_subdirectory(gemcutter)
add_subdirectory(tools)

option(ENABLE_TESTS "Build unit tests." ${IS_TOP_LEVEL})
if (ENABLE_TESTS)
	add_subdirectory(tests)
endif()
