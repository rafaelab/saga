#include "saga/SQLiteInterface.h"


namespace saga{

/* For CRPropa usage should be 1. Otherwise can be set manually for better reads of the table. */
const int maxNumThreads=1; 

int threadID;
sqlite3 *dbarr[maxNumThreads];    
sqlite3 *dbsing;    

static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
    int i;
    for(i=0; i<argc; i++){
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}


/*********************************************************************************************************/ 
// Initializes all SQL parameters prior to the run
//
SQLiteDB::SQLiteDB()
{

    #ifndef _OPENMP 
        std::cout << "SAGA will run *WITHOUT* OpenMP. Recompile the code including the option." << std::endl;
        std::cout << "We strongly advise you to enable this option to avoid problems with CRPropa." << std::endl;
    #else
        omp_set_num_threads(maxNumThreads);
        std::cout << "SAGA will run with OpenMP." << std::endl;
        std::cout << maxNumThreads << " threads will be used" << std::endl;
    #endif
    
    sqlite3_enable_shared_cache(1);

}

SQLiteDB::~SQLiteDB()
{
}

/*********************************************************************************************************/ 
// Opens the SQL database
// Input:
//     filename: name of the file
//
bool SQLiteDB::open(std::string filename)
{
    int fileRet;
    #ifndef _OPENMP
        fileRet = sqlite3_open(filename.c_str(), &dbsing);
    #else
        // opening connections with the database
        //int threadID;
        #pragma omp parallel private(fileRet,threadID) shared(filename, dbarr) default(none)
        {
            threadID = omp_get_thread_num();
            fileRet = sqlite3_open_v2(filename.c_str(), &dbarr[threadID], SQLITE_OPEN_READONLY | SQLITE_OPEN_NOMUTEX, NULL);
        }
    #endif
           // std::cout << "threadID " << threadID << std::endl;

    if(fileRet==0) {
        std::cout << "Database successfully opened." << std::endl;
        return true;
    }
    else {
        throw std::runtime_error("Failed to open the file.");
        return false;
    }
        
}

/*********************************************************************************************************/ 
// Closes the SQL database
//
bool SQLiteDB::close() 
{
    int fileRet;
    #ifndef _OPENMP
        fileRet = sqlite3_close(dbsing);
    #else
        //int threadID;
        // closing connections to the database
        #pragma omp parallel private(threadID,fileRet) shared(dbarr) default(none)
        {
            threadID = omp_get_thread_num();
            fileRet = sqlite3_close(dbarr[threadID]);
        }
    #endif

    if(fileRet==0)
        return true;
    else
            throw std::runtime_error("Failed to close the SQL database.");
        return false;
} 

/*********************************************************************************************************/ 
// Returns the result of a query
// Input:
//   queryString: contains the string to be used as query
// Output:
//   vector<vector> with the results of the search
//
std::vector<std::vector<std::string> > SQLiteDB::query(char* queryString)
{

	sqlite3_stmt *statement;
	std::vector<std::vector<std::string> > results;

    // single thread
    #ifndef _OPENMP
        if(sqlite3_prepare_v2(dbsing, queryString, -1, &statement, 0) == SQLITE_OK){
            int cols = sqlite3_column_count(statement);
            int result = 0;
            while(true){
                result = sqlite3_step(statement);
                if(result == SQLITE_ROW){
                    std::vector<std::string> values;
                    for(int col = 0; col < cols; col++){
                        char *charPtr = (char*)sqlite3_column_text(statement, col);
                        std::string str = charPtr;
                        values.push_back(str);
                        //std::cout << str << std::endl;
                    }
                    results.push_back(values);
                } else {
                    break;   
                }
            }
            sqlite3_finalize(statement);
        }
        std::string error = sqlite3_errmsg(dbsing);

    // multiple threads
    #else
        if(sqlite3_prepare_v2(dbarr[threadID], queryString, -1, &statement, 0) == SQLITE_OK){
            int cols = sqlite3_column_count(statement);
            int result = 0;
            char *charPtr;
            while(true){
                result = sqlite3_step(statement);
                if(result == SQLITE_ROW){
                    std::vector<std::string> values;
                    for(int col = 0; col < cols; col++){
                        char *charPtr = (char*)sqlite3_column_text(statement, col);
                        std::string str = charPtr;
                        values.push_back(str);
                    }
                    results.push_back(values);
                } else {
                    break;   
                }
            }
            sqlite3_finalize(statement);
        }
        std::string error = sqlite3_errmsg(dbarr[threadID]);
    #endif

	if(error != "not an error")  std::cout << queryString << " " << error << std::endl;
	
	return results;  
}

/*********************************************************************************************************/ 
// Returns the database
//
#ifdef _OPENMP
    sqlite3** SQLiteDB::getSQLiteDatabase() 
    {
        return dbarr;
    }
#else
    sqlite3* SQLiteDB::getSQLiteDatabase()
    {
        return dbsing;
    }
#endif




} // namespace