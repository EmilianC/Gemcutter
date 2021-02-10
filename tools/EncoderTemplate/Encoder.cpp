#include "Encoder.h"

#define CURRENT_VERSION 1

Encoder::Encoder()
	: gem::Encoder(CURRENT_VERSION)
{
}

gem::ConfigTable Encoder::GetDefault() const
{
	gem::ConfigTable defaultConfig;

	defaultConfig.SetValue("version", CURRENT_VERSION);
	// Any default values for a new asset can be added to metadata here.

	return defaultConfig;
}

bool Encoder::Validate(const gem::ConfigTable& metadata, unsigned loadedVersion) const
{
	switch (loadedVersion)
	{
	case 1:
		// Check the presence of your metadata fields here.
		// Also ensure that they have correct values.
		//...

		if (metadata.GetSize() != 1)
		{
			gem::Error("Incorrect number of value entries.");
			return false;
		}
	}

	return true;
}

bool Encoder::Convert(std::string_view source, std::string_view destination, const gem::ConfigTable& metadata) const
{
	// Load the source file and output the built data to the destination folder.
	// The conversion should be done using the properties inside the metadata.
	//...

	return true;
}
