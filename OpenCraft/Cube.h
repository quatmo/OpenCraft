#pragma once

#include <stdint.h>
#include "CubeType.h"

struct Cube
{
	CubeType type;
	int32_t direction;
	int32_t durability;
};