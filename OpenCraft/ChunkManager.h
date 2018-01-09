#pragma once

#include <unordered_map>
#include <set>
#include <utility>
#include <tuple>
#include <array>
#include <memory>
#include <mutex>

#include "ext/glm/glm.hpp"
#include "Chunk.h"
#include "DBManager.h"


class ChunkManager
{
public:
	ChunkManager(const glm::vec3 cameraPos);

	std::array<std::shared_ptr<Chunk>,5*5> getChunks(int *);

	std::tuple<int, int, int, int, int> calRealPos(const glm::vec3 worldPos)const;
	glm::vec3 calWorldPos(const int p, const int q, const int x, const int y, const int z)const;
	float calFloorDistance(const glm::vec3 pos);
	bool testSurrounding(const int p, const int q, const int x, const int y, const int z);
	bool hitDetection(const glm::vec3 pos);
	void updateChunks(const glm::vec3 cameraPos);
	void tryDestoryCube(const glm::vec3 cameraPos, const glm::vec3 frontVec3);
	void tryPlaceCube(const glm::vec3 cameraPos, const glm::vec3 frontVec3,const CubeType cubeType);


	ChunkManager& operator=(const ChunkManager&) = delete;
	ChunkManager(const ChunkManager&) = delete;

	~ChunkManager();

private:
	DBManager m_db;
	int32_t m_p, m_q;
	bool m_modified;
	std::unordered_map<int32_t,std::unordered_map<int32_t,std::shared_ptr<Chunk>>> m_chunkMap;
	std::set<std::pair<int, int>> m_chunkSet;
	std::shared_ptr<Chunk> loadBasicChunk(const int32_t p, const int32_t q);


};

