#pragma once

#include <stdint.h>
#include "CubeType.h"

struct Cube
{
	int32_t x;
	int32_t y;
	int32_t z;
	CubeType type;
	int32_t direction;
	int32_t durability;
};