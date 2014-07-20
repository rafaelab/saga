/*********************************************************************************************/
/** SQLite3 Thread handler                                                                  **/
/**                                                                                         **/
/** Originally written by Marcus Grimm                                                      **/
/**   (http://www.sqlite.org/cvstrac/wiki?p=SampleCode)                                     **/
/**                                                                                         **/
/** This version was adapted by Rafael Alves Batista, 24/12/2013, to handle AMR grids       **/                                        **/
/**   in CRPropa with multithreading.                                                       **/
/**                                                                                         **/
/*********************************************************************************************/
/** This program basically simulates a server that opens multible DB connections that are   **/
/** within multible threads. Each thread uses his dedicated DB pointer.                     **/
/** The reader threads only parse some data of some tables.                                 **/
/** The write threads updates or inserts new data in some tables randomly.                  **/
/** All writes into the DB are covered with an exclusive transaction to lock the reader     **/
/** threads and/or other writer threads I don't use busy callback or busy timeout settings  **/
/** here, since I do the busy handling 'manually'. However, my handling here should         **/
/** equivalent to what sqlite would do with its internal busy handlers.                     **/
/** This code is public domain, you may use it freely for whatever purpose.                 **/
/*********************************************************************************************/

#include    <stdio.h>
#include    <time.h>
#include    <process.h>

#include    "sqlite3.h"



/** sqlite DB Filename **/
#define DBFILENAME        "testdb.db" 
#define MAX_OPENDBS        10    /** Total number of open DB == number of running threads **/
#define NUM_WRITE_THREADS  2    /** Number of running writing threads.                   **/
/** DB Connection pool **/
sqlite3            *DBPool[MAX_OPENDBS];    /** used within the threads     **/
sqlite3            *MainDB = NULL;         /** used by main process only.  **/
/** To have clear fprintfs in threads we use a lock for this **/
CRITICAL_SECTION  stderr_lock;
int global_run;
int global_sql_error = 0;
/** Timeout values on busy or lock conditions   **/
/** if you observe lock errors you might try to **/
/** increase the values.                        **/
#define SQLTM_COUNT       200  /** -> SQLTM_COUNT*SQLTM_TIME ms timeout **/
#define SQLTM_TIME        50

/******************************************************/
/** PrepareSql:                                      **/
/** This encapsulates sqlite prepare call to handle  **/
/** timeout condition.                               **/
/******************************************************/
int PrepareSql(sqlite3 *db, sqlite3_stmt  **hs, char *SqlStr, int handle)
{
    int n, rc;
    n = 0;
    do
    {
        rc = sqlite3_prepare_v2(db, SqlStr, -1, hs, 0);
        if( (rc == SQLITE_BUSY) || (rc == SQLITE_LOCKED) )
        {
            n++;
            Sleep(SQLTM_TIME);
        }
    }while( (n < SQLTM_COUNT) && ((rc == SQLITE_BUSY) || (rc == SQLITE_LOCKED)));
    if( rc != SQLITE_OK)
    {
        EnterCriticalSection(&stderr_lock);
        fprintf(stderr, "SqlPrepare-Error-H(%d): (%d) %s \n", handle, rc, sqlite3_errmsg(db));
        fprintf(stderr, "Statement: %s \n", SqlStr);
        LeaveCriticalSection(&stderr_lock);
        return(0);
    }
    return(1);
}


/** This encapsulates sqlite step call to handle timeout condition. **/
int        StepSql(sqlite3_stmt  *hs, int handle)
{
    int        rc, n;
    n = 0;
    do
    {
        rc = sqlite3_step(hs);
      if( rc == SQLITE_LOCKED )
      {
         rc = sqlite3_reset(hs); /** Note: This will return SQLITE_LOCKED as well... **/
         n++;
         Sleep(SQLTM_TIME);
      }
      else
        if( (rc == SQLITE_BUSY) )
        {
            Sleep(SQLTM_TIME);
            n++;
        }
    }while( (n < SQLTM_COUNT) && ((rc == SQLITE_BUSY) || (rc == SQLITE_LOCKED)));
    if( n == SQLTM_COUNT )
    {
        EnterCriticalSection(&stderr_lock);
        fprintf(stderr, "SqlStep Timeout on handle: %d (rc = %d)\n", handle, rc);
        LeaveCriticalSection(&stderr_lock);
    }
   if( n > 2 )
   {
      EnterCriticalSection(&stderr_lock);
      fprintf(stderr, "SqlStep tries on handle %d: %d\n", handle, n);
        LeaveCriticalSection(&stderr_lock);
   }
    if( rc == SQLITE_MISUSE )
    {
        EnterCriticalSection(&stderr_lock);
        fprintf(stderr, "sqlite3_step missuse ?? on handle %d\n", handle);
        LeaveCriticalSection(&stderr_lock);
    }
    return(rc);
}


