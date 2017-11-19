#include <iostream>
#include <string>

#include "CubeType.h"
#include "DBManager.h"

using namespace std;
				
int main(int argc, char **argv) 
{
	cout << "Let's go!" << endl;
	DBManager db{ "test" };
	cout << "OK, everything seems fine...." << endl;
	return 0;
}

/*
string sql_01 =
"CREATE TABLE CubeType (   "\
"ID INT PRIMARY KEY NOT NULL,"\
"name VARCHAR(256) NOT NULL,"\
"transparent INT NOT NULL,"\
"destroyable INT NOT NULL,"\
"hardness INT NOT NULL "\
")";

string sql_02 =
"Insert into CubeType values(0, 'air',0 ,0 ,0 )";

string sql_03 =
"select * from CubeType where ID = 0";
*/

/*
string sql_03 =
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
*/

/*
string sql_05 =
"create table Chunk(				   "\
"	p int not null,					   "\
"	q int not null,					   "\
"	weather int unsigned not null,	   "\
"	ecosystem int unsigned not null,   "\
"	primary key(p, q)				   "\
")									   ";
*/

/*
CubeType_raw t;
t.ID = 1;
t.name = "stone";
t.transparent = 255;
t.destroyable = true;
t.hardness = -1;
db.registerCubeType(t);
*/