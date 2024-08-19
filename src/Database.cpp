#include "Database.h"

namespace DNest5
{

Database::Database()
:db("output/dnest5.db")
{
    std::cout << "Initialising database." << std::endl;

    pragmas();
    db << "BEGIN;";
    create_tables();
    create_indexes();
    create_views();
    db << "COMMIT;";
    db << "VACUUM;";
}

void Database::pragmas()
{
    db << "PRAGMA SYNCHRONOUS = 0;";
    db << "PRAGMA JOURNAL_MODE = WAL;";
}

void Database::create_tables()
{
    db <<
"CREATE TABLE IF NOT EXISTS samplers\n\
    (id                 INTEGER NOT NULL PRIMARY KEY\n\
                        CHECK (id = 1),\n\
     num_particles      INTEGER NOT NULL,\n\
     num_threads        INTEGER NOT NULL,\n\
     new_level_interval INTEGER NOT NULL,\n\
     save_interval      INTEGER NOT NULL,\n\
     thin               REAL NOT NULL,\n\
     max_num_levels     INTEGER,\n\
     lambda             REAL NOT NULL,\n\
     beta               REAL NOT NULL,\n\
     max_num_saves      INTEGER NOT NULL);";

    db <<
"CREATE TABLE IF NOT EXISTS rngs\n\
    (seed    INTEGER NOT NULL PRIMARY KEY,\n\
     sampler INTEGER NOT NULL,\n\
     FOREIGN KEY (sampler) REFERENCES samplers (id));";

    db <<
"CREATE TABLE IF NOT EXISTS particles\n\
    (id      INTEGER NOT NULL PRIMARY KEY,\n\
     sampler INTEGER NOT NULL,\n\
     level   INTEGER NOT NULL,\n\
     params  TEXT,\n\
     logl    REAL NOT NULL,\n\
     tb      REAL NOT NULL,\n\
     FOREIGN KEY (sampler) REFERENCES samplers (id),\n\
     FOREIGN KEY (level) REFERENCES levels (id));";

    db <<
"CREATE TABLE IF NOT EXISTS levels\n\
    (id      INTEGER NOT NULL PRIMARY KEY,\n\
    logx    REAL NOT NULL,\n\
    logl    REAL NOT NULL,\n\
    tb      REAL NOT NULL,\n\
    exceeds INTEGER NOT NULL DEFAULT 0,\n\
    visits  INTEGER NOT NULL DEFAULT 0,\n\
    accepts INTEGER NOT NULL DEFAULT 0,\n\
    tries   INTEGER NOT NULL DEFAULT 0);";
}

void Database::create_indexes()
{
    db <<
"CREATE INDEX IF NOT EXISTS particle_logl_tb_idx\n\
ON particles (logl, tb);";

    db <<
"CREATE INDEX IF NOT EXISTS level_logl_tb_idx\n\
ON levels (logl, tb);";
}

void Database::create_views()
{
    db <<
"CREATE VIEW IF NOT EXISTS levels_leq_particles AS\n\
SELECT p.id particle,\n\
   (SELECT id FROM levels l\n\
        WHERE (l.logl, l.tb) <= (p.logl, p.tb)\n\
        ORDER BY l.logl DESC, l.tb DESC\n\
        LIMIT 1) AS level\n\
    FROM particles p;";

    db <<
"CREATE VIEW IF NOT EXISTS particles_per_level AS\n\
    SELECT level, COUNT(*) num_particles\n\
    FROM levels_leq_particles\n\
    GROUP BY level;";
}

int Database::num_full_particles(int sampler_id)
{
    int num;
    db << "SELECT COUNT(params) FROM particles WHERE sampler = ?;"
       << sampler_id >> num;
    return num;
}

} // namespace