/** This generates an exclusive transaction, used to **/
/** block db from all other writing operations.      **/
int BeginTrans(sqlite3 *DB, int handle)
{
    int rc;
    sqlite3_stmt *bt_stmt;
    bt_stmt = NULL;
    if( DB == NULL )
    {
      fprintf(stderr, "Sql: BeginTrans: No DB connection!\n");
      return(0);
    }
    if( !PrepareSql(DB, &bt_stmt, "BEGIN EXCLUSIVE TRANSACTION;", handle) )
    {
        EnterCriticalSection(&stderr_lock);
        fprintf(stderr, "Begin Transaction error on handle: %d\n", handle);
        LeaveCriticalSection(&stderr_lock);
        return(0);
    }
    rc = StepSql(bt_stmt, handle);
    sqlite3_finalize(bt_stmt);
    if( rc != SQLITE_DONE )
    {
        EnterCriticalSection(&stderr_lock);
        fprintf(stderr, "BeginTrans Timeout/Error on handle:  %d, Errorcode = %d \n", handle, rc);
        LeaveCriticalSection(&stderr_lock);
        return(0);
    }
    return(1);
}



/** This ends the exclusive transactions started with BeginTrans. **/
int EndTrans(sqlite3 *DB, int handle)
{
    int         rc;
    sqlite3_stmt    *bt_stmt;
    if( DB == NULL )
    {
      fprintf(stderr, "Sql: EndTrans: No DB connection!\n");
      return(0);
    }
    if( !PrepareSql(DB, &bt_stmt, "COMMIT;", handle) )
    {
        EnterCriticalSection(&stderr_lock);
        fprintf(stderr, "EndTransaction prepare failed/timeout on handle %d\n", handle);
        LeaveCriticalSection(&stderr_lock);
        return(0);
    }
    rc = StepSql(bt_stmt, handle);
    sqlite3_finalize(bt_stmt);
    if( rc != SQLITE_DONE )
    {
        EnterCriticalSection(&stderr_lock);
        fprintf(stderr, "EndTrans Step Timeout on handle %d (code = %d) \n", handle, rc);
        LeaveCriticalSection(&stderr_lock);
        return(0);
    }
    return(1);
}

