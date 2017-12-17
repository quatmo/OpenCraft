#include <iostream>
#include "ext/glm/gtc/noise.hpp"
#include "ChunkManager.h"

ChunkManager::ChunkManager(const glm::vec3 cameraPos)
	: m_db{ "test" }, m_p(static_cast<int>(cameraPos[0] / 16)), m_q(static_cast<int>(cameraPos[2] / 16))
{
	for (int dp = -8; dp < 8; dp++)
	{
		for (int dq = -8; dq < 8; dq++) // for each chunk
		{
			std::shared_ptr<Chunk> chunk_t = std::make_shared<Chunk>();
			chunk_t->p = m_p + dp;
			chunk_t->q = m_q + dq;
			for (int x = 0; x < 16; x++)
			{
				for (int z = 0; z < 16; z++)
				{
					auto y = static_cast<int>(8*glm::perlin(glm::vec2((m_p*10 + dp*10+x)*0.1f, (m_q*10 + dq*10+z)*0.1f)));
					chunk_t->cubes[x * 16 * 256 + z * 256 + y + 128] = { 1,0,0 };
				}
			}
			m_chunkMap[m_p + dp][m_q + dq] = chunk_t;
		}
	}
}

std::array<std::shared_ptr<Chunk>, 16 * 16> ChunkManager::getChunks(void)
{
	std::array<std::shared_ptr<Chunk>, 16 * 16> res;
	int index = 0;
	for (int dp = -8; dp < 8; dp++)
	{
		for (int dq = -8; dq < 8; dq++) // for each chunk
		{
			res[index++] = m_chunkMap[m_p + dp][m_q + dq];
		}
	}
	return res;
}

void ChunkManager::updateChunks(const glm::vec3 cameraPos)
{
	m_p = static_cast<int>(cameraPos[0] / 16);
	m_q = static_cast<int>(cameraPos[2] / 16);
	static int counter = 0;
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
			std::shared_ptr<Chunk> chunk_t = std::make_shared<Chunk>();
			chunk_t->p = m_p + dp;
			chunk_t->q = m_q + dq;
			for (int x = 0; x < 16; x++)
			{
				for (int z = 0; z < 16; z++)
				{
					auto y = static_cast<int>(8 * glm::perlin(glm::vec2((m_p * 1007 + dp * 103 + x)*0.1f, (m_q * 1007 + dq * 103 + z)*0.1f)));
					chunk_t->cubes[x * 16 * 256 + z * 256 + y + 128] = { 1,0,0 };
				}
			}
			m_chunkMap[m_p + dp][m_q + dq] = chunk_t;
		}
	}
}

ChunkManager::~ChunkManager()
{
}
