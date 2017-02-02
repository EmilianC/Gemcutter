// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "Shareable.h"
#include "Jewel3D/Application/FileSystem.h"
#include "Jewel3D/Application/Logging.h"
#include "Jewel3D/Reflection/Reflection.h"

#include <string>
#include <unordered_map>

namespace Reflection
{
	struct Resource : MetaTagBase {};
}

namespace Jwl
{
	static std::string RootAssetDirectory = "./Assets/";

	//- Base resource class. Provides an interface for cached loading.
	template<class Asset>
	class Resource : public Shareable<Asset>
	{
	protected:
		Resource() = default;
		Resource(const Resource&) = delete;
		Resource& operator=(const Resource&) = delete;

	public:
		//- Loads an asset from the specified file, if it wasn't loaded already.
		//- Calls the derived class's Load() with the file path and any other arguments.
		template<typename... Args>
		static Ptr Load(std::string filePath, Args&&... params)
		{
			if (IsPathRelative(filePath))
			{
				filePath = RootAssetDirectory + filePath;
			}
			else
			{
				Warning("Asset loaded with an absolute path. To ensure proper asset caching, it is recommended to use relative paths only.");
			}

			auto itr = resourceCache.find(filePath);
			if (itr == resourceCache.end())
			{
				// Create the new asset.
				Ptr resourcePtr = MakeNew();

				if (!resourcePtr->Load(filePath, std::forward<Args>(params)...))
				{
					// Error loading file.
					return nullptr;
				}

				// Add new asset to cache.
				resourceCache.insert(std::make_pair(filePath, resourcePtr));

				return resourcePtr;
			}
			else
			{
				return itr->second;
			}
		}

		//- Searches for a loaded asset previously loaded from the specified file path.
		static Ptr Find(const std::string& filePath)
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

		//- Clears the asset cache. An asset will need to load from file again after this call.
		static void UnloadAll()
		{
			resourceCache.clear();
		}

	private:
		static std::unordered_map<std::string, Ptr> resourceCache;
	};

	template<class Asset>
	std::unordered_map<std::string, typename Resource<Asset>::Ptr> Resource<Asset>::resourceCache;

	//- Helper function to load an asset.
	template<class Asset, typename... Args>
	typename Resource<Asset>::Ptr Load(const std::string& filePath, Args&&... params)
	{
		return Resource<Asset>::Load(filePath, std::forward<Args>(params)...);
	}

	//- Helper function to unload all managed instances of an asset.
	template<class Asset>
	void UnloadAll()
	{
		Resource<Asset>::UnloadAll();
	}
}