/***    Creates tables in test DB **/
int CreateTestDB(char *DBFileName)
{
   sqlite3_stmt *stmt;
   char       sqlStr[1024];
   int     rc;
    rc = sqlite3_open_v2(DBFileName, &MainDB, SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE, NULL);
    if( rc )
    {
        fprintf(stderr, "Can't create database:  %s\n", DBFileName);
        sqlite3_close(MainDB);
        MainDB = NULL;
        return(0);
    }
    fprintf(stderr, "Start creating database...\n");
    /** Change default page size **/
    strcpy(sqlStr,"PRAGMA page_size = 2048;");
    if (SQLITE_OK != sqlite3_prepare_v2(MainDB, sqlStr, -1, &stmt, 0))
    {
        fprintf(stderr, "prepare error: %s", sqlite3_errmsg(MainDB));
    }
    else
    {
       rc = sqlite3_step(stmt);
       if( rc != SQLITE_DONE )
       {
        fprintf(stderr, "step error (%d): %s", rc, sqlite3_errmsg(MainDB));
        return(0);
       }
    }
    sqlite3_finalize(stmt);
    /** Change default cache size **/
    strcpy(sqlStr,"PRAGMA default_cache_size = 10000;");
    if (SQLITE_OK != sqlite3_prepare_v2(MainDB, sqlStr, -1, &stmt, 0))
    {
        fprintf(stderr, "prepare error: %s", sqlite3_errmsg(MainDB));
    }
    else
    {
       rc = sqlite3_step(stmt);
       if( rc != SQLITE_DONE )
       {
        fprintf(stderr, "step error (%d): %s", rc, sqlite3_errmsg(MainDB));
        return(0);
       }
    }
    sqlite3_finalize(stmt);
    /** Create Test Tables: **/
    /** 1. Main Table **/
    strcpy(sqlStr,"CREATE TABLE Main (");
    strcat(sqlStr,"ID INTEGER PRIMARY KEY AUTOINCREMENT,");
    strcat(sqlStr,"Name1 VARCHAR(50) COLLATE NOCASE,");
    strcat(sqlStr,"Name2 VARCHAR(128) COLLATE NOCASE,");
    strcat(sqlStr,"Comment VARCHAR(250),");
    strcat(sqlStr,"Property INTEGER,");
    strcat(sqlStr,"Field INTEGER);");
    if (SQLITE_OK != sqlite3_prepare_v2(MainDB, sqlStr, -1, &stmt, 0)) {
        fprintf(stderr, "prepare error: %s", sqlite3_errmsg(MainDB));
    } else
    {
       rc = sqlite3_step(stmt);
       if( rc != SQLITE_DONE )
       {
        fprintf(stderr, "step error (%d): %s", rc, sqlite3_errmsg(MainDB));
        return(0);
       }
    }
    sqlite3_finalize(stmt);
    /** Data Table **/
    strcpy(sqlStr,"CREATE TABLE Daten (");
    strcat(sqlStr,"ID INTEGER PRIMARY KEY AUTOINCREMENT,");
    strcat(sqlStr,"Typ INTEGER,");
    strcat(sqlStr,"ParentID INTEGER,");
    strcat(sqlStr,"FieldA VARCHAR(16),");
    strcat(sqlStr,"FieldB VARCHAR(500),");
    strcat(sqlStr,"MainID INTEGER,");
    strcat(sqlStr,"PropertyMask INTEGER,");
    strcat(sqlStr,"FieldC VARCHAR(128),");
    strcat(sqlStr,"FieldD VARCHAR(128),");
    strcat(sqlStr,"TypeNumber INTEGER,");
    strcat(sqlStr,"FieldE VARCHAR(128));");
    if (SQLITE_OK != sqlite3_prepare_v2(MainDB, sqlStr, -1, &stmt, 0))
    {
        fprintf(stderr, "prepare error: %s\n", sqlite3_errmsg(MainDB));
    }
   else
    {
       rc = sqlite3_step(stmt);
       if( rc != SQLITE_DONE )
       {
        fprintf(stderr, "step error (%d): %s\n", rc, sqlite3_errmsg(MainDB));
        return(0);
       }
    }
    sqlite3_finalize(stmt);
    /** Create index table on Main ID **/
    if (SQLITE_OK != sqlite3_prepare_v2(MainDB, "CREATE INDEX DatIndexMain ON Daten(MainID);", -1, &stmt, 0))
    {
        fprintf(stderr, "prepare error: %s", sqlite3_errmsg(MainDB));
    }
    else
    {
          rc = sqlite3_step(stmt);
       if( rc != SQLITE_DONE )
       {
        fprintf(stderr, "step error (%d): %s", rc, sqlite3_errmsg(MainDB));
        return(0);
       }
    }
    sqlite3_finalize(stmt);
    /** Type Numbers Table **/
    strcpy(sqlStr,"CREATE TABLE TypeNumbers (");
    strcat(sqlStr,"ID INTEGER PRIMARY KEY AUTOINCREMENT,");
    strcat(sqlStr,"MainID INTEGER,");
    strcat(sqlStr,"Type INTEGER,");
    strcat(sqlStr,"CurrentNumber INTEGER);");
    if (SQLITE_OK != sqlite3_prepare_v2(MainDB, sqlStr, -1, &stmt, 0))
    {
        fprintf(stderr, "prepare error: %s\n", sqlite3_errmsg(MainDB));
    } else
    {
          rc = sqlite3_step(stmt);
       if( rc != SQLITE_DONE )
       {
        fprintf(stderr, "step error (%d): %s\n", rc, sqlite3_errmsg(MainDB));
        return(0);
       }
    }
    sqlite3_finalize(stmt);
    /** Create index table on Dicom Studies ID **/
    if (SQLITE_OK != sqlite3_prepare_v2(MainDB, "CREATE INDEX MainIndexTypeNum ON TypeNumbers(MainID);", -1, &stmt, 0))
    {
        fprintf(stderr, "prepare error: %s", sqlite3_errmsg(MainDB));
    }
   else
    {
       rc = sqlite3_step(stmt);
       if( rc != SQLITE_DONE )
       {
        fprintf(stderr, "step error (%d): %s", rc, sqlite3_errmsg(MainDB));
        return(0);
       }
    }
    sqlite3_finalize(stmt);
    return(1);
}

