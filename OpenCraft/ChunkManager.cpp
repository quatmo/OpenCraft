#include <iostream>
#include <vector>
#include <thread>
#include <limits>
#include <stack>
#include <mutex>
#include "ext/glm/gtc/noise.hpp"
#include "ChunkManager.h"



ChunkManager::ChunkManager(const glm::vec3 cameraPos)
	: m_db{ "opcraft" },
	m_p(static_cast<int>(cameraPos[0] / 4) - (cameraPos[0] < 0 ? 1 : 0)),
	m_q(static_cast<int>(cameraPos[2] / 4) - (cameraPos[2] < 0 ? 1 : 0)),
	m_modified(false),
	m_subtleModified(false)
{
	for (int dp = -8; dp < 8; dp++)
	{
		for (int dq = -8; dq < 8; dq++) // for each chunk
		{
			m_chunkMap[m_p + dp][m_q + dq] = loadBasicChunk(m_p + dp, m_q + dq);
		}
	}
}

std::array<std::shared_ptr<Chunk>, 5 * 5> ChunkManager::getChunks(int* updated)
{
	static int pre_m = std::numeric_limits<int>::max();
	static int pre_n = std::numeric_limits<int>::max();
	static std::array<std::shared_ptr<Chunk>, 5 * 5> res;
	if (pre_m == m_p && pre_n == m_q && m_modified == false) // no need to change the chunk
	{
		*updated = -1;
		return res;
	}
	m_modified = false;
	pre_m = m_p;
	pre_n = m_q;
	int index = 0;
	for (int dp = -2; dp <= 2; dp++)
	{
		for (int dq = -2; dq <= 2; dq++) // for each chunk
		{
			res[index++] = m_chunkMap[m_p + dp][m_q + dq];
		}
	}
	return res;
}

const std::list<RanderUnit>& ChunkManager::getRenderUnit(int *updated,const glm::vec3 pos)
{

	if (m_subtleModified == true)
	{
		m_subtleModified = false;
		return m_blocks;
	}
	static int pre_m = std::numeric_limits<int>::max();
	static int pre_n = std::numeric_limits<int>::max();
	if (pre_m == m_p && pre_n == m_q && m_modified == false) // no need to change the chunk
	{
		*updated = -1;
		return m_blocks;
	}
	m_modified = false;
	pre_m = m_p;
	pre_n = m_q;
	auto realPos = calRealPos(pos);
	auto chunkP = std::get<0>(realPos);
	auto chunkQ = std::get<1>(realPos);
	auto cubeX = std::get<2>(realPos);
	auto cubeY = std::get<3>(realPos);
	auto cubeZ = std::get<4>(realPos);
	floodSearch(chunkP,chunkQ, cubeX, cubeY, cubeZ);
	/*std::set<std::tuple<int, int, int, int, int,CubeType,int>> blockSet;
	int counter = 0;
	for (auto block : m_blocks)
	{

		if (blockSet.find({ block.p,block.q,block.x,block.y,block.z,block.type,block.durability }) != blockSet.end())
		{
			counter++;
		}
		blockSet.insert({ block.p,block.q,block.x,block.y,block.z,block.type,block.durability });
	}
	std::cerr << counter << " :duplicated" << std::endl;*/
	std::cerr << m_blocks.size() << " blocks rendered" << std::endl;
	return m_blocks;
}

float ChunkManager::calFloorDistance(const glm::vec3 pos)
{
	auto realPos = calRealPos(pos);
	auto chunkP = std::get<0>(realPos);
	auto chunkQ = std::get<1>(realPos);
	auto cubeX = std::get<2>(realPos);
	auto cubeY = std::get<3>(realPos);
	auto cubeZ = std::get<4>(realPos);

	float res = pos.y - cubeY*0.25f;
	while (true)
	{
		cubeY -= 1;
		auto type = m_chunkMap[chunkP][chunkQ]->cubes[cubeX * 16 * 256 + cubeZ * 256 + cubeY].type;
		if (type!= 0 && type < 1024)
		{
			break;
		}
		res += 0.25f;
	}
	return res;
}

