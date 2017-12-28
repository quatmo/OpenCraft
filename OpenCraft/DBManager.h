#pragma once

#include <string>
#include <set>
#include <memory>
#include <mutex>
#include <unordered_set>
#include <vector>

#include "ext\sqlite3\sqlite3.h"

#include "Chunk.h"
#include "Cube.h"

class DBManager
{
public:

	DBManager(std::string dbName_t);

	DBManager(const DBManager&) = delete;
	DBManager& operator=(const DBManager&) = delete;
	
	void exec(const std::string& query);
	void registerCubeType(const CubeType_raw& cubeType_t);
	void addChunk(const Chunk& chunk_t);
	void addChunkFast(const int32_t p_t, const int32_t q_t, const int32_t weather, const int32_t ecosystem,
		const std::vector<Cube>& cubes);
	
	std::set<std::pair<int, int>> fetchChunkSet();
	std::shared_ptr<Chunk> fetchChunk(const int32_t p_t, const int32_t q_t);
	CubeType_raw fetchCubeType(const size_t ID_t);

	~DBManager();
private:
	sqlite3* db;
	char* msg;
	std::mutex mtx;
};