/**  This fills the test DB with some data **/
#define MAINTBL_ENTRIES    10000
#define DATATBL_ENTRIES    20     /** Number of entries in each main table entry **/
int FillTestDB(sqlite3 *DB)
{
    char            SqlStr[512];
    int                PrimKey;
    sqlite3_stmt    *stmt, *stmt_read;
    int                i;
    BeginTrans(DB, -1);
    stmt = NULL;
    for(i=0; i < MAINTBL_ENTRIES; i++)
    {
        sprintf(SqlStr, "INSERT INTO Main (Name1,Name2,Comment,Property,Field) VALUES('%dSmith','John','Comment Test...', 42, 4711);", i);
        PrepareSql(DB, &stmt, SqlStr, -1);
        StepSql(stmt, -1);
        sqlite3_finalize(stmt);
        stmt = NULL;
    }
    EndTrans(DB, -1);
    BeginTrans(DB, -1);
    /** for each entry in main: generate N data entries **/
    PrepareSql(DB, &stmt_read, "SELECT * FROM Main;", -1);
    while( StepSql(stmt_read, -1) == SQLITE_ROW )
    {
        /** read primary key **/
        PrimKey = sqlite3_column_int(stmt_read, 0);  /** Index 0 --> ID **/
        for(i=0; i < DATATBL_ENTRIES; i++)
        {
            /** Init Type Number table  **/
            sprintf(SqlStr, "INSERT INTO TypeNumbers (MainID,Type,CurrentNumber) VALUES(%d,%d,1);", PrimKey, i);
            PrepareSql(DB, &stmt, SqlStr, -1);
            StepSql(stmt, -1);
            sqlite3_finalize(stmt);
            stmt = NULL;
            sprintf(SqlStr, "INSERT INTO Daten (Typ,ParentID,FieldA,FieldB,MainID,PropertyMask,FieldC,FieldD,TypeNumber,FieldE) VALUES(%d,0,'Ignore','Longer text field',%d,42,'FieldCData','FieldDData', %d, 'Longer Text...');", i, PrimKey, 1);
            PrepareSql(DB, &stmt, SqlStr, -1);
            StepSql(stmt, -1);
            sqlite3_finalize(stmt);
            stmt = NULL;
        }
    }
    sqlite3_finalize(stmt_read);
    EndTrans(DB, -1);
    return(1);
}