bool ChunkManager::testSurrounding(const int p, const int q, const int x, const int y, const int z)
{
	if (y == 0)
	{
		return false;
	}
	auto wordPos = calWorldPos(p, q, x, y, z);
	std::vector<glm::vec3> testVec = {
		glm::vec3(-0.25f, 0, 0),
		glm::vec3(0.25f, 0, 0),
		glm::vec3(0, 0.25f, 0),
		glm::vec3(0, -0.25f, 0),
		glm::vec3(0, 0, 0.25f),
		glm::vec3(0, 0, -0.25f),
	};
	for (auto testDir : testVec)
	{
		auto realPosToProbe = calRealPos(wordPos+testDir);
		auto && chunk = m_chunkMap[std::get<0>(realPosToProbe)][std::get<1>(realPosToProbe)];
		auto probeX = std::get<2>(realPosToProbe);
		auto probeY = std::get<3>(realPosToProbe);
		auto probeZ = std::get<4>(realPosToProbe);
		auto type = chunk->cubes[probeX * 16 * 256 + probeZ * 256 + probeY].type;
		if (type == 0 || type > 1024) // it's an transparent block
		{
			return false;
		}
	}
	return true;
}

bool ChunkManager::hitDetection(const glm::vec3 pos)
{
	auto realPos = calRealPos(pos);
	auto chunkP = std::get<0>(realPos);
	auto chunkQ = std::get<1>(realPos);
	auto cubeX = std::get<2>(realPos);
	auto cubeY = std::get<3>(realPos);
	auto cubeZ = std::get<4>(realPos);

	auto hitType = m_chunkMap[chunkP][chunkQ]->cubes[cubeX * 16 * 256 + cubeZ * 256 + cubeY].type;
	if (hitType == 0 || hitType > 1024)
	{
		return false;
	}
	return true;
}

void ChunkManager::updateChunks(const glm::vec3 cameraPos)
{
	static int pre_m = -1;
	static int pre_n = -1;
	auto realPos = calRealPos(cameraPos);
	m_p = std::get<0>(realPos);
	m_q = std::get<1>(realPos);
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
	auto realPos = calRealPos(cameraPos);
	auto cubeX = std::get<2>(realPos);
	auto cubeY = std::get<3>(realPos);
	auto cubeZ = std::get<4>(realPos);
	auto probePos = cameraPos;
	for (int step = 1; step <= 6; step++) // R=6 sphere probe
	{
		probePos += frontVec3*0.25f;
		auto realPosToProbe = calRealPos(probePos);
		auto && chunk = m_chunkMap[std::get<0>(realPosToProbe)][std::get<1>(realPosToProbe)];
		auto probeX = std::get<2>(realPosToProbe);
		auto probeY = std::get<3>(realPosToProbe);
		auto probeZ = std::get<4>(realPosToProbe);
		auto type = chunk->cubes[probeX * 16 * 256 + probeZ * 256 + probeY].type;
		if (type != 0) // it's a none-air block
		{
			auto durability = chunk->cubes[probeX * 16 * 256 + probeZ * 256 + probeY].durability;
			if (durability == 10 || type > 1024)
			{
				chunk->cubes[probeX * 16 * 256 + probeZ * 256 + probeY].type = 0;
				chunk->cubes[probeX * 16 * 256 + probeZ * 256 + probeY].durability = 0;
				m_modified = true;
			}
			else
			{
				chunk->cubes[probeX * 16 * 256 + probeZ * 256 + probeY].durability++;
				for (auto&& block : m_blocks)
				{
					if (block.p == std::get<0>(realPosToProbe) &&
						block.q == std::get<1>(realPosToProbe) &&
						block.x == probeX &&
						block.y == probeY &&
						block.z == probeZ)
					{
						block.durability = chunk->cubes[probeX * 16 * 256 + probeZ * 256 + probeY].durability;
						break;
					}
				}
				m_subtleModified = true;
			}
			
			break;
		}
	}

}

void ChunkManager::tryPlaceCube(const glm::vec3 cameraPos, const glm::vec3 frontVec3,const CubeType cubeType)
{
	auto realPos = calRealPos(cameraPos);
	auto cubeX = std::get<2>(realPos);
	auto cubeY = std::get<3>(realPos);
	auto cubeZ = std::get<4>(realPos);
	auto probePos = cameraPos;
	for (int step = 1; step <= 6; step++) // R=6 sphere probe
	{
		probePos += frontVec3*0.25f;
		auto realPosToProbe = calRealPos(probePos);
		auto && chunk = m_chunkMap[std::get<0>(realPosToProbe)][std::get<1>(realPosToProbe)];
		auto probeX = std::get<2>(realPosToProbe);
		auto probeY = std::get<3>(realPosToProbe);
		auto probeZ = std::get<4>(realPosToProbe);
		if (chunk->cubes[probeX * 16 * 256 + probeZ * 256 + probeY].type != 0) // it's a none-air block
		{
			m_modified = true;
			probePos -= frontVec3*0.25f;
			break;
		}
	}

	if (m_modified = true)
	{
		auto realPosToPlace = calRealPos(probePos);
		auto && chunk = m_chunkMap[std::get<0>(realPosToPlace)][std::get<1>(realPosToPlace)];
		auto placeX = std::get<2>(realPosToPlace);
		auto placeY = std::get<3>(realPosToPlace);
		auto placeZ = std::get<4>(realPosToPlace);
		chunk->cubes[placeX * 16 * 256 + placeZ * 256 + placeY].type = cubeType;
	}

}

