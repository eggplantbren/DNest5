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
        void create_indexes();
        void create_views();
        void clear_previous();

    public:
        Database();
        ~Database();

        int num_full_particles(int sampler_id);

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
    create_indexes();
    create_views();
    db << "COMMIT;";
}

Database::~Database()
{
    std::cout << "Tidying up database..." << std::flush;
    db << "VACUUM;";
    db << "PRAGMA main.WAL_CHECKPOINT(TRUNCATE);";
    std::cout << "done." << std::endl;
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
             num_particles          INTEGER NOT NULL,\
             num_threads            INTEGER NOT NULL,\
             new_level_interval     INTEGER NOT NULL,\
             save_interval          INTEGER NOT NULL,\
             thin                   REAL NOT NULL,\
             max_num_levels         INTEGER,\
             lambda                 REAL NOT NULL,\
             beta                   REAL NOT NULL,\
             max_num_saves          INTEGER NOT NULL);";

    db << "CREATE TABLE IF NOT EXISTS rngs\
            (seed    INTEGER NOT NULL PRIMARY KEY,\
             sampler INTEGER NOT NULL,\
             FOREIGN KEY (sampler) REFERENCES samplers (id));";

    db << "CREATE TABLE IF NOT EXISTS particles\
            (id      INTEGER NOT NULL PRIMARY KEY,\
             sampler INTEGER NOT NULL,\
             level   INTEGER NOT NULL,\
             params  BLOB,\
             logl    REAL NOT NULL,\
             tb      REAL NOT NULL,\
             FOREIGN KEY (sampler) REFERENCES samplers (id),\
             FOREIGN KEY (sampler, level) REFERENCES levels (sampler, level));";

    db << "CREATE TABLE IF NOT EXISTS levels\
            (sampler INTEGER NOT NULL,\
             level   INTEGER NOT NULL,\
             logx    REAL NOT NULL,\
             logl    REAL NOT NULL,\
             tb      REAL NOT NULL,\
             exceeds INTEGER NOT NULL DEFAULT 0,\
             visits  INTEGER NOT NULL DEFAULT 0,\
             accepts INTEGER NOT NULL DEFAULT 0,\
             tries   INTEGER NOT NULL DEFAULT 0,\
             PRIMARY KEY (sampler, level),\
             FOREIGN KEY (sampler) REFERENCES samplers (id));";
}

void Database::create_indexes()
{
    db << "CREATE INDEX IF NOT EXISTS particle_logl_tb_idx\
            ON particles (logl, tb);";

    db << "CREATE INDEX IF NOT EXISTS level_logl_tb_idx\
            ON levels (logl, tb);";
}

void Database::create_views()
{
    db << "CREATE VIEW IF NOT EXISTS levels_leq_particles AS\
            SELECT p.id particle,\
                   (SELECT level FROM levels l\
                        WHERE (l.logl, l.tb) <= (p.logl, p.tb)\
                        ORDER BY l.logl DESC, l.tb DESC\
                        LIMIT 1) AS level\
            FROM particles p;";

    db << "CREATE VIEW IF NOT EXISTS particles_per_level AS\
            SELECT level, COUNT(*) num_particles\
            FROM levels_leq_particles\
            GROUP BY level;";
}

void Database::clear_previous()
{
    db << "DELETE FROM samplers;";
    db << "DELETE FROM particles;";
    db << "DELETE FROM levels;";
    db << "DELETE FROM rngs;";
}

int Database::num_full_particles(int sampler_id)
{
    int num;
    db << "SELECT COUNT(params) FROM particles WHERE sampler = ?;"
       << sampler_id >> num;
    return num;
}

} // namespace

#endif

