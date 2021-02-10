list(APPEND material_encoder_files
	"main.cpp"
	"MaterialEncoder.h"
	"MaterialEncoder.cpp"
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${material_encoder_files})

add_executable(material_encoder ${material_encoder_files})
sf_target_compile_warnings(material_encoder)
sf_target_compile_warnings_as_errors(material_encoder OPTIONAL)

target_link_libraries(material_encoder PRIVATE gemcutter)
