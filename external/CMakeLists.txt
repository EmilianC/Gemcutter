# Append to any IDE folders already set be calling scripts.
set(PARENT_FOLDER ${CMAKE_FOLDER})

# Catch #
set(CMAKE_FOLDER "${PARENT_FOLDER}/external/catch")
add_library(catch INTERFACE)
target_include_directories(catch INTERFACE catch/)
target_compile_definitions(catch
	INTERFACE
		CATCH_CONFIG_FAST_COMPILE
		CATCH_CONFIG_DISABLE_EXCEPTIONS
)

# Dirent #
set(CMAKE_FOLDER "${PARENT_FOLDER}/external/dirent")
add_library(dirent INTERFACE)
target_include_directories(dirent INTERFACE dirent/)

# FreeType #
set(CMAKE_FOLDER "${PARENT_FOLDER}/external/freetype")
add_library(freetype INTERFACE)
target_include_directories(freetype INTERFACE freetype/)
target_link_directories(freetype INTERFACE freetype/)
target_link_libraries(freetype INTERFACE freetype${CMAKE_STATIC_LIBRARY_SUFFIX})

# GLEW #
set(CMAKE_FOLDER "${PARENT_FOLDER}/external/glew")
add_library(glew INTERFACE)
target_include_directories(glew INTERFACE glew/)
target_link_directories(glew INTERFACE glew/)
target_link_libraries(glew INTERFACE glew32s${CMAKE_STATIC_LIBRARY_SUFFIX})
target_compile_definitions(glew INTERFACE GLEW_STATIC)

# OpenAL #
set(CMAKE_FOLDER "${PARENT_FOLDER}/external/openal-soft")
set(ALSOFT_EXAMPLES                    OFF CACHE BOOL "")
set(ALSOFT_INSTALL                     OFF CACHE BOOL "")
set(ALSOFT_INSTALL_AMBDEC_PRESETS      OFF CACHE BOOL "")
set(ALSOFT_INSTALL_CONFIG              OFF CACHE BOOL "")
set(ALSOFT_INSTALL_EXAMPLES            OFF CACHE BOOL "")
set(ALSOFT_INSTALL_HRTF_DATA           OFF CACHE BOOL "")
set(ALSOFT_INSTALL_UTILS               OFF CACHE BOOL "")
set(ALSOFT_NO_CONFIG_UTIL              ON  CACHE BOOL "")
set(ALSOFT_TESTS                       OFF CACHE BOOL "")
set(ALSOFT_UTILS                       OFF CACHE BOOL "")
set(CMAKE_POLICY_DEFAULT_CMP0069 NEW) # Allow LTO
add_subdirectory(openal-soft)

# SOIL ext #
set(CMAKE_FOLDER "${PARENT_FOLDER}/external/soil")
add_library(soil INTERFACE)
target_include_directories(soil INTERFACE soil/)
target_link_directories(soil INTERFACE soil/)
target_link_libraries(soil
	INTERFACE
		optimized SOIL_ext
		debug SOIL_ext-d
)
