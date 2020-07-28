#ifndef DNest5_Database_hpp
#define DNest5_Database_hpp

#include <DNest5/Misc.hpp>
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
        void clear_previous();

    public:
        Database();
        ~Database();

        // Friend
        template<typename T>
        friend class Sampler;
};

/* Implementations follow */

Database::Database()
:db(Options::db_filename)
{
    std::cout << "Initialising database." << std::endl;

    pragmas();
    db << "BEGIN;";
    create_tables();
    db << "COMMIT;";
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

    db << "CREATE TABLE IF NOT EXISTS particles\
            (id      INTEGER NOT NULL PRIMARY KEY,\
             sampler INTEGER,\
             logl    REAL NOT NULL,\
             tb      REAL NOT NULL,\
             params  BLOB,\
             FOREIGN KEY (sampler) REFERENCES samplers (id));";

    // Insert the zero particle
    db << "INSERT INTO particles VALUES (0, null, ?, 0.0, null)\
            ON CONFLICT (id) DO NOTHING;"
       << minus_infinity;
}

void Database::clear_previous()
{
    db << "DELETE FROM samplers;";
    db << "DELETE FROM particles WHERE id != 0;";
}

} // namespace

#endif

