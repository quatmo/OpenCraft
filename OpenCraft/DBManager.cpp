#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <stdlib.h>
#include <assert.h>

#include "DBManager.h"

int fetch_cubetype_callback(void* para, int nCount, char** pValue, char** pName)
{
	std::stringstream ss;
	std::string string_value;
	int32_t int32_t_value;
	assert(nCount == 5);
	auto p = static_cast<CubeType_raw*>(para);
	for (int i = 0; i < nCount; i++)
	{
		ss << pValue[i] << " ";
	}
	ss >> int32_t_value; // ID
	p->ID = int32_t_value;
	ss >> string_value;  // name
	p->name = string_value;
	ss >> int32_t_value;  // transparent
	p->transparent = int32_t_value;
	ss >> int32_t_value; // destroyable
	p->destroyable = int32_t_value == 1 ? true : false;
	ss >> int32_t_value;  // hardness
	p->hardness = int32_t_value;
	return 0;
}

int fetch_chunk_callback(void* para, int nCount, char** pValue, char** pName)
{
	std::stringstream ss;
	int32_t int32_t_value;
	assert(nCount == 4);
	auto ptr = static_cast<Chunk*>(para);
	for (int i = 0; i < nCount; i++)
	{
		ss << pValue[i] << " ";
	}
	ss >> int32_t_value; // p
	ptr->p = int32_t_value;
	ss >> int32_t_value;  // q
	ptr->q = int32_t_value;
	ss >> int32_t_value;  // weather
	ptr->weather = int32_t_value;
	ss >> int32_t_value; // ecosystem
	ptr->ecosystem = int32_t_value;
	return 0;
}

std::string init_sql_01 =
"CREATE TABLE CubeType (   "\
"ID INT PRIMARY KEY NOT NULL,"\
"name VARCHAR(256) NOT NULL,"\
"transparent INT NOT NULL,"\
"destroyable INT NOT NULL,"\
"hardness INT NOT NULL "\
")";

std::string init_sql_02 =
"create table Cube(					  "\
"	p int not null,					  "\
"	q int not null,					  "\
"	x int not null,					  "\
"	y int not null,					  "\
"	z int not null,					  "\
"	type int unsigned not null,		  "\
"	direction int unsigned not null,  "\
"	durability int not null,		  "\
"	primary key (p, q, x, y, z)		  "\
")";

std::string init_sql_03 =
"create table Chunk(				   "\
"	p int not null,					   "\
"	q int not null,					   "\
"	weather int unsigned not null,	   "\
"	ecosystem int unsigned not null,   "\
"	primary key(p, q)				   "\
")									   ";

DBManager::DBManager(std::string dbName_t)
{
	std::fstream file;
	bool firstCreate = false;
	file.open(dbName_t, std::ios::in);
	if (!file.is_open())
	{
		firstCreate = true;
	}
	auto ret = sqlite3_open(dbName_t.c_str(), &db);
	std::cerr << "Open Database successfully!" << std::endl;
	if (ret != 0)
	{
		std::cerr << "Failed to open database!" << std::endl;
		std::cerr << sqlite3_errmsg(db) << std::endl;
	}
	if (firstCreate)
	{
		std::cerr << "First time starting the game, start initialization" << std::endl;
		this->exec(init_sql_01);
		this->exec(init_sql_02);
		this->exec(init_sql_03);
		std::cerr << "Initialization completes" << std::endl;
	}
}

void DBManager::exec(const std::string & query)
{
	auto ret = sqlite3_exec(db, query.c_str(), 0, 0, &msg);
	if (ret != SQLITE_OK)
	{
		std::cerr << "Failed to execute query: " << query << std::endl;
		std::cerr << msg << std::endl;
	}
}

void DBManager::registerCubeType(const CubeType_raw & cubeType_t)
{
	std::stringstream sql;
	sql << "insert into CubeType values( ";
	sql << cubeType_t.ID << ", ";
	sql << "'" << cubeType_t.name << "'" << ", ";
	sql << cubeType_t.transparent << ", ";
	sql << (cubeType_t.destroyable ? 1 : 0) << ", ";
	sql << cubeType_t.hardness << " )";
	this->exec(sql.str());
}

void DBManager::addChunk(const Chunk & chunk_t)
{
	mtx.lock();
	std::stringstream sql;
	sql << "insert into Chunk values( ";
	sql << chunk_t.p << ", ";
	sql << chunk_t.q << ", ";
	sql << chunk_t.weather << ", ";
	sql << chunk_t.ecosystem << "  )";
	this->exec(sql.str());
	this->exec("begin");
	const char* fmt = "insert into cube values (?, ?, ?, ?, ?, ?, ?, ?)";
	sqlite3_stmt *stmt;
	sqlite3_prepare_v2(db, fmt, strlen(fmt), &stmt, 0);
	for (int x = 0; x < 16; x++)
	{
		for (int y = 0; y < 16; y++)
		{
			for (int z = 0; z < 256; z++)
			{
				auto&& tmp = chunk_t.cubes[x * 256 * 16 + y * 256 + z];
				if (tmp.type == 0) // no need to store the air
				{
					continue;
				}
				sqlite3_reset(stmt);
				sqlite3_bind_int(stmt, 1, chunk_t.p);
				sqlite3_bind_int(stmt, 2, chunk_t.q);
				sqlite3_bind_int(stmt, 3, x);
				sqlite3_bind_int(stmt, 4, y);
				sqlite3_bind_int(stmt, 5, z);
				sqlite3_bind_int(stmt, 6, tmp.type);
				sqlite3_bind_int(stmt, 7, tmp.direction);
				sqlite3_bind_int(stmt, 8, tmp.durability);
				sqlite3_step(stmt);
			}
		}
	}
	sqlite3_finalize(stmt);
	this->exec("commit");
	mtx.unlock();
}

