# Asset Manager
This tool allows you to easily prepare your assets for run-time use by the framework.

# Folder Structure
Remember that upon building, the Asset directory is re-created from scratch. This directory should be completely driven
by the Asset Manager. For this reason, persistant game data should be kept outside the Asset directory. This also means
that the Asset directory is safe to ignore from source control.

The recommended folder structure for a project is:
- Workspace
	- Raw assets
- Assets
	- Packed assets
- Game
	- Persistant game data
