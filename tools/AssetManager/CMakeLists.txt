if (NOT MSVC OR NOT WIN32)
	return()
endif()

enable_language(CSharp)

include(CSharpUtilities)

list(APPEND asset_manager_files
	"App.config"
	"EncoderEntry.cs"
	"FileCache.cs"
	"Manager.cs"
	"Manager.Designer.cs"
	"Manager.resx"
	"Program.cs"
	"RichTextBoxStreamWriter.cs"
	"Settings.cs"
	"Settings.Designer.cs"
	"Settings.resx"
	"WorkspaceConfig.cs"

	"Properties/AssemblyInfo.cs"
    "Properties/Resources.Designer.cs"
    "Properties/Resources.resx"
    "Properties/Settings.Designer.cs"
    "Properties/Settings.settings"

    "Resources/Building.ico"
    "Resources/Collapse.png"
    "Resources/Correct.png"
    "Resources/Delete.png"
    "Resources/Error.png"
    "Resources/Expand.png"
    "Resources/Folder.png"
    "Resources/JewelIcon.ico"
    "Resources/Search.png"
    "Resources/Settings.ico"
    "Resources/Settings.png"
)

list(APPEND dotnet_references
	"Microsoft.CSharp"
	"System"
    "System.Core"
    "System.Data.DataSetExtensions"
    "System.Data"
    "System.Deployment"
    "System.DirectoryServices"
    "System.Drawing"
    "System.Net.Http"
    "System.Windows.Forms"
    "System.Xml.Linq"
    "System.Xml"
)

add_executable(asset_manager ${asset_manager_files})
add_dependencies(asset_manager
	font_encoder
	material_encoder
	mesh_encoder
	texture_encoder
)
set(CMAKE_CSharp_FLAGS "/langversion:latest")
set_target_properties(asset_manager PROPERTIES
	VS_DOTNET_TARGET_FRAMEWORK_VERSION
		"v4.8"
	VS_DOTNET_REFERENCES
		"${dotnet_references}"
	VS_GLOBAL_ROOTNAMESPACE
		"AssetManager"
	WIN32_EXECUTABLE
		TRUE
)

csharp_set_windows_forms_properties(
	"Manager.cs"
	"Manager.Designer.cs"
	"Program.cs"
	"Settings.cs"
	"Settings.Designer.cs"
	"WorkspaceConfig.cs"
    "Properties/AssemblyInfo.cs"
    "Properties/Resources.Designer.cs"
    "Properties/Resources.resx"
    "Properties/Settings.Designer.cs"
    "Properties/Settings.settings"
    "Properties/Settings.resx"
)

# Initialize a folder with scripts for using with the AssetManager.
# You can optionally create a named target for packing the assets. Usage:
# gem_set_up_workspace(
#   raw_asset_directory       # Where the raw assets are located
#   [PACK_TARGET target_name] # The name of the new target
#   [IDE_FOLDER folder])      # A specific IDE folder for the new target, or
#   [IDE_ROOT]                # Place the new target at the root of the IDE
function(gem_set_up_workspace workspace_directory)
	cmake_parse_arguments(THIS "IDE_ROOT" "PACK_TARGET;IDE_FOLDER" "" ${ARGN})

	file(MAKE_DIRECTORY ${workspace_directory})

	get_target_property(ASSET_MANAGER_SOURCE_DIR asset_manager SOURCE_DIR)
	get_target_property(ASSET_MANAGER_BINARY_DIR asset_manager BINARY_DIR)

	configure_file(
		"${ASSET_MANAGER_SOURCE_DIR}/workspace_templates/pack_assets.cmd"
		"${workspace_directory}/pack_assets.cmd"
	)
	configure_file(
		"${ASSET_MANAGER_SOURCE_DIR}/workspace_templates/open_asset_manager.cmd"
		"${workspace_directory}/open_asset_manager.cmd"
	)
	configure_file(
		"${ASSET_MANAGER_SOURCE_DIR}/workspace_templates/update_metadata.cmd"
		"${workspace_directory}/update_metadata.cmd"
	)

	# Do not replace an existing config, it might have user configured encoders.
	if (NOT EXISTS "${workspace_directory}/config.workspace")
		configure_file(
			"${ASSET_MANAGER_SOURCE_DIR}/workspace_templates/config.workspace"
			"${workspace_directory}/config.workspace"
		)
	endif()

	if (THIS_PACK_TARGET)
		if (THIS_IDE_ROOT)
			unset(CMAKE_FOLDER)
		elseif (THIS_IDE_FOLDER)
			set(CMAKE_FOLDER "${THIS_IDE_FOLDER}")
		endif()

		add_custom_target(${THIS_PACK_TARGET}
			COMMAND "${workspace_directory}/pack_assets.cmd" $<CONFIG>
			COMMENT "Packing out of date assets in \"${workspace_directory}\"..."
			WORKING_DIRECTORY "${workspace_directory}"
			VERBATIM
		)
		add_dependencies(${THIS_PACK_TARGET} asset_manager)
	endif()

endfunction()
