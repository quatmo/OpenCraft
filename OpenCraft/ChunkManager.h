#pragma once

#include <unordered_map>
#include <tuple>
#include <array>
#include <memory>

#include "ext/glm/glm.hpp"
#include "Chunk.h"
#include "DBManager.h"


class ChunkManager
{
public:
	ChunkManager(const glm::vec3 cameraPos);


	std::array<std::shared_ptr<Chunk>, 16 * 16> getChunks(void);

	void updateChunks(const glm::vec3 cameraPos);

	ChunkManager& operator=(const ChunkManager&) = delete;
	ChunkManager(const ChunkManager&) = delete;

	~ChunkManager();

private:
	DBManager m_db;
	int32_t m_p, m_q;
	std::unordered_map<int32_t,std::unordered_map<int32_t,std::shared_ptr<Chunk>>> m_chunkMap;
};

