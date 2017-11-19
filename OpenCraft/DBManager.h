#pragma once

#include <string>
#include <memory>

#include "ext\sqlite3\sqlite3.h"

#include "Chunk.h"
#include "CubeType.h"

class DBManager
{
public:

	DBManager(std::string dbName_t);

	DBManager(const DBManager&) = delete;
	DBManager& operator=(const DBManager&) = delete;
	
	void exec(const std::string& query);
	void registerCubeType(const CubeType_raw& cubeType_t);
	void addChunk(const Chunk& chunk_t);

	std::unique_ptr<Chunk> fetchChunk(const int32_t p_t, const int32_t q_t);
	CubeType_raw fetchCubeType(const size_t ID_t);

	~DBManager();
private:
	sqlite3* db;
	char* msg;
};

