// Copyright (c) 2017 Emilian Cioca
#pragma once
#include <fstream>
#include <string>
#include <string_view>
#include <vector>

namespace gem
{
	class DirectoryData
	{
	public:
		std::vector<std::string> files;
		std::vector<std::string> folders;
	};

	// Returns the maximum length of a filepath on the system (null terminator included).
	unsigned GetMaxPathLength();

	// Enumerates the current directory caches all filenames and folder names it contains.
	bool ParseDirectory(DirectoryData& outData);

	// Enumerates a directory and caches all filenames and folder names it contains.
	bool ParseDirectory(DirectoryData& outData, std::string_view directory);

	// Returns true if the specified string contains a valid, existing path.
	bool DirectoryExists(std::string_view directory);

	// Returns true if the specified path does not start with a drive letter.
	bool IsPathRelative(std::string_view path);

	// Returns true if the specified path starts with a drive letter.
	bool IsPathAbsolute(std::string_view path);

	// Returns true if the specified string contains a valid, existing file.
	bool FileExists(std::string_view file);

	// Deletes the specified file.
	bool RemoveFile(std::string_view file);

	// Attempts to create the specified directory. Returns true on success.
	bool MakeDirectory(std::string_view directory);

	// Returns the current Directory path.
	std::string GetCurrentDirectory();

	// Returns the current Directory path.
	void SetCurrentDirectory(std::string_view directory);

	// Saves the current directory on a global stack before changing it to the new directory.
	void PushCurrentDirectory(std::string_view newDirectory);

	// Restores the last current directory path pushed with PushCurrentDirectory().
	void PopCurrentDirectory();

	// Returns the drive letter specified in the path, if one exists.
	char ExtractDriveLetter(std::string_view path);

	// Returns the path specified in the string, including the drive letter and ignoring filenames.
	std::string ExtractPath(std::string_view path);

	// Returns the name of any file specified in the string, extension included.
	std::string ExtractFile(std::string_view path);

	// Returns the name of any file specified in the string, without any extension.
	std::string ExtractFilename(std::string_view path);

	// Returns the extension of any file specified in the string, '.' included.
	std::string ExtractFileExtension(std::string_view path);

	// Loads all the contents of a file into the provided string.
	bool LoadFileAsString(std::string_view file, std::string& output);

	// Streams input from a file or loads a file as a buffer.
	class FileReader
	{
	public:
		FileReader() = default;
		~FileReader();

		bool operator!() const;

		// Opens the file and loads it into RAM.
		bool OpenAsBuffer(std::string_view filePath);
		// Opens the file for reading on command.
		bool OpenAsStream(std::string_view filePath);
		// Closes the file or releases memory from RAM.
		void Close();

		// Returns the contents from the current position to the end of the file.
		std::string GetContents();
		// Returns one line of text.
		std::string GetLine();
		// Returns the next spaced word.
		std::string GetWord();
		// Gets the next word as a float.
		float GetFloat();
		// Gets the next word as an int.
		int GetInt();
		// Gets the next character.
		char GetChar();

		// Returns true if the current position in the file is at the end.
		bool IsEOF() const;

		// Returns the size of the file in bytes.
		int GetSize() const;

	private:
		std::ifstream file;
		std::string buffer;

		unsigned currentPos = 0;
		unsigned length = 0;
	};
}
