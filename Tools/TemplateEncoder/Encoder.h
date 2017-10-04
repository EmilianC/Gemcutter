#pragma once
#include "Jewel3D/Resource/Encoder.h"

#include <string>

class Encoder : public Jwl::Encoder
{
public:
	Encoder();

	Jwl::ConfigTable GetDefault() const override;

	bool Validate(const Jwl::ConfigTable& metadata, unsigned loadedVersion) const override;

	bool Convert(const std::string& source, const std::string& destination, const Jwl::ConfigTable& metadata) const override;
};
