list(APPEND gemcutter_files
	"AI/ProbabilityMatrix.cpp"
	"AI/ProbabilityMatrix.h"

	"Application/Application.cpp"
	"Application/Application.h"
	"Application/CmdArgs.cpp"
	"Application/CmdArgs.h"
	"Application/Delegate.cpp"
	"Application/Delegate.h"
	"Application/Delegate.inl"
	"Application/Event.cpp"
	"Application/Event.h"
	"Application/Event.inl"
	"Application/FileSystem.cpp"
	"Application/FileSystem.h"
	"Application/HierarchicalEvent.cpp"
	"Application/HierarchicalEvent.h"
	"Application/Logging.cpp"
	"Application/Logging.h"
	"Application/Reflection.cpp"
	"Application/Reflection.h"
	"Application/Reflection.inl"
	"Application/Timer.cpp"
	"Application/Timer.h"

	"Entity/Entity.cpp"
	"Entity/Entity.h"
	"Entity/Entity.inl"
	"Entity/Hierarchy.cpp"
	"Entity/Hierarchy.h"
	"Entity/Hierarchy.inl"
	"Entity/Name.cpp"
	"Entity/Name.h"
	"Entity/Query.inl"

	"GUI/Button.cpp"
	"GUI/Button.h"
	"GUI/Image.cpp"
	"GUI/Image.h"
	"GUI/Label.cpp"
	"GUI/Label.h"
	"GUI/Rectangle.cpp"
	"GUI/Rectangle.h"
	"GUI/Screen.cpp"
	"GUI/Screen.h"
	"GUI/Widget.cpp"
	"GUI/Widget.h"
	"GUI/Widget.inl"

	"Input/Input.cpp"
	"Input/Input.h"
	"Input/XboxGamePad.cpp"
	"Input/XboxGamePad.h"

	"Math/Math.cpp"
	"Math/Math.h"
	"Math/Matrix.cpp"
	"Math/Matrix.h"
	"Math/Quaternion.cpp"
	"Math/Quaternion.h"
	"Math/Transform.cpp"
	"Math/Transform.h"
	"Math/Vector.cpp"
	"Math/Vector.h"

	"Network/Network.cpp"
	"Network/Network.h"

	"Rendering/Camera.cpp"
	"Rendering/Camera.h"
	"Rendering/Light.cpp"
	"Rendering/Light.h"
	"Rendering/Mesh.cpp"
	"Rendering/Mesh.h"
	"Rendering/ParticleEmitter.cpp"
	"Rendering/ParticleEmitter.h"
	"Rendering/Primitives.cpp"
	"Rendering/Primitives.h"
	"Rendering/Renderable.cpp"
	"Rendering/Renderable.h"
	"Rendering/Rendering.cpp"
	"Rendering/Rendering.h"
	"Rendering/RenderPass.cpp"
	"Rendering/RenderPass.h"
	"Rendering/RenderTarget.cpp"
	"Rendering/RenderTarget.h"
	"Rendering/Sprite.cpp"
	"Rendering/Sprite.h"
	"Rendering/Text.cpp"
	"Rendering/Text.h"
	"Rendering/Viewport.cpp"
	"Rendering/Viewport.h"

	"Resource/ConfigTable.cpp"
	"Resource/ConfigTable.h"
	"Resource/Encoder.h"
	"Resource/Font.cpp"
	"Resource/Font.h"
	"Resource/Material.cpp"
	"Resource/Material.h"
	"Resource/Model.cpp"
	"Resource/Model.h"
	"Resource/ParticleBuffer.cpp"
	"Resource/ParticleBuffer.h"
	"Resource/ParticleFunctor.cpp"
	"Resource/ParticleFunctor.h"
	"Resource/Resource.cpp"
	"Resource/Resource.h"
	"Resource/Shader.cpp"
	"Resource/Shader.h"
	"Resource/Shareable.h"
	"Resource/Sound.cpp"
	"Resource/Sound.h"
	"Resource/Texture.cpp"
	"Resource/Texture.h"
	"Resource/UniformBuffer.cpp"
	"Resource/UniformBuffer.h"
	"Resource/UniformBuffer.inl"
	"Resource/VertexArray.cpp"
	"Resource/VertexArray.h"
	"Resource/VertexArray.inl"

	"Sound/SoundListener.cpp"
	"Sound/SoundListener.h"
	"Sound/SoundSource.cpp"
	"Sound/SoundSource.h"
	"Sound/SoundSystem.cpp"
	"Sound/SoundSystem.h"

	"Utilities/EnumFlags.h"
	"Utilities/Identifier.h"
	"Utilities/Meta.h"
	"Utilities/Random.cpp"
	"Utilities/Random.h"
	"Utilities/ScopeGuard.h"
	"Utilities/StdExt.h"
	"Utilities/String.cpp"
	"Utilities/String.h"

	"Gemcutter.natvis"
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${gemcutter_files})

