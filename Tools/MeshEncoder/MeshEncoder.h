// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "Jewel3D/Resource/Encoder.h"

#include <string>

class MeshEncoder : public Jwl::Encoder
{
public:
	MeshEncoder();

	virtual Jwl::ConfigTable GetDefault() const override;

	virtual bool Validate(const Jwl::ConfigTable& metadata, unsigned loadedVersion) const override;

	virtual bool Convert(const std::string& source, const std::string& destination, const Jwl::ConfigTable& metadata) const override;

	virtual bool Upgrade(Jwl::ConfigTable& metadata, unsigned loadedVersion) const override;
};
