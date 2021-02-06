list(APPEND unit_test_files
	"EntityComponentSystem.cpp"
	"EnumFlags.cpp"
	"FileSystem.cpp"
	"Hierarchy.cpp"
	"main.cpp"
	"Math.cpp"
	"String.cpp"
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${unit_test_files})

add_executable(unit_tests WIN32 ${unit_test_files})
sf_target_compile_warnings(unit_tests)

# Disable warning: local variable is initialized but not referenced.
target_compile_options(unit_tests PRIVATE /wd4189)

target_link_libraries(unit_tests
	PRIVATE
		gemcutter
		catch
)
