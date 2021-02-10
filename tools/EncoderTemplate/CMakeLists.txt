list(APPEND encoder_template_files
	"main.cpp"
	"Encoder.h"
	"Encoder.cpp"
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${encoder_template_files})

add_executable(encoder_template ${encoder_template_files})
sf_target_compile_warnings(encoder_template)
sf_target_compile_warnings_as_errors(encoder_template OPTIONAL)

target_link_libraries(encoder_template PRIVATE gemcutter)