ChunkManager::~ChunkManager()
{
}

std::list<std::tuple<int, int, int, int, int>> searchStack;

void ChunkManager::floodSearch( const int p, const int q, const int x, const int y, const int z)
{
	m_blocks.clear();
	searchStack.clear();
	searchStack.push_back({ p,q,x,y,z });
	m_searched.clear();
	m_searched.insert({ p,q,x,y,z });
	while (!searchStack.empty())
	{
		auto currentBlock = searchStack.back();
		searchStack.pop_back();
		auto p = std::get<0>(currentBlock);
		auto q = std::get<1>(currentBlock);
		auto x = std::get<2>(currentBlock);
		auto y = std::get<3>(currentBlock);
		auto z = std::get<4>(currentBlock);
		auto type = m_chunkMap[p][q]->cubes[x * 16 * 256 + z * 256 + y].type;
		if (type != 0) // not air, not transparent
		{
			auto durability = m_chunkMap[p][q]->cubes[x * 16 * 256 + z * 256 + y].durability;
			m_blocks.push_back({ p,q,x,y,z,type,durability });
			if (type < 1024) // it's not transparent, we stop here
			{
				continue;
			}
		}
		if (y + 1 < 11) // top
		{
			if (m_searched.find({ p,q,x,y + 1,z }) == m_searched.end())
			{
				if (abs(p - m_p) < 5 && abs(q - m_q) < 5)
				{
					searchStack.push_back({ p,q,x,y + 1,z });
					m_searched.insert({ p,q,x,y + 1,z });
				}
			}
		}
		if (y - 1 >= 0) // bottom
		{
			if (m_searched.find({ p, q, x, y - 1, z }) == m_searched.end())
			{
				if (abs(p - m_p) < 5 && abs(q - m_q) < 5)
				{
					searchStack.push_back({ p, q, x, y - 1, z });
					m_searched.insert({ p,q,x,y -1,z });
				}
			}
		}
		if (x - 1 >= 0) // left
		{
			if (m_searched.find({ p, q, x - 1, y, z }) == m_searched.end())
			{
				if (abs(p - m_p) < 5 && abs(q - m_q) < 5)
				{
					searchStack.push_back({ p, q, x - 1, y, z });
					m_searched.insert({ p,q,x-1,y,z });
				}
			}
		}
		else
		{
			if (m_searched.find({ p - 1, q, 15, y, z }) == m_searched.end())
			{
				if (abs(p-1 - m_p) < 5 && abs(q - m_q) < 5)
				{
					searchStack.push_back({ p - 1, q, 15, y, z });
					m_searched.insert({ p-1,q,15,y,z });
				}

			}
		}
		if (x + 1 < 16) // right
		{
			if (m_searched.find({ p, q, x + 1, y, z }) == m_searched.end())
			{
				if (abs(p - m_p) < 5 && abs(q - m_q) < 5)
				{
					searchStack.push_back({ p, q, x + 1, y, z });
					m_searched.insert({ p,q,x+1,y,z });
				}
			}
		}
		else
		{
			if (m_searched.find({ p + 1, q, 0, y, z }) == m_searched.end())
			{
				if (abs(p+1 - m_p) < 5 && abs(q - m_q) < 5)
				{
					searchStack.push_back({ p + 1, q, 0, y, z });
					m_searched.insert({ p+1,q,0,y,z });
				}
			}
		}
		if (z + 1 < 16) // front
		{
			if (m_searched.find({ p, q, x, y, z + 1 }) == m_searched.end())
			{
				if (abs(p - m_p) < 5 && abs(q - m_q) < 5)
				{
					searchStack.push_back({ p, q, x, y, z + 1 });
					m_searched.insert({ p,q,x,y,z+1 });
				}
			}
		}
		else
		{
			if (m_searched.find({ p, q + 1, x, y, 0 }) == m_searched.end())
			{
				if (abs(p - m_p) < 5 && abs(q+1 - m_q) < 5)
				{
					searchStack.push_back({ p, q + 1, x, y, 0 });
					m_searched.insert({ p,q+1,x,y,0 });
				}
			}
		}
		if (z - 1 >= 0) // back
		{
			if (m_searched.find({ p, q, x, y, z - 1 }) == m_searched.end())
			{
				if (abs(p - m_p) < 5 && abs(q - m_q) < 5)
				{
					searchStack.push_back({ p, q, x, y, z - 1 });
					m_searched.insert({ p,q,x,y,z-1 });
				}
			}
		}
		else
		{
			if (m_searched.find({ p, q-1, x, y, 15 }) == m_searched.end())
			{
				if (abs(p - m_p) < 5 && abs(q -1- m_q) < 5)
				{
					searchStack.push_back({ p, q-1, x, y, 15 });
					m_searched.insert({ p,q-1,x,y,15 });
				}
			}
		}
	}

}

