// Copyright (c) 2022 Emilian Cioca
#pragma once
#include <gemcutter/Resource/Encoder.h>

class SoundEncoder final : public gem::Encoder
{
public:
	SoundEncoder();

	gem::ConfigTable GetDefault() const override;

	bool Validate(const gem::ConfigTable& metadata, unsigned loadedVersion) const override;

private:
	bool Convert(std::string_view source, std::string_view destination, const gem::ConfigTable& metadata) const override;

	bool Upgrade(gem::ConfigTable& metadata, unsigned loadedVersion) const override;
};
