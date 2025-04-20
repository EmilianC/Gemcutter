# Append to any IDE folders already set be calling scripts.
set(PARENT_FOLDER ${CMAKE_FOLDER})

## Catch ##
set(CMAKE_FOLDER "${PARENT_FOLDER}/external/catch")
add_library(catch INTERFACE)
target_include_directories(catch INTERFACE catch/)
target_compile_definitions(catch
	INTERFACE
		CATCH_CONFIG_FAST_COMPILE
		CATCH_CONFIG_DISABLE_EXCEPTIONS
)

## Dirent ##
set(CMAKE_FOLDER "${PARENT_FOLDER}/external/dirent")
add_library(dirent INTERFACE)
target_include_directories(dirent INTERFACE dirent/)

## Freetype ##
update_submodule("${CMAKE_CURRENT_LIST_DIR}/freetype")
set(CMAKE_FOLDER "${PARENT_FOLDER}/external/freetype")
set(FT_DISABLE_ZLIB     ON CACHE BOOL "")
set(FT_DISABLE_BZIP2    ON CACHE BOOL "")
set(FT_DISABLE_PNG      ON CACHE BOOL "")
set(FT_DISABLE_HARFBUZZ ON CACHE BOOL "")
set(FT_DISABLE_BROTLI   ON CACHE BOOL "")
set(SKIP_INSTALL_ALL    ON CACHE BOOL "")
set(SKIP_PACKAGING_ALL  ON CACHE BOOL "")
cmake_policy(PUSH)
set(CMAKE_POLICY_DEFAULT_CMP0069 NEW) # Allow IPO
add_subdirectory(freetype)
cmake_policy(POP)

## GLEW ##
update_submodule("${CMAKE_CURRENT_LIST_DIR}/glew")
set(CMAKE_FOLDER "${PARENT_FOLDER}/external/glew")
set(glew-cmake_BUILD_SHARED OFF CACHE BOOL "")
set(glew-cmake_BUILD_STATIC ON  CACHE BOOL "")
set(ONLY_LIBS               ON  CACHE BOOL "")
cmake_policy(PUSH)
set(CMAKE_POLICY_DEFAULT_CMP0069 NEW) # Allow IPO
add_subdirectory(glew)
cmake_policy(POP)

## Imgui ##
if (ENABLE_IMGUI_SUPPORT OR ENABLE_IMGUI_DEV_SUPPORT)
	update_submodule("${CMAKE_CURRENT_LIST_DIR}/imgui")
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

## Loupe ##
update_submodule("${CMAKE_CURRENT_LIST_DIR}/loupe")
set(CMAKE_FOLDER "${PARENT_FOLDER}/external/loupe")
add_subdirectory(loupe)

## SOIL2 ##
update_submodule("${CMAKE_CURRENT_LIST_DIR}/soil2")
set(CMAKE_FOLDER "${PARENT_FOLDER}/external/soil2")
set(SOIL2_BUILD_TESTS OFF CACHE BOOL "")
add_subdirectory(soil2)

## SoLoud ##
update_submodule("${CMAKE_CURRENT_LIST_DIR}/soloud")
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
cmake_policy(PUSH)
set(CMAKE_POLICY_DEFAULT_CMP0069 NEW) # Allow IPO
add_subdirectory(soloud/contrib)
cmake_policy(POP)
target_include_directories(soloud INTERFACE soloud/include/)
