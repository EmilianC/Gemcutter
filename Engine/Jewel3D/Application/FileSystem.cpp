// Copyright (c) 2017 Emilian Cioca
#include "Jewel3D/Precompiled.h"
#include "FileSystem.h"
#include "Logging.h"
#include "Jewel3D/Utilities/ScopeGuard.h"

#include <Dirent/dirent.h>
#include <direct.h>
#include <stack>

#define SUCCESS 0

// Resolve name conflict with our functions.
#undef GetCurrentDirectory
#undef SetCurrentDirectory

namespace
{
	std::stack<std::string> currentDirectoryStack;
}

namespace Jwl
{
	bool ParseDirectory(DirectoryData& outData)
	{
		return ParseDirectory(outData, GetCurrentDirectory());
	}

	bool ParseDirectory(DirectoryData& outData, std::string_view directory)
	{
		outData.files.clear();
		outData.folders.clear();

		DIR* dir = opendir(directory.data());
		if (dir == nullptr)
		{
			return false;
		}

		dirent* drnt;
		while ((drnt = readdir(dir)) != nullptr)
		{
			switch (drnt->d_type)
			{
			// Regular file.
			case DT_REG:
				outData.files.emplace_back(drnt->d_name);
				break;

			// Directory.
			case DT_DIR:
				if (strcmp(drnt->d_name, ".") != 0 &&
					strcmp(drnt->d_name, "..") != 0)
				{
					outData.folders.emplace_back(drnt->d_name);
				}
				break;
			}
		}

		closedir(dir);

		return true;
	}

	bool DirectoryExists(std::string_view directory)
	{
		if (auto dir = opendir(directory.data()))
		{
			closedir(dir);
			return true;
		}
		else
		{
			return false;
		}
	}

	bool IsPathRelative(std::string_view directory)
	{
		return ExtractDriveLetter(directory).empty();
	}

	bool IsPathAbsolute(std::string_view directory)
	{
		return !IsPathRelative(directory);
	}

	bool FileExists(std::string_view fileName)
	{
		FILE* file = nullptr;
		fopen_s(&file, fileName.data(), "r");

		if (file != nullptr)
		{
			fclose(file);
			return true;
		}
		else
		{
			return false;
		}
	}

	bool RemoveFile(std::string_view file)
	{
		return DeleteFile(file.data()) == TRUE;
	}

	bool MakeDirectory(std::string_view directory)
	{
		if (_mkdir(directory.data()) == ENOENT)
		{
			Error("FileSystem: Could not create directory due to an invalid directory path.");
			return false;
		}
		else
		{
			return true;
		}
	}

	std::string GetCurrentDirectory()
	{
		char result[MAX_PATH] = { '\0' };
		GetCurrentDirectoryA(MAX_PATH, result);

		return result;
	}

	void SetCurrentDirectory(std::string_view directory)
	{
		SetCurrentDirectoryA(directory.data());
	}

	void PushCurrentDirectory(std::string_view newDirectory)
	{
		currentDirectoryStack.push(GetCurrentDirectory());
		SetCurrentDirectory(newDirectory);
	}

	void PopCurrentDirectory()
	{
		SetCurrentDirectory(currentDirectoryStack.top());
		currentDirectoryStack.pop();
	}

	std::string ExtractDriveLetter(std::string_view path)
	{
		if (path.empty())
		{
			Error("FileSystem: Cannot process an empty string.");
			return std::string();
		}

		char result[_MAX_DRIVE] = { '\0' };
		if (_splitpath_s(path.data(), result, _MAX_DRIVE, nullptr, 0, nullptr, 0, nullptr, 0) != SUCCESS)
		{
			Error("FileSystem: Invalid path.");
			return std::string();
		}

		return result;
	}

	std::string ExtractPath(std::string_view path)
	{
		if (path.empty())
		{
			Error("FileSystem: Cannot process an empty string.");
			return std::string();
		}

		char result[_MAX_DIR] = { '\0' };
		if (_splitpath_s(path.data(), nullptr, 0, result, _MAX_DIR, nullptr, 0, nullptr, 0) != SUCCESS)
		{
			Error("FileSystem: Invalid path.");
			return std::string();
		}

		return ExtractDriveLetter(path) + result;
	}

	std::string ExtractFile(std::string_view path)
	{
		return ExtractFilename(path) + ExtractFileExtension(path);
	}

