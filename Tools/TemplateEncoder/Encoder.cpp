#include "Encoder.h"

#define CURRENT_VERSION 1

Encoder::Encoder()
	: Jwl::Encoder(CURRENT_VERSION)
{
}

Jwl::ConfigTable Encoder::GetDefault() const
{
	Jwl::ConfigTable defaultConfig;

	defaultConfig.SetValue("version", CURRENT_VERSION);
	// Any default values for a new asset can be added to metadata here.

	return defaultConfig;
}

bool Encoder::Validate(const Jwl::ConfigTable& metadata, unsigned loadedVersion) const
{
	switch (loadedVersion)
	{
	case 1:
		// Check the presence of your metadata fields here.
		// Also ensure that they have correct values.
		//...

		if (metadata.GetSize() != 1)
		{
			Jwl::Error("Incorrect number of value entries.");
			return false;
		}
	}

	return true;
}

bool Encoder::Convert(std::string_view source, std::string_view destination, const Jwl::ConfigTable& metadata) const
{
	// Load the source file and output the built data to the destination folder.
	// The conversion should be done using the properties inside the metadata.
	//...

	return true;
}
