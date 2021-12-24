// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "gemcutter/Application/FileSystem.h"
#include "gemcutter/Application/Logging.h"
#include "gemcutter/Utilities/Container.h"
#include "gemcutter/Utilities/String.h"

#include <string_view>
#include <string>
#include <unordered_map>

namespace gem
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

		const std::string& GetPath() const { return path; }

		// Loads an asset from the specified file, or retrieves it from the cache.
		// Calls the derived class's Load() with the normalized file path.
		static std::shared_ptr<Asset> Load(std::string filePath)
		{
			NormalizeFilePath(filePath);

			// Search for the cached asset.
			if (std::shared_ptr<Asset> ptr = FindNormalized(filePath))
			{
				return ptr;
			}

			// Create the new asset.
			auto resourcePtr = std::make_shared<Asset>();
			resourcePtr->path = filePath;
			if (!resourcePtr->Load(filePath))
			{
				return nullptr;
			}

			// Add new asset to cache.
			resourceCache.insert(std::make_pair(std::move(filePath), resourcePtr));

			return resourcePtr;
		}

		// Searches for a loaded asset previously loaded from the specified file path.
		static std::shared_ptr<Asset> Find(std::string filePath)
		{
			NormalizeFilePath(filePath);
			return FindNormalized(filePath);
		}

		// Clears the asset cache. An asset will need to load from file again after this call.
		static void UnloadAll()
		{
			resourceCache.clear();
		}

	private:
		// Converts the path to a standard format for cache lookups.
		static void NormalizeFilePath(std::string& filePath)
		{
			ToLowercase(filePath);

			if (!HasExtension(filePath))
			{
				filePath += Asset::Extension;
			}

			if (IsPathRelative(filePath)) [[likely]]
			{
				filePath.insert(0, RootAssetDirectory);
			}
			else
			{
				Warning("Resource ( %s ) referenced with an absolute path. To ensure proper caching, it is recommended to use only relative paths.", filePath.c_str());
			}
		}

		static std::shared_ptr<Asset> FindNormalized(std::string_view filePath)
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

		static inline std::unordered_map<std::string, std::shared_ptr<Asset>, string_hash, std::equal_to<>> resourceCache;

		std::string path;
	};

	// Helper function to load an asset.
	template<class Asset>
	std::shared_ptr<Asset> Load(std::string filePath)
	{
		return Resource<Asset>::Load(std::move(filePath));
	}

	// Helper function to unload all managed instances of an asset.
	template<class Asset>
	void UnloadAll()
	{
		Resource<Asset>::UnloadAll();
	}
}
