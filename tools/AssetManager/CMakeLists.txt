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

# Initialize a folder for use with the AssetManager.
function(gem_set_up_workspace workspace_directory)
	if (EXISTS ${workspace_directory})
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

		# Do not replace an existing config, it might have user configured encoders.
		if (NOT EXISTS "${workspace_directory}/config.workspace")
			configure_file(
				"${ASSET_MANAGER_SOURCE_DIR}/workspace_templates/config.workspace"
				"${workspace_directory}/config.workspace"
			)
		endif()
	endif()
endfunction()
