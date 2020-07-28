#ifndef DNest5_Database_hpp
#define DNest5_Database_hpp

#include <DNest5/Options.hpp>
#include <iostream>
#include <sqlite_modern_cpp/hdr/sqlite_modern_cpp.h>

namespace DNest5
{


/* Create and manage the output database */
class Database
{
    private:
        sqlite::database db;
        void pragmas();
        void create_tables();

    public:
        Database();
        ~Database();

};

/* Implementations follow */

Database::Database()
:db(Options::db_filename)
{
    std::cout << "Initialising database..." << std::flush;

    pragmas();
    db << "BEGIN;";
    create_tables();
    db << "COMMIT;";

    std::cout << "done." << std::endl;
}

Database::~Database()
{
    db << "PRAGMA main.WAL_CHECKPOINT(TRUNCATE);";
}

void Database::pragmas()
{
    db << "PRAGMA SYNCHRONOUS = 0;";
    db << "PRAGMA JOURNAL_MODE = WAL;";
}

void Database::create_tables()
{
    db << "CREATE TABLE IF NOT EXISTS samplers\
            (id                     INTEGER NOT NULL PRIMARY KEY,\
             new_level_interval     INTEGER NOT NULL,\
             save_interval          INTEGER NOT NULL,\
             metadata_save_interval INTEGER NOT NULL,\
             max_num_levels         INTEGER,\
             lambda                 REAL NOT NULL,\
             beta                   REAL NOT NULL,\
             max_num_saves          INTEGER NOT NULL,\
             rng_seed               INTEGER NOT NULL);";
}


} // namespace

#endif