/** This is the reader thread. **/
DWORD WINAPI ReadThread( int DBHandle)
{
   sqlite3        *DB;
   sqlite3_stmt    *stmt, *stmtDaten;
   char            SqlStr[512];
   int            Index, PrimKey, DatIndex;
   int            avgIndex, rc, n, mainSteps;
   EnterCriticalSection(&stderr_lock);
   fprintf(stderr, "Read Thread started, using DB Handle: %d\n", DBHandle);
   if( DBHandle < 0 || DBHandle > MAX_OPENDBS )
   {
      LeaveCriticalSection(&stderr_lock);
       fprintf(stderr, "Error: DB Handle out of range! \n");
       return(0);
   }
   LeaveCriticalSection(&stderr_lock);
   DB = DBPool[DBHandle];
   stmt = NULL;
   avgIndex = 0;
   mainSteps = 0;
   while(global_run)
   {
       /** Read randomly a main table entry **/
       Index = (rand() * MAINTBL_ENTRIES) / RAND_MAX;
       sprintf(SqlStr, "SELECT * FROM Main WHERE ID >= %u;", Index);
       if( !PrepareSql(DB, &stmt, SqlStr, DBHandle) )
       {
          /** Timeout or error --> exit **/
          global_sql_error = 1;
          EnterCriticalSection(&stderr_lock);
          fprintf(stderr, "Read Thread PrepareSql on Main Table Error. Handle = %d\n", DBHandle);
          LeaveCriticalSection(&stderr_lock);
          return(0);
       }
      if( (rc = StepSql(stmt, DBHandle)) == SQLITE_ROW )
       {
          PrimKey = sqlite3_column_int(stmt, 0);
          /** Parse the data that refers to this Main Entry **/
          sprintf(SqlStr, "SELECT * FROM Daten WHERE MainID = %u;", PrimKey);
          if( !PrepareSql(DB, &stmtDaten, SqlStr, DBHandle) )
          {
            /** Timeout or error --> exit **/
            global_sql_error = 1;
            EnterCriticalSection(&stderr_lock);
            fprintf(stderr, "Read Thread Prepare from Daten Error. Handle = %d\n", DBHandle);
            LeaveCriticalSection(&stderr_lock);
            return(0);
          }
          /** Parse throu the result set **/
          n = 0;
          avgIndex = 0;
          while( (rc = StepSql(stmtDaten, DBHandle)) == SQLITE_ROW )
          {
            /** Normally display data, process, etc.          **/
            /** Here we just do something silly to avoid that **/
            /** the optimizer probably removes functions.     **/
            DatIndex = sqlite3_column_int(stmtDaten, 0);
            avgIndex += DatIndex;
            n++;
          }
          sqlite3_finalize(stmtDaten);
          if( rc == SQLITE_MISUSE )
          {
            global_sql_error = 1;
            EnterCriticalSection(&stderr_lock);
            fprintf(stderr, "Read Thread Step returns missuse after %d steps on daten table!!  Handle = %d\n", n, DBHandle);
            LeaveCriticalSection(&stderr_lock);
            return 0;
          }
       }
       else
       {
           if( rc == SQLITE_MISUSE )
           {
               global_sql_error = 1;
               EnterCriticalSection(&stderr_lock);
               fprintf(stderr, "Read Thread Step returns missuse on Main Table!!  Handle = %d, steps = %d\n", DBHandle, mainSteps);
               LeaveCriticalSection(&stderr_lock);
               return 0;
           }
       }
       sqlite3_finalize(stmt);
       stmt = NULL;
      Sleep(5); /** ...otherwise the write thread will never get a slot to obtain the exclusive lock...**/
   }
   EnterCriticalSection(&stderr_lock);
   fprintf(stderr, "Read Thread exit.  Handle = %d (x=%d)\n", DBHandle, avgIndex);
   LeaveCriticalSection(&stderr_lock);
   return(0);
}

