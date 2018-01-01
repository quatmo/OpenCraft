#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include "ext/glm/gtc/noise.hpp"
#include "ChunkManager.h"



ChunkManager::ChunkManager(const glm::vec3 cameraPos)
	: m_db{ "opcraft" }, 
	m_p(static_cast<int>(cameraPos[0] / 16) - (cameraPos[0]<0 ? 1 : 0)), 
	m_q(static_cast<int>(cameraPos[2] / 16) - (cameraPos[2]<0 ? 1 : 0)),
	m_modified(false)
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
	static int pre_m = 999;
	static int pre_n = 999;
	static std::array<std::shared_ptr<Chunk>, 9*9> res;
	if (pre_m == m_p && pre_n == m_q && m_modified == false) // no need to change the chunk
	{
		*updated = -1;
		return res;
	}
	m_modified = false;
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
	m_p = static_cast<int>(cameraPos[0] / 16) - (cameraPos[0]<0?1:0);
	m_q = static_cast<int>(cameraPos[2] / 16) - (cameraPos[2]<0?1:0);
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

void ChunkManager::tryDestoryCube(const glm::vec3 cameraPos, const glm::vec3 frontVec3)
{

	auto cubeX = static_cast<int>(cameraPos[0] - m_p *16);
	auto cubeY = static_cast<int>(cameraPos[1]);
	auto cubeZ = static_cast<int>(cameraPos[2] - m_q * 16);
	cubeX = cubeX < 0 ? cubeX + 16:cubeX;
	cubeZ = cubeZ < 0 ? cubeZ + 16:cubeZ;
	auto&& chunk = m_chunkMap[m_p][m_q];
	chunk->cubes[cubeX * 16 * 256 + cubeZ * 256 + cubeY].type = 3;
	m_modified = true;
	//auto probeX = frontVec3[0]+0.5;
	//auto probeY = frontVec3[1]+0.5;
	//auto probeZ = frontVec3[2]+0.5;
	//std::cerr << "probeX: " << probeX;
	//std::cerr << " probeY: " << probeY;
	//std::cerr << " probeZ: " << probeZ << std::endl;
	//auto&& chunk = m_chunkMap[m_p][m_q];
	//for (int step = 1; step < 6; step++)
	//{
	//	auto newCubeX = static_cast<int>(cubeX+probeX*step);
	//	auto newCubeY = static_cast<int>(cubeY+probeY*step);
	//	auto newCubeZ = static_cast<int>(cubeZ+probeZ*step);
	//	std::cerr << "newCubeX: " << newCubeX;
	//	std::cerr << " newCubeY: " << newCubeY;
	//	std::cerr << " newCubeZ: " << newCubeZ << std::endl;
	//	if (newCubeX > 16 || newCubeY > 256 || newCubeZ > 16)
	//	{
	//		std::cerr << "************Invalid!************" << std::endl;
	//		break;
	//	}
	//	if (newCubeX < 0 || newCubeY < 0 || newCubeZ < 0)
	//	{
	//		std::cerr << "************Invalid!************" << std::endl;
	//		break;
	//	}
	//	if (chunk->cubes[newCubeX * 16 * 256 + newCubeZ * 256 + newCubeY].type != 0)
	//	{
	//		std::cerr << "====================HIT===============" << std::endl;
	//		std::cerr << "m_p: " << m_p;
	//		std::cerr << " m_q: " << m_q<<std::endl;
	//		chunk->cubes[newCubeX * 16 * 256 + newCubeZ * 256 + newCubeY].type = 3;
	//		m_modified = true;
	//		return;
	//	}
	//	else
	//	{
	//		std::cerr << "====================MISS===============" << std::endl;
	//	}
	//}
	//std::cerr << "cubeX: " << cubeX << std::endl;
	//std::cerr << "cubeY: " << cubeY << std::endl;
	//std::cerr << "cubeZ: " << cubeZ << std::endl;
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
			//res->cubes[x * 16 * 256 + z * 256 - y_max + 128] = { x,- y_max,z,1,0,0 };
			for (int y = 0; y < y_max; y++)
			{
				res->cubes[x * 16 * 256 + z* 256 + y] = { x,y,z,2,0,0 };
			}
			res->cubes[x * 16 * 256 + z * 256 + y_max] = { x,y_max,z,1,0,0 };
		}
	}
	return res;
}

