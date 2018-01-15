#pragma once

#include <unordered_map>
#include <set>
#include <list>
#include <utility>
#include <tuple>
#include <array>
#include <memory>
#include <mutex>

#include <ext/glm/glm.hpp>
#include "Chunk.h"
#include "DBManager.h"


class ChunkManager
{
public:
	ChunkManager(const glm::vec3 cameraPos);

	std::array<std::shared_ptr<Chunk>, 5 * 5> getChunks(int *);
	const std::list<RanderUnit>& getRenderUnit(int*, const glm::vec3);
	std::tuple<int, int, int, int, int> calRealPos(const glm::vec3 worldPos)const;
	glm::vec3 calWorldPos(const int p, const int q, const int x, const int y, const int z)const;
	float calFloorDistance(const glm::vec3 pos);
	bool testSurrounding(const int p, const int q, const int x, const int y, const int z);
	bool hitDetection(const glm::vec3 pos);
	void updateChunks(const glm::vec3 cameraPos);
	void tryDestoryCube(const glm::vec3 cameraPos, const glm::vec3 frontVec3);
	void tryPlaceCube(const glm::vec3 cameraPos, const glm::vec3 frontVec3, const CubeType cubeType);
	std::set<std::tuple<int, int, int, int, int>> getLightPos(void)const;


	ChunkManager& operator=(const ChunkManager&) = delete;
	ChunkManager(const ChunkManager&) = delete;

	~ChunkManager();

private:
	const int FLOOD_SEARCH_DEPTH = 10000;
	DBManager m_db;
	int32_t m_p, m_q;
	bool m_modified;
	bool m_subtleModified;
	void floodSearch(const int p, const int q, const int x, const int y, const int z);
	std::unordered_map<int32_t, std::unordered_map<int32_t, std::shared_ptr<Chunk>>> m_chunkMap;
	std::set<std::pair<int, int>> m_chunkSet;
	std::list<RanderUnit> m_blocks;
	std::set<std::tuple<int, int, int, int, int>> m_searched;
	std::set<std::tuple<int, int, int, int, int>> m_pointLightPos;
	std::set<std::pair<int, int>> m_searchedPQ;
	std::shared_ptr<Chunk> loadBasicChunk(const int32_t p, const int32_t q);

};

