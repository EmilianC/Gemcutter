#pragma once
#include "Jewel3D/Resource/Encoder.h"

#include <string>

class FontEncoder : public Jwl::Encoder
{
public:
	FontEncoder();

	//- 
	virtual Jwl::ConfigTable GetDefault() const override;

	//- 
	virtual bool Validate(const Jwl::ConfigTable& data, unsigned loadedVersion) const override;

	//- 
	virtual bool Convert(const std::string& source, const std::string& destination, const Jwl::ConfigTable& data) const override;
};
