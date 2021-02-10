#pragma once
#include <gemcutter/Resource/Encoder.h>

class MaterialEncoder final : public gem::Encoder
{
public:
	MaterialEncoder();

	gem::ConfigTable GetDefault() const override;

	bool Validate(const gem::ConfigTable& metadata, unsigned loadedVersion) const override;

private:
	bool Convert(std::string_view source, std::string_view destination, const gem::ConfigTable& metadata) const override;
};
