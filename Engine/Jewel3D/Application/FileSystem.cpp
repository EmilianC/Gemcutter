// Copyright (c) 2017 Emilian Cioca
#include "Jewel3D/Precompiled.h"
#include "FileSystem.h"
#include "Logging.h"

#include <Dirent/dirent.h>
#include <direct.h>
#include <stack>
#include <stdlib.h>

#define SUCCESS 0

// Resolve name conflict with our functions.
#undef GetCurrentDirectory
#undef SetCurrentDirectory

namespace
{
	static std::stack<std::string> currentDirectoryStack;
}

namespace Jwl
{
	bool ParseDirectory(DirectoryData& outData)
	{
		return ParseDirectory(outData, GetCurrentDirectory());
	}

	bool ParseDirectory(DirectoryData& outData, const std::string& directory)
	{
		outData.files.clear();
		outData.folders.clear();

		DIR* dir = nullptr;
		dirent* drnt = nullptr;

		dir = opendir(directory.c_str());
		if (dir == nullptr)
		{
			return false;
		}

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

	bool DirectoryExists(const std::string& directory)
	{
		if (auto dir = opendir(directory.c_str()))
		{
			closedir(dir);
			return true;
		}
		else
		{
			return false;
		}
	}

	bool IsPathRelative(const std::string& directory)
	{
		return ExtractDriveLetter(directory).empty();
	}

	bool IsPathAbsolute(const std::string& directory)
	{
		return !IsPathRelative(directory);
	}

	bool FileExists(const std::string& fileName)
	{
		FILE* file = nullptr;
		fopen_s(&file, fileName.c_str(), "r");

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

	bool MakeDirectory(const std::string& directory)
	{
		if (_mkdir(directory.c_str()) == ENOENT)
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

		return std::string(result);
	}

	void SetCurrentDirectory(const std::string& directory)
	{
		SetCurrentDirectoryA(directory.c_str());
	}

	void PushCurrentDirectory(const std::string& newDirectory)
	{
		currentDirectoryStack.push(GetCurrentDirectory());
		SetCurrentDirectory(newDirectory);
	}

	void PopCurrentDirectory()
	{
		SetCurrentDirectory(currentDirectoryStack.top());
		currentDirectoryStack.pop();
	}

	std::string ExtractDriveLetter(const std::string& path)
	{
		if (path.empty())
		{
			Error("FileSystem: Cannot process an empty string.");
			return std::string();
		}

		char result[_MAX_DRIVE] = { '\0' };
		if (_splitpath_s(path.c_str(), result, _MAX_DRIVE, nullptr, 0, nullptr, 0, nullptr, 0) != SUCCESS)
		{
			Error("FileSystem: Invalid path.");
			return std::string();
		}

		return result;
	}

	std::string ExtractPath(const std::string& path)
	{
		if (path.empty())
		{
			Error("FileSystem: Cannot process an empty string.");
			return std::string();
		}

		char result[_MAX_DIR] = { '\0' };
		if (_splitpath_s(path.c_str(), nullptr, 0, result, _MAX_DIR, nullptr, 0, nullptr, 0) != SUCCESS)
		{
			Error("FileSystem: Invalid path.");
			return std::string();
		}

		return ExtractDriveLetter(path) + result;
	}

	std::string ExtractFile(const std::string& path)
	{
		return ExtractFilename(path) + ExtractFileExtension(path);
	}

	std::string ExtractFilename(const std::string& path)
	{
		if (path.empty())
		{
			Error("FileSystem: Cannot process an empty string.");
			return std::string();
		}

		char result[_MAX_FNAME] = { '\0' };
		if (_splitpath_s(path.c_str(), nullptr, 0, nullptr, 0, result, _MAX_FNAME, nullptr, 0) != SUCCESS)
		{
			Error("FileSystem: Invalid path.");
			return std::string();
		}

		return result;
	}

	std::string ExtractFileExtension(const std::string& path)
	{
		if (path.empty())
		{
			Error("FileSystem: Cannot process an empty string.");
			return std::string();
		}

		char result[_MAX_EXT] = { '\0' };
		if (_splitpath_s(path.c_str(), nullptr, 0, nullptr, 0, nullptr, 0, result, _MAX_EXT) != SUCCESS)
		{
			Error("FileSystem: Invalid path.");
			return std::string();
		}

		return result;
	}

	std::string LoadFileAsString(const std::string& fileName)
	{
		std::ifstream inStream(fileName);
		if (!inStream.good())
		{
			return std::string();
		}

		std::string data(std::istreambuf_iterator<char>(inStream), (std::istreambuf_iterator<char>()));
		inStream.close();

		return data;
	}

	FileReader::~FileReader()
	{
		Close();
	}

	bool FileReader::operator!() const
	{
		return !file.is_open() && buffer.empty();
	}

	bool FileReader::OpenAsBuffer(const std::string& filePath)
	{
		ASSERT(!(*this), "FileReader: Already associated with a file");

		file.open(filePath);
		if (!file.good())
		{
			return false;
		}

		// Get length of file.
		file.seekg(0, file.end);
		length = (unsigned)file.tellg();
		file.seekg(0, file.beg);

		// Read data as a block.
		char* buff = new char[length];
		memset(buff, '\0', length);
		file.read(buff, length);
		buffer = buff;

		delete buff;
		file.close();

		return true;
	}

	bool FileReader::OpenAsStream(const std::string& filePath)
	{
		ASSERT(!(*this), "FileReader: Already associated with a file");

		file.open(filePath);
		if (!file.good())
		{
			return false;
		}

		// get length of file
		file.seekg(0, file.end);
		length = (unsigned)file.tellg();
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
		if (!buffer.empty())
		{// Buffer mode
			// bounds check
			if (currentPos >= buffer.size())
			{
				return 0;
			}

			return buffer.substr(currentPos);
		}
		else
		{// Stream mode
			return std::string(std::istreambuf_iterator<char>(file), (std::istreambuf_iterator<char>()));
		}
	}

	std::string FileReader::GetLine()
	{
		if (!buffer.empty())
		{// Buffer mode
			// bounds check
			if (currentPos >= buffer.size())
			{
				return 0;
			}

			std::string line = buffer.substr(currentPos, buffer.find('\n', currentPos) - currentPos);
			currentPos += line.size() + 1; //+ 1 for '\n'
			return line;
		}
		else
		{// Stream mode
			std::string result;
			std::getline(file, result);
			return result;
		}
	}

	std::string FileReader::GetWord()
	{
		if (!buffer.empty())
		{// Buffer mode
			// bounds check
			if (currentPos >= buffer.size())
			{
				return std::string();
			}

			std::string word = buffer.substr(currentPos, buffer.find_first_of(" \n", currentPos) - currentPos);
			currentPos += word.size();
			return word;
		}
		else
		{// Stream mode
			std::string result;
			file >> result;
			return result;
		}
	}

	float FileReader::GetFloat()
	{
		if (!buffer.empty())
		{// Buffer mode
			// bounds check
			if (currentPos >= buffer.size())
			{
				return 0.0f;
			}

			return std::stof(&buffer[currentPos], &currentPos);
		}
		else
		{// Stream mode
			float result;
			file >> result;
			return result;
		}
	}

	int FileReader::GetInt()
	{
		if (!buffer.empty())
		{// Buffer mode
			// bounds check
			if (currentPos >= buffer.size())
			{
				return 0;
			}

			return std::stoi(&buffer[currentPos], &currentPos);
		}
		else
		{// Stream mode
			int result;
			file >> result;
			return result;
		}
	}

	char FileReader::GetChar()
	{
		if (!buffer.empty())
		{// Buffer mode
			// bounds check
			if (currentPos >= buffer.size())
			{
				return 0;
			}

			return buffer[currentPos++];
		}
		else
		{// Stream mode
			return (char)file.get();
		}
	}

	bool FileReader::IsEOF() const
	{
		if (!buffer.empty())
		{// Buffer mode
			return currentPos >= buffer.size();
		}
		else
		{// Stream mode
			return file.eof();
		}
	}

	int FileReader::GetSize() const
	{
		return length;
	}
}
