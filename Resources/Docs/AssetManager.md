# AssetManager
![AssetManager](AssetManager.png)

This tool allows you to easily prepare your assets for run-time use by the framework.

# Workspace
The workspace folder holds all the raw assets that will eventually be used by the game.

# Encoders
An Encoder is a DLL that implements the interface provided by ```Jewel3D\Resource\Encoder.h```.
In the AssetManager, each Encoder is associated with a specific file extension. Assets of that type will be processed using the functions in the Encoder.
Custom Encoders can be created for game-specific assets (levels, items, dialog-trees etc.), allowing you to easily pack game data.

# Metadata
Every asset with an associated encoder has metaData created for it. Metadata is saved adjacent to the asset (```asset.extension.meta```) as plain text.
The content of the metaData file depends on the encoder. These are the properties used when packing the asset into a binary format.
For instance, .ttf.meta files for fonts specify the output resolution of the generated textures.

# Building Assets
The AssetManager provides two functions, Updating and Packing. Updating your workspace will ensure that all
assets have up-to-date metaData associated with them. If an asset doesn't have metaData, Updating the workspace
will use the Encoder associated with that asset to generate a default metaData file. If an asset already has
metaData, the Encoder will be used to validate the integrity of the metadata. For now, metaData is edited manually with a text editor.

Packing the workspace will prepare all assets for use by the game. An ```Assets``` folder will be created adjacent to the Workspace.
All assets with an associated Encoder will be converted using the DLL and saved to the Assets folder. Any remaining assets are
copied 1:1 to the folder.

# Folder Structure
When assets are packing, the asset directory is re-created from scratch. This directory should be completely driven
by the AssetManager. For this reason, persistent game data should be kept outside the Asset directory during development. This also means
that the Asset directory should be ignored from source control.