std::shared_ptr<Chunk> ChunkManager::loadBasicChunk(const int32_t p, const int32_t q)
{
	auto res = std::make_shared<Chunk>();
	res->p = p; // assemble the chunk
	res->q = q;
	res->weather = 1;
	if (res->p == 0 && res->q == 0)
	{
		for (int x = 0; x < 16; x++)
		{
			for (int z = 0; z < 16; z++)
			{
				res->cubes[x * 16 * 256 + z * 256 +0] = { x,0,z,1,0,0 };
			}
		}
		return res;
	}
	for (int x = 0; x < 16; x++)
	{
		for (int z = 0; z < 16; z++)
		{
			//auto y = 0;
			auto y_max = abs(static_cast<int>(8 * glm::perlin(glm::vec2((p * 10 + x)*0.1f, (q * 10 + z)*0.1f))));
			//res->cubes[x * 16 * 256 + z * 256 - y_max + 128] = { x,- y_max,z,1,0,0 };
			for (int y = 0; y < y_max; y++)
			{
				res->cubes[x * 16 * 256 + z * 256 + y] = { x,y,z,2,0,0 };
			}
			if (x == 8 && z == 8)
			{
				if (abs(static_cast<int>(8 * glm::perlin(glm::vec2((p * 10 + x)*0.2f, (q * 10 + z)*0.2f)))) > 3)
				{
					for (int dx = -3; dx <= 3; dx++) // render leaves
					{
						for (int dz = -3; dz <= 3; dz++)
						{
							for (int dy = -1; dy <= 3; dy++)
							{
								if (dx*dx + dy*dy + dz*dz <= 10)
								{
									res->cubes[(x + dx) * 16 * 256 + (z + dz) * 256 + y_max + 6 + dy] = { x + dx,y_max + 6 + dy,z + dz,5,0,0 };
								}
							}
						}
					}
					for (int y = y_max; y < y_max + 6; y++) // render tree trunk
					{
						res->cubes[x * 16 * 256 + z * 256 + y] = { x,y,z,4,0,0 };
					}
				}
			}
			res->cubes[x * 16 * 256 + z * 256 + y_max] = { x,y_max,z,1,0,0 };
		}
	}

	return res;
}

std::tuple<int, int, int, int, int> ChunkManager::calRealPos(const glm::vec3 worldPos)const
{
	// p,q,x,y,z
	std::tuple<int, int, int, int, int> res;
	std::get<0>(res) = static_cast<int>(worldPos[0] / 4) - (worldPos[0] < 0 ? 1 : 0); // p
	std::get<1>(res) = static_cast<int>(worldPos[2] / 4) - (worldPos[2] < 0 ? 1 : 0); // q
	std::get<2>(res) = static_cast<int>((worldPos[0] - std::get<0>(res) * 4) / 0.25); // x
	std::get<3>(res) = static_cast<int>(worldPos[1] / 0.25); // y
	std::get<4>(res) = static_cast<int>((worldPos[2] - std::get<1>(res) * 4) / 0.25); // z
	return res;
}

glm::vec3 ChunkManager::calWorldPos(const int p, const int q, const int x, const int y, const int z) const
{
	glm::vec3 res;
	res[0] = static_cast<float>(p * 4 + x*0.25 + 0.125);
	res[1] = static_cast<float>(y*0.25 + 0.125);
	res[2] = static_cast<float>(q * 4 + z*0.25 + 0.125);
	return res;
}

