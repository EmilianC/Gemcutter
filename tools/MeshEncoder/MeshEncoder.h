// Copyright (c) 2017 Emilian Cioca
#pragma once
#include <gemcutter/Resource/Encoder.h>

class MeshEncoder final : public gem::Encoder
{
public:
	MeshEncoder();

	gem::ConfigTable GetDefault() const override;

	bool Validate(const gem::ConfigTable& metadata, unsigned loadedVersion) const override;

private:
	bool Convert(std::string_view source, std::string_view destination, const gem::ConfigTable& metadata) const override;

	bool Upgrade(gem::ConfigTable& metadata, unsigned loadedVersion) const override;
};