void DBManager::addChunkFast(const int32_t p_t, const int32_t q_t, const int32_t weather, const int32_t ecosystem, const std::vector<Cube>& cubes)
{
	mtx.lock();
	std::stringstream sql;
	sql << "insert into Chunk values( ";
	sql << p_t << ", ";
	sql << q_t << ", ";
	sql << weather << ", ";
	sql << ecosystem << "  )";
	this->exec(sql.str());
	this->exec("begin");
	const char* fmt = "insert into cube values (?, ?, ?, ?, ?, ?, ?, ?)";
	sqlite3_stmt *stmt;
	sqlite3_prepare_v2(db, fmt, strlen(fmt), &stmt, 0);
	for (const auto& cube : cubes)
	{
		sqlite3_reset(stmt);
		sqlite3_bind_int(stmt, 1, p_t);
		sqlite3_bind_int(stmt, 2, q_t);
		sqlite3_bind_int(stmt, 3, cube.x);
		sqlite3_bind_int(stmt, 4, cube.y);
		sqlite3_bind_int(stmt, 5, cube.z);
		sqlite3_bind_int(stmt, 6, cube.type);
		sqlite3_bind_int(stmt, 7, cube.direction);
		sqlite3_bind_int(stmt, 8, cube.durability);
		sqlite3_step(stmt);
	}
	sqlite3_finalize(stmt);
	this->exec("commit");
	mtx.unlock();
}

std::set<std::pair<int, int>> DBManager::fetchChunkSet()
{
	std::set<std::pair<int, int>> res;
	this->exec("begin");
	sqlite3_stmt *stmt;
	const char *fmt = "select * from Chunk";
	sqlite3_prepare_v2(db, fmt, strlen(fmt), &stmt, 0);
	sqlite3_reset(stmt);
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		auto p = sqlite3_column_int(stmt, 0);
		auto q = sqlite3_column_int(stmt, 1);
		res.insert({ p,q });
	}
	sqlite3_finalize(stmt);
	this->exec("commit");
	return res;
}

std::shared_ptr<Chunk> DBManager::fetchChunk(const int32_t p_t, const int32_t q_t)
{
	std::shared_ptr<Chunk> res = std::make_shared<Chunk>();
	std::stringstream sql;
	sql << "select * from Chunk where p = " << p_t << " and q = " << q_t;
	auto ret = sqlite3_exec(db, sql.str().c_str(), fetch_chunk_callback, res.get(), &msg);
	if (ret != SQLITE_OK)
	{
		std::cerr << "Failed to execute query: " << sql.str() << std::endl;
		std::cerr << msg << std::endl;
	}
	if (res->weather == 0) // not in the db
	{

		return res;
	}
	this->exec("begin");
	sqlite3_stmt *stmt;
	const char *fmt = "select * from Cube where p = ? and q = ?";
	sqlite3_prepare_v2(db, fmt, strlen(fmt), &stmt, 0);
	sqlite3_reset(stmt);
	sqlite3_bind_int(stmt, 1, p_t);
	sqlite3_bind_int(stmt, 2, q_t);
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		auto x = sqlite3_column_int(stmt, 2);
		auto y = sqlite3_column_int(stmt, 3);
		auto z = sqlite3_column_int(stmt, 4);
		if (x < 0 || y < 0 || z < 0)
		{
			continue;
		}
		auto&& ref = res->cubes[x * 256 * 16 + y * 256 + z];
		ref.type = sqlite3_column_int(stmt, 5);
		ref.direction = sqlite3_column_int(stmt, 6);
		ref.durability = sqlite3_column_int(stmt, 7);
	}
	sqlite3_finalize(stmt);
	this->exec("commit");
	return res;
}

CubeType_raw DBManager::fetchCubeType(const size_t ID_t)
{
	std::stringstream sql;
	sql << "select * from CubeType where ID = " << ID_t;
	CubeType_raw tmp;
	auto ret = sqlite3_exec(db, sql.str().c_str(), fetch_cubetype_callback, &tmp, &msg);
	if (ret != SQLITE_OK)
	{
		std::cerr << "Failed to execute query: " << sql.str() << std::endl;
		std::cerr << msg << std::endl;
	}
	return tmp;
}

DBManager::~DBManager()
{
	sqlite3_close(db);
}