/** This is the writer thread... **/
DWORD WINAPI WriterThread( int DBHandle)
{
   sqlite3        *DB;
   sqlite3_stmt    *stmt, *stmtDaten, *stmtTypes;
   char            SqlStr[512];
   int            Index, PrimKey, DatIndex;
   int            avgIndex, rc, n, writeMode, bt;
   unsigned long    TypeIndex, NewTypeNum;
   EnterCriticalSection(&stderr_lock);
   fprintf(stderr, "Writer Thread started, using DB Handle: %d\n", DBHandle);
   if( DBHandle < 0 || DBHandle > MAX_OPENDBS )
   {
       fprintf(stderr, "Error: DB Handle out of range! \n");
       return(0);
   }
   LeaveCriticalSection(&stderr_lock);
   DB = DBPool[DBHandle];
   writeMode = 0;
   stmt = NULL;
   stmtTypes = NULL;
   stmtDaten = NULL;
   Sleep(2000);  /** give the reader thread some time to start up ... **/
   while(global_run)
   {
       Sleep(50);
       /** Read randomly an main table entry **/
       Index = (rand() * MAINTBL_ENTRIES) / RAND_MAX;
       sprintf(SqlStr, "SELECT * FROM Main WHERE ID >= %u;", Index);
       if( !PrepareSql(DB, &stmt, SqlStr, DBHandle) )
       {
          /** Timeout or error --> exit **/
          global_sql_error = 1;
          EnterCriticalSection(&stderr_lock);
          fprintf(stderr, "Write Thread Sql Error. Handle = %d\n", DBHandle);
          LeaveCriticalSection(&stderr_lock);
          return(0);
       }
       if( StepSql(stmt, DBHandle) == SQLITE_ROW )
       {
          PrimKey = sqlite3_column_int(stmt, 0);
          /** Either insert a new entry, or just update an existing **/
          if( writeMode == 0)
          {
              /** --> insert new entry **/
           bt = 0;
              while( !BeginTrans(DB, DBHandle) )
           {
            EnterCriticalSection(&stderr_lock);
            fprintf(stderr, "Write Thread: DB is busy! tries = %d handle = %d\n", bt, DBHandle);
                LeaveCriticalSection(&stderr_lock);
            Sleep(128);
            bt++;
           }
              /** get new Type: Read max ID from Types and increase by one **/
              sprintf(SqlStr, "SELECT * FROM TypeNumbers WHERE MainID = %u AND Type = 4;", PrimKey);
              if( !PrepareSql(DB, &stmtTypes, SqlStr, DBHandle) )
              {
                /** Timeout or error --> exit **/
                global_sql_error = 1;
                EnterCriticalSection(&stderr_lock);
                fprintf(stderr, "Write Thread Sql Error. Handle = %d\n", DBHandle);
                LeaveCriticalSection(&stderr_lock);
                EndTrans(DB,DBHandle );
                return(0);
              }
              if( (rc = StepSql(stmtTypes, DBHandle)) == SQLITE_ROW )
              {
                  TypeIndex  = sqlite3_column_int(stmtTypes, 0);
                  NewTypeNum = sqlite3_column_int(stmtTypes, 3) + 1;
                  sqlite3_finalize(stmtTypes);
              }
              else
              {
                  /** Should not happend: No Type found for this Main and Type **/
                  /** --> create a new one                                       **/
                  sprintf(SqlStr, "INSERT INTO TypeNumbers (MainID,Type,CurrentNumber) VALUES(%u, 4, 0);", PrimKey);
                  PrepareSql(DB, &stmtTypes, SqlStr, DBHandle);
                  StepSql(stmtTypes, -1);
                  sqlite3_finalize(stmtTypes);
                  TypeIndex = (unsigned long)sqlite3_last_insert_rowid(DB);
                  NewTypeNum=1;
              }
              /** Update Type Number with the new Type Number **/
              sprintf(SqlStr, "UPDATE TypeNumbers SET CurrentNumber=%d WHERE ID=%u;", NewTypeNum, TypeIndex);
              PrepareSql(DB, &stmtTypes, SqlStr, DBHandle);
              StepSql(stmtTypes, -1);
              sqlite3_finalize(stmtTypes);
              /** Insert New entry in Data table **/
                sprintf(SqlStr, "INSERT INTO Daten (Typ,ParentID,FieldA,FieldB,MainID,PropertyMask,FieldC,FieldD,TypeNumber,FieldE) VALUES(4,0,'Ignore','Longer text field',%d,42,'FieldCData','FieldDData', %d, 'Longer Text...');", PrimKey, NewTypeNum);
              if( !PrepareSql(DB, &stmtDaten, SqlStr, DBHandle) )
              {
                /** Timeout or error --> exit **/
                global_sql_error = 1;
                EnterCriticalSection(&stderr_lock);
                fprintf(stderr, "Write Thread Sql Error. Handle = %d\n", DBHandle);
                LeaveCriticalSection(&stderr_lock);
                EndTrans(DB,DBHandle );
                return(0);
              }
              if( StepSql(stmtDaten, DBHandle) != SQLITE_DONE )
              {
                /** Timeout or error --> exit **/
                global_sql_error = 1;
                EnterCriticalSection(&stderr_lock);
                fprintf(stderr, "Write Thread Sql Error. Handle = %d\n", DBHandle);
                LeaveCriticalSection(&stderr_lock);
                EndTrans(DB,DBHandle );
                return(0);
              }
              sqlite3_finalize(stmtDaten);
              stmtDaten = NULL;
              EndTrans(DB, DBHandle);
          }
          else
          {
              stmtDaten = NULL;
              bt = 0;
              while( !BeginTrans(DB, DBHandle) )
           {
            EnterCriticalSection(&stderr_lock);
            fprintf(stderr, "Write Thread: DB is busy! tries = %d handle = %d\n", bt, DBHandle);
                LeaveCriticalSection(&stderr_lock);
            Sleep(128);
            bt++;
           }
              /** Update all entries that belong to this main table entry **/
              sprintf(SqlStr, "UPDATE Daten SET FieldA='CHanged',FieldB='sqlite3..' WHERE MainID=%u;", PrimKey);
              if( !PrepareSql(DB, &stmtDaten, SqlStr, DBHandle) )
              {
                /** Timeout or error --> exit **/
                global_sql_error = 1;
                EnterCriticalSection(&stderr_lock);
                fprintf(stderr, "Write Thread Sql Error. Handle = %d\n", DBHandle);
                LeaveCriticalSection(&stderr_lock);
                EndTrans(DB,DBHandle );
                return(0);
              }
              if( StepSql(stmtDaten, DBHandle) != SQLITE_DONE )
              {
                /** Timeout or error --> exit **/
                global_sql_error = 1;
                EnterCriticalSection(&stderr_lock);
                fprintf(stderr, "Write Thread Sql Error. Handle = %d\n", DBHandle);
                LeaveCriticalSection(&stderr_lock);
                EndTrans(DB,DBHandle );
                return(0);
              }
              sqlite3_finalize(stmtDaten);
              stmtDaten = NULL;
              EndTrans(DB, DBHandle);
          }
          writeMode = !writeMode;  /** toggle DB write access modes **/
       }
       sqlite3_finalize(stmt);
       stmt = NULL;
   }
   return(0);
}

