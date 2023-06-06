// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "gemcutter/Application/FileSystem.h"
#include "gemcutter/Utilities/StdExt.h"
#include "gemcutter/Utilities/String.h"

#include <string_view>
#include <string>
#include <type_traits>
#include <unordered_map>

namespace gem
{
	// Base class for all resources loadable from a path on disk.
	class ResourceBase
	{
	public:
		const std::string& GetPath() const { return path; }

	protected:
		ResourceBase() = default;

		std::string path;
	};

	// Provides an interface for cached loading of the specified asset.
	template<class Asset>
	class Resource : public ResourceBase
	{
	protected:
		Resource() = default;

	public:
		Resource(const Resource&) = delete;
		Resource& operator=(const Resource&) = delete;

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
	};

	// Helper function to load an asset.
	template<class Asset>
	std::shared_ptr<Asset> Load(std::string filePath)
	{
		static_assert(std::is_base_of_v<Resource<Asset>, Asset>);
		return Resource<Asset>::Load(std::move(filePath));
	}

	// Helper function to unload all managed instances of an asset.
	template<class Asset>
	void UnloadAll()
	{
		static_assert(std::is_base_of_v<Resource<Asset>, Asset>);
		Resource<Asset>::UnloadAll();
	}
}
