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

file(MAKE_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/")
execute_process( # Copy dlls alongside project executable.
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_LIST_DIR}/freetype/freetype6.dll" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/"
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_LIST_DIR}/freetype/freetype6.dll" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/"
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_LIST_DIR}/freetype/zlib1.dll" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/"
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_LIST_DIR}/freetype/zlib1.dll" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/"
)

# GLEW #
set(CMAKE_FOLDER "${PARENT_FOLDER}/external/glew")
add_library(glew INTERFACE)
target_include_directories(glew INTERFACE glew/)
target_link_directories(glew INTERFACE glew/)
target_link_libraries(glew INTERFACE glew32s${CMAKE_STATIC_LIBRARY_SUFFIX})
target_compile_definitions(glew INTERFACE GLEW_STATIC)

# Imgui #
if (ENABLE_IMGUI_SUPPORT)
	set(CMAKE_FOLDER "${PARENT_FOLDER}/external/imgui")
	add_library(imgui STATIC
		"imgui/backends/imgui_impl_win32.cpp"
		"imgui/backends/imgui_impl_opengl3.cpp"
		"imgui/misc/cpp/imgui_stdlib.cpp"
		"imgui/imgui.cpp"
		"imgui/imgui_demo.cpp"
		"imgui/imgui_draw.cpp"
		"imgui/imgui_tables.cpp"
		"imgui/imgui_widgets.cpp"
	)
	target_include_directories(imgui PUBLIC imgui/)
	target_compile_definitions(imgui PUBLIC IMGUI_ENABLED)
	sf_target_compile_disable_exceptions(imgui)
endif()

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

# SoLoud #
set(CMAKE_FOLDER "${PARENT_FOLDER}/external/soloud")
set(SOLOUD_BACKEND_ALSA          OFF CACHE BOOL "")
set(SOLOUD_BACKEND_COREAUDIO     OFF CACHE BOOL "")
set(SOLOUD_BACKEND_NULL          OFF CACHE BOOL "")
set(SOLOUD_BACKEND_OPENSLES      OFF CACHE BOOL "")
set(SOLOUD_BACKEND_SDL2          OFF CACHE BOOL "")
set(SOLOUD_BACKEND_WASAPI        OFF CACHE BOOL "")
set(SOLOUD_BACKEND_WINMM         OFF CACHE BOOL "")
set(SOLOUD_BACKEND_XAUDIO2       ON  CACHE BOOL "")
set(SOLOUD_BUILD_DEMOS           OFF CACHE BOOL "")
set(SOLOUD_C_API                 OFF CACHE BOOL "")
set(SOLOUD_DYNAMIC               OFF CACHE BOOL "")
set(SOLOUD_GENERATE_GLUE         OFF CACHE BOOL "")
set(SOLOUD_INSTALL               OFF CACHE BOOL "")
set(SOLOUD_STATIC                ON  CACHE BOOL "")
set(CMAKE_POLICY_DEFAULT_CMP0069 NEW) # Allow LTO
add_subdirectory(soloud/contrib)
target_include_directories(soloud INTERFACE soloud/include/)
