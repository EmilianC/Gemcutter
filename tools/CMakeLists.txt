set(CMAKE_FOLDER "${CMAKE_FOLDER}/tools")
add_subdirectory(FontEncoder)
add_subdirectory(MaterialEncoder)
add_subdirectory(MeshEncoder)
add_subdirectory(TextureEncoder)

add_subdirectory(AssetManager)

if (IS_TOP_LEVEL)
	add_subdirectory(EncoderTemplate)
endif()