/** Running tests **/
int  main(int argc, char **argv)
{
  int i;
  long threadid;
  fprintf(stderr, "sqlite thread test program start...\n");
  sqlite3_enable_shared_cache( 1 );
  /** try to open test DB **/
  if( sqlite3_open_v2(DBFILENAME, &MainDB, SQLITE_OPEN_READWRITE|SQLITE_OPEN_FULLMUTEX, NULL) )
  {
    /** failed, --> create new DB **/
    fprintf(stderr, "Creating Test DB: %s\n", DBFILENAME);
    if( !CreateTestDB(DBFILENAME) )
    {
        fprintf(stderr, "Fatal: Unable to create test DB!\n");
        return(0);
    }
    else
    {
        FillTestDB(MainDB);
        sqlite3_close(MainDB);
        /** I exit here, and ask for a restart because this simulates  **/
        /** better the typical situation: to start with an existing DB **/
        fprintf(stderr, "TestDB created: Please restart application.\n");
        return(0);
    }
  }
  fprintf(stderr, "Opened TEST DB: %s \n", DBFILENAME);
  fprintf(stderr, "SQL ThreadSafe: %d\n", sqlite3_threadsafe());
  /*** Open DBs to be used by the threads **/
  memset(DBPool, 0, sizeof(DBPool));
  for(i=0; i < MAX_OPENDBS; i++)
  {
    if( sqlite3_open_v2(DBFILENAME, &DBPool[i], SQLITE_OPEN_READWRITE|SQLITE_OPEN_FULLMUTEX, NULL) )
    {
        fprintf(stderr, "Error: Failed to open DB for thread pool!\n");
        return(0);
    }
  }
  InitializeCriticalSection(&stderr_lock );
  /** Finally: Let's start the test:                                     **/
  /** This will generate a number of reading threads and 1 writer thread **/
  /** The reader threads parse the tables with maximum speed             **/
  /** the write thread will randomly update or insert new entries in the **/
  /** data table.                                                        **/
  global_run = 1;
  /** Start reader threads **/
  for(i=0; i < MAX_OPENDBS-NUM_WRITE_THREADS; i++)
  {
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ReadThread, (LPVOID)i, 0, &threadid);
  }
  /** start writer threads **/
  for(i=MAX_OPENDBS-NUM_WRITE_THREADS; i < MAX_OPENDBS; i++)
  {
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)WriterThread, (LPVOID)i, 0, &threadid);
  }
  /** ok, threads are up and running -- the main process just waits **/
  while( !global_sql_error )
  {
    Sleep(1000);
    if( global_sql_error )
        global_run = 0;
  }
  Sleep(1000);
  /** I should close all DB here, but that is not the focus of the test ... **/
  fprintf(stderr, "SqlTest finished...\n");
  return(0);
}