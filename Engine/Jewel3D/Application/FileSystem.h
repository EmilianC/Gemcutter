// Copyright (c) 2017 Emilian Cioca
#pragma once
#include <fstream>
#include <string>
#include <vector>

//Resolve name conflict with our functions
#undef GetCurrentDirectory
#undef SetCurrentDirectory

namespace Jwl
{
	struct DirectoryData
	{
		std::vector<std::string> files;
		std::vector<std::string> folders;
	};

	//- Enumerates the current directory caches all filenames and folder names it contains.
	bool ParseCurrentDirectory(DirectoryData& outData);

	//- Enumerates a directory and caches all filenames and folder names it contains.
	bool ParseDirectory(const std::string& directory, DirectoryData& outData);

	//- Returns true if the specified string contains a valid, existing path.
	bool DirectoryExists(const std::string& directory);

	//- Returns true if the specified path does not start with a drive letter.
	bool IsPathRelative(const std::string& path);

	//- Returns true if the specified path starts with a drive letter.
	bool IsPathAbsolute(const std::string& path);

	//- Returns true if the specified string contains a valid, existing file.
	bool FileExists(const std::string& file);

	//- Attempts to create the specified directory. Returns true on success.
	bool MakeDirectory(const std::string& directory);

	//- Returns the current Directory path.
	std::string GetCurrentDirectory();

	//- Returns the current Directory path.
	void SetCurrentDirectory(const std::string& directory);

	//- Saves the current directory on a global stack before changing it to the new directory.
	void PushCurrentDirectory(const std::string& newDirectory);

	//- Restores the last current directory path pushed with PushCurrentDirectory().
	void PopCurrentDirectory();

	//- Returns the drive letter specified in the string, followed by a ':'.
	std::string ExtractDriveLetter(const std::string& path);

	//- Returns the path specified in the string, including the drive letter and ignoring filenames.
	std::string ExtractPath(const std::string& path);

	//- Returns the name of any file specified in the string, extension included.
	std::string ExtractFile(const std::string& path);

	//- Returns the name of any file specified in the string, without any extension.
	std::string ExtractFilename(const std::string& path);

	//- Returns the extension of any file specified in the string, '.' included.
	std::string ExtractFileExtension(const std::string& path);

	//- Loads all the content of a file as a string, and returns the result.
	std::string LoadFileAsString(const std::string& file);

	//- Streams input from a file or loads a file as a buffer.
	class FileReader
	{
	public:
		FileReader() = default;
		~FileReader();

		bool operator!() const;

		//- Opens the file and loads it into RAM.
		bool OpenAsBuffer(const std::string& filePath);
		//- Opens the file for reading on command.
		bool OpenAsStream(const std::string& filePath);
		//- Closes the file or releases memory from RAM.
		void Close();

		//- Returns the contents from the current position to the end of the file.
		std::string GetContents();
		//- Returns one line of text.
		std::string GetLine();
		//- Returns the next spaced word.
		std::string GetWord();
		//- Gets the next word as a float.
		float GetFloat();
		//- Gets the next word as an int.
		int GetInt();
		//- Gets the next character.
		char GetChar();
		
		//- Returns true if the current position in the file is at the end.
		bool IsEOF() const;

		//- Returns the size of the file in bytes.
		int GetSize() const;

	private:
		std::ifstream file;
		std::string buffer;

		unsigned currentPos = 0;
		unsigned length = 0;
	};
}
