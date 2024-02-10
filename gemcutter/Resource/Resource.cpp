// Copyright (c) 2023 Emilian Cioca
#include "Resource.h"

REFLECT(gem::ResourceBase)
	MEMBERS {
		REF_PRIVATE_MEMBER(path, gem::ReadOnly)
	}
REF_END;
