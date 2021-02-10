list(APPEND texture_encoder_files
	"main.cpp"
	"TextureEncoder.h"
	"TextureEncoder.cpp"
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${texture_encoder_files})

add_executable(texture_encoder ${texture_encoder_files})
sf_target_compile_warnings(texture_encoder)
sf_target_compile_warnings_as_errors(texture_encoder OPTIONAL)

target_link_libraries(texture_encoder
	PRIVATE
		gemcutter
		soil
)
