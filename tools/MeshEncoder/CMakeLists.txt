list(APPEND mesh_encoder_files
	"main.cpp"
	"MeshEncoder.h"
	"MeshEncoder.cpp"
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${mesh_encoder_files})

add_executable(mesh_encoder ${mesh_encoder_files})
sf_target_compile_warnings(mesh_encoder)
sf_target_compile_warnings_as_errors(mesh_encoder OPTIONAL)

target_link_libraries(mesh_encoder PRIVATE gemcutter)
