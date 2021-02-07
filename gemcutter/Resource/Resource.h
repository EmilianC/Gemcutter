// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "gemcutter/Application/FileSystem.h"
#include "gemcutter/Application/Logging.h"
#include "gemcutter/Utilities/Container.h"

#include <string>
#include <unordered_map>

namespace Jwl
{
	extern std::string RootAssetDirectory;

	// Base resource class. Provides an interface for cached loading.
	template<class Asset>
	class Resource
	{
	protected:
		Resource() = default;

	public:
		Resource(const Resource&) = delete;
		Resource& operator=(const Resource&) = delete;

		// Loads an asset from the specified file, if it wasn't loaded already.
		// Calls the derived class's Load() with the file path and any other arguments.
		template<typename... Args>
		static std::shared_ptr<Asset> Load(std::string filePath, Args&&... params)
		{
			if (IsPathRelative(filePath))
			{
				filePath = RootAssetDirectory + filePath;
			}
			else
			{
				Warning("Asset loaded with an absolute path. To ensure proper asset caching, it is recommended to use only relative paths.");
			}

			// Search for the cached asset.
			if (auto ptr = Find(filePath))
			{
				return ptr;
			}

			// Create the new asset.
			auto resourcePtr = std::make_shared<Asset>();

			if (!resourcePtr->Load(filePath, std::forward<Args>(params)...))
			{
				// Error loading file.
				return nullptr;
			}

			// Add new asset to cache.
			resourceCache.insert(std::pair(std::move(filePath), resourcePtr));

			return resourcePtr;
		}

		// Searches for a loaded asset previously loaded from the specified file path.
		static std::shared_ptr<Asset> Find(std::string_view filePath)
		{
			auto itr = resourceCache.find(filePath);
			if (itr == resourceCache.end())
			{
				return nullptr;
			}
			else
			{
				return itr->second;
			}
		}

		// Clears the asset cache. An asset will need to load from file again after this call.
		static void UnloadAll()
		{
			resourceCache.clear();
		}

	private:
		static std::unordered_map<std::string, std::shared_ptr<Asset>, string_hash, std::equal_to<>> resourceCache;
	};

	template<class Asset>
	std::unordered_map<std::string, std::shared_ptr<Asset>, string_hash, std::equal_to<>> Resource<Asset>::resourceCache;

	// Helper function to load an asset.
	template<class Asset, typename... Args>
	std::shared_ptr<Asset> Load(const std::string& filePath, Args&&... params)
	{
		return Resource<Asset>::Load(filePath, std::forward<Args>(params)...);
	}

	// Helper function to unload all managed instances of an asset.
	template<class Asset>
	void UnloadAll()
	{
		Resource<Asset>::UnloadAll();
	}
}
