list(APPEND unit_test_files
	"Delegate.cpp"
	"EntityComponentSystem.cpp"
	"EnumFlags.cpp"
	"FileSystem.cpp"
	"Hierarchy.cpp"
	"main.cpp"
	"Math.cpp"
	"Meta.cpp"
	"String.cpp"
	"WeakPtr.cpp"
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${unit_test_files})

add_executable(unit_tests ${unit_test_files})
sf_target_compile_warnings(unit_tests)

target_link_libraries(unit_tests
	PRIVATE
		gemcutter
		catch
)