add_library(gemcutter STATIC ${gemcutter_files})
sf_target_compile_disable_exceptions(gemcutter OPTIONAL)
sf_target_compile_disable_rtti(gemcutter OPTIONAL)
sf_target_compile_fast_fp(gemcutter OPTIONAL)
sf_target_compile_warnings_as_errors(gemcutter OPTIONAL)
sf_target_compile_warnings(gemcutter)

target_compile_features(gemcutter PUBLIC cxx_std_23)
target_include_directories(gemcutter PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../)
target_include_directories(gemcutter SYSTEM PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../external/dirent/)
target_link_libraries(gemcutter
	PRIVATE
		dirent
	PUBLIC
		OpenGL::GL
		soil2
		$<$<BOOL:${CMAKE_OS_IS_WINDOWS}>:ws2_32>
		Xinput
		libglew_static
		soloud
		loupe
)
target_link_options(gemcutter
	PUBLIC
		$<$<CXX_COMPILER_ID:MSVC>:/WHOLEARCHIVE:gemcutter.lib>
)

if (ENABLE_IMGUI_SUPPORT)
	target_link_libraries(gemcutter PUBLIC imgui)
elseif (ENABLE_IMGUI_DEV_SUPPORT)
	target_link_libraries(gemcutter PUBLIC $<$<CONFIG:Debug,RelWithDebInfo>:imgui>)
endif()

target_compile_definitions(gemcutter
	PUBLIC
		$<$<CONFIG:Debug>:GEM_DEBUG>
		$<$<CONFIG:Debug,RelWithDebInfo>:GEM_DEV>
		$<$<CONFIG:Release>:GEM_PRODUCTION>
		$<$<BOOL:${CMAKE_OS_IS_LINUX}>:GEM_LINUX>
		$<$<BOOL:${CMAKE_OS_IS_WINDOWS}>:GEM_WINDOWS>
		$<$<BOOL:${CMAKE_OS_IS_WINDOWS}>:WIN32_LEAN_AND_MEAN>
		$<$<BOOL:${CMAKE_OS_IS_WINDOWS}>:_CRT_SECURE_NO_WARNINGS>
		$<$<BOOL:${CMAKE_OS_IS_WINDOWS}>:NOMINMAX>
)

option(ENABLE_PCH "Use a precompiled header file." TRUE)
if (ENABLE_PCH)
	target_precompile_headers(gemcutter
		PRIVATE
			<algorithm>
			<array>
			<bit>
			<concepts>
			<cstddef>
			<cstdint>
			<cstring>
			<fstream>
			<functional>
			<map>
			<memory>
			<numeric>
			<optional>
			<queue>
			<span>
			<string_view>
			<string>
			<type_traits>
			<unordered_map>
			<vector>
			Application/FileSystem.h
			Application/Logging.h
			Application/Reflection.h
			Entity/Entity.h
			GUI/Widget.h
			Math/Matrix.h
			Math/Quaternion.h
			Math/Transform.h
			Math/Vector.h
			Resource/Shareable.h
			Utilities/Meta.h
			Utilities/Random.h
	)
endif()
