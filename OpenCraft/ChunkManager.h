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

	std::array<std::shared_ptr<Chunk>, 9*9> getChunks(int *);

	void updateChunks(const glm::vec3 cameraPos);
	void tryDestoryCube(const glm::vec3 cameraPos, const glm::vec3 frontVec3);


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

