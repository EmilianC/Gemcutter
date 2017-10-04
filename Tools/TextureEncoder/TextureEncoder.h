#pragma once
#include "Jewel3D/Resource/Encoder.h"

#include <string>

class TextureEncoder : public Jwl::Encoder
{
public:
	TextureEncoder();

	Jwl::ConfigTable GetDefault() const override;

	bool Validate(const Jwl::ConfigTable& metadata, unsigned loadedVersion) const override;

	bool Convert(const std::string& source, const std::string& destination, const Jwl::ConfigTable& metadata) const override;

	bool Upgrade(Jwl::ConfigTable& metadata, unsigned loadedVersion) const override;
};