	std::string ExtractFilename(std::string_view path)
	{
		if (path.empty())
		{
			Error("FileSystem: Cannot process an empty string.");
			return std::string();
		}

		char result[_MAX_FNAME] = { '\0' };
		if (_splitpath_s(path.data(), nullptr, 0, nullptr, 0, result, _MAX_FNAME, nullptr, 0) != SUCCESS)
		{
			Error("FileSystem: Invalid path.");
			return std::string();
		}

		return result;
	}

	std::string ExtractFileExtension(std::string_view path)
	{
		if (path.empty())
		{
			Error("FileSystem: Cannot process an empty string.");
			return std::string();
		}

		char result[_MAX_EXT] = { '\0' };
		if (_splitpath_s(path.data(), nullptr, 0, nullptr, 0, nullptr, 0, result, _MAX_EXT) != SUCCESS)
		{
			Error("FileSystem: Invalid path.");
			return std::string();
		}

		return result;
	}

	std::string LoadFileAsString(std::string_view fileName)
	{
		std::string result;

		std::ifstream inStream(fileName.data());
		if (inStream.good())
		{
			result = std::string(std::istreambuf_iterator<char>(inStream), std::istreambuf_iterator<char>());
			inStream.close();
		}

		return result;
	}

	FileReader::~FileReader()
	{
		Close();
	}

	bool FileReader::operator!() const
	{
		return !file.is_open() && buffer.empty();
	}

	bool FileReader::OpenAsBuffer(std::string_view filePath)
	{
		ASSERT(!(*this), "FileReader: Already associated with a file");

		file.open(filePath.data());
		if (!file.good())
		{
			return false;
		}

		// Get length of file.
		file.seekg(0, file.end);
		length = static_cast<unsigned>(file.tellg());
		file.seekg(0, file.beg);

		// Read data as a block.
		char* buff = static_cast<char*>(malloc(sizeof(char) * length));
		defer { free(buff); };

		memset(buff, '\0', length);
		file.read(buff, length);
		buffer = buff;

		file.close();

		return true;
	}

	bool FileReader::OpenAsStream(std::string_view filePath)
	{
		ASSERT(!(*this), "FileReader: Already associated with a file");

		file.open(filePath.data());
		if (!file.good())
		{
			return false;
		}

		// Get length of file.
		file.seekg(0, file.end);
		length = static_cast<unsigned>(file.tellg());
		file.seekg(0, file.beg);

		return true;
	}

	void FileReader::Close()
	{
		buffer.clear();
		currentPos = 0;

		if (file.good())
		{
			file.close();
		}
	}

	std::string FileReader::GetContents()
	{
		if (buffer.empty())
		{
			// Buffer mode
			return std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
		}
		else
		{
			// Stream mode
			if (currentPos >= buffer.size())
			{
				return std::string();
			}

			return buffer.substr(currentPos);
		}
	}

	std::string FileReader::GetLine()
	{
		std::string result;
		if (buffer.empty())
		{
			std::getline(file, result);
		}
		else
		{
			if (currentPos < buffer.size())
			{
				result = buffer.substr(currentPos, buffer.find('\n', currentPos) - currentPos);
				currentPos += result.size() + 1; //+ 1 for '\n'
			}
		}
		
		return result;
	}

	std::string FileReader::GetWord()
	{
		std::string result;
		if (buffer.empty())
		{
			file >> result;
		}
		else
		{
			if (currentPos < buffer.size())
			{
				result = buffer.substr(currentPos, buffer.find_first_of(" \n", currentPos) - currentPos);
				currentPos += result.size();
			}
		}

		return result;
	}

	float FileReader::GetFloat()
	{
		if (buffer.empty())
		{
			float result;
			file >> result;
			return result;
		}
		else
		{
			if (currentPos >= buffer.size())
			{
				return 0.0f;
			}

			return std::stof(&buffer[currentPos], &currentPos);
		}
	}

	int FileReader::GetInt()
	{
		if (buffer.empty())
		{
			int result;
			file >> result;
			return result;
		}
		else
		{
			if (currentPos >= buffer.size())
			{
				return 0;
			}

			return std::stoi(&buffer[currentPos], &currentPos);
		}
	}

	char FileReader::GetChar()
	{
		if (buffer.empty())
		{
			return static_cast<char>(file.get());
		}
		else
		{
			if (currentPos >= buffer.size())
			{
				return 0;
			}

			return buffer[currentPos++];
		}
	}

	bool FileReader::IsEOF() const
	{
		if (buffer.empty())
		{
			return file.eof();
		}
		else 
		{
			return currentPos >= buffer.size();
		}
	}

	int FileReader::GetSize() const
	{
		return length;
	}
}
