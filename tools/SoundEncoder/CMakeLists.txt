list(APPEND sound_encoder_files
	"main.cpp"
	"SoundEncoder.h"
	"SoundEncoder.cpp"
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${sound_encoder_files})

add_executable(sound_encoder ${sound_encoder_files})
sf_target_compile_warnings(sound_encoder)
sf_target_compile_warnings_as_errors(sound_encoder OPTIONAL)

target_link_libraries(sound_encoder
	PRIVATE
		gemcutter
)
