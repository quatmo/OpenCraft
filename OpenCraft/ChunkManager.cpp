#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include "ext/glm/gtc/noise.hpp"
#include "ChunkManager.h"



ChunkManager::ChunkManager(const glm::vec3 cameraPos)
	: m_db{ "opcraft" }, m_p(static_cast<int>(cameraPos[0] / 16)), m_q(static_cast<int>(cameraPos[2] / 16))
{
	for (int dp = -8; dp < 8; dp++)
	{
		for (int dq = -8; dq < 8; dq++) // for each chunk
		{
			m_chunkMap[m_p + dp][m_q + dq] = loadBasicChunk(m_p + dp, m_q + dq);
		}
	}
}

std::array<std::shared_ptr<Chunk>, 9 *9> ChunkManager::getChunks(int* updated)
{
	static int pre_m = -1;
	static int pre_n = -1;
	static std::array<std::shared_ptr<Chunk>, 9*9> res;
	if (pre_m == m_p && pre_n == m_q) // no need to change the chunk
	{
		*updated = -1;
		return res;
	}
	pre_m = m_p;
	pre_n = m_q;
	int index = 0;
	for (int dp = -4; dp <= 4; dp++)
	{
		for (int dq = -4; dq <= 4; dq++) // for each chunk
		{
			res[index++] = m_chunkMap[m_p + dp][m_q + dq];
		}
	}
	return res;
}

void ChunkManager::updateChunks(const glm::vec3 cameraPos)
{
	static int pre_m = -1;
	static int pre_n = -1;
	m_p = static_cast<int>(cameraPos[0] / 16);
	m_q = static_cast<int>(cameraPos[2] / 16);
	if (pre_m == m_p && pre_n == m_q) // no need to update the chunk
	{
		return;
	}
	for (int dp = -8; dp < 8; dp++)
	{
		for (int dq = -8; dq < 8; dq++) // for each chunk
		{
			auto tmp = m_chunkMap.find(m_p + dp); // check if it's already in the chunk map
			if (tmp != m_chunkMap.end())
			{
				if (tmp->second.find(m_q + dq) != tmp->second.end())
				{
					continue; // m_chunkMap[p][q] exists
				}
			}
			m_chunkMap[m_p + dp][m_q + dq] = loadBasicChunk(m_p + dp, m_q + dq);
		}
	}
}

ChunkManager::~ChunkManager()
{
}

std::shared_ptr<Chunk> ChunkManager::loadBasicChunk(const int32_t p, const int32_t q)
{
	auto res = std::make_shared<Chunk>();
	res->p = p; // assemble the chunk
	res->q = q;
	res->weather = 1;
	for (int x = 0; x < 16; x++)
	{
		for (int z = 0; z < 16; z++)
		{
			//auto y = 0;
			auto y_max = abs(static_cast<int>(8 * glm::perlin(glm::vec2((p * 10 + x)*0.1f, (q * 10 + z)*0.1f))));
			res->cubes[x * 16 * 256 + z * 256 - y_max + 128] = { x,- y_max,z,1,0,0 };
			for (int y = 0; y > -y_max; y--)
			{
				res->cubes[x * 16 * 256 + z * 256 + y + 128] = { x,y,z,2,0,0 };
			}
		}
	}
	return res;
}

