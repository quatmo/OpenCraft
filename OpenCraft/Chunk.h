#pragma once

#include <array>
#include <stdint.h>
#include "Cube.h"

constexpr int CHUNK_SIZE = 16 * 16 * 256;

struct Chunk
{
	int32_t p, q;
	int32_t weather;
	int32_t ecosystem;
	std::array<Cube, CHUNK_SIZE> cubes;
};

