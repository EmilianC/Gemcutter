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

# Create shortcuts for using the asset manager.
if (DEFINED ASSET_WORKSPACE_DIRECTORY AND EXISTS ${ASSET_WORKSPACE_DIRECTORY})
	get_target_property(ASSET_MANAGER_BINARY_DIR asset_manager BINARY_DIR)

	configure_file(pack_assets_template.cmd "${ASSET_WORKSPACE_DIRECTORY}/pack_assets.cmd" )
	configure_file(open_asset_manager_template.cmd "${ASSET_WORKSPACE_DIRECTORY}/open_asset_manager.cmd" )
endif()
