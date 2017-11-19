#pragma once

#include <string>

using CubeType = size_t;

struct CubeType_raw
{
	int32_t ID;
	std::string name;
	int32_t transparent;
	bool destroyable;
	int32_t hardness;
};