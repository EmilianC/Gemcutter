#pragma once
#include <gemcutter/Resource/Encoder.h>

class TextureEncoder final : public Jwl::Encoder
{
public:
	TextureEncoder();

	Jwl::ConfigTable GetDefault() const override;

	bool Validate(const Jwl::ConfigTable& metadata, unsigned loadedVersion) const override;

private:
	bool Convert(std::string_view source, std::string_view destination, const Jwl::ConfigTable& metadata) const override;

	bool Upgrade(Jwl::ConfigTable& metadata, unsigned loadedVersion) const override;
};
