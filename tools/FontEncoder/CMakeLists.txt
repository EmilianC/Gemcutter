list(APPEND font_encoder_files
	"main.cpp"
	"FontEncoder.h"
	"FontEncoder.cpp"
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${font_encoder_files})

add_executable(font_encoder ${font_encoder_files})
sf_target_compile_warnings(font_encoder)
sf_target_compile_warnings_as_errors(font_encoder OPTIONAL)

target_link_libraries(font_encoder
	PRIVATE
		gemcutter
		freetype
)
