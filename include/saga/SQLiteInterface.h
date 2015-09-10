#ifndef SAGA_SQLITEINTERFACE_H
#define SAGA_SQLITEINTERFACE_H

#include <vector>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include "sqlite3/sqlite3.h"

#ifdef _OPENMP
    #include "omp.h"
#endif


namespace saga {

class SQLiteDB
{
public:
    SQLiteDB();
    ~SQLiteDB();
	
    bool open(std::string filename);
    bool close();
    std::vector<std::vector<std::string> > query(char* queryString);
    
    #ifdef _OPENMP
        sqlite3** getSQLiteDatabase();
    #else
        sqlite3* getSQLiteDatabase();
    #endif
    

};


} // namespace


#endif