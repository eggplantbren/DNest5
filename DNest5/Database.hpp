#ifndef DNest5_Database_hpp
#define DNest5_Database_hpp

#include <DNest5/Misc.hpp>
#include <DNest5/Options.hpp>
#include <iostream>
#include <sqlite_modern_cpp/hdr/sqlite_modern_cpp.h>

namespace DNest5
{

// A postprocessing function
void postprocess();


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

void postprocess()
{
    // A read-only database connection
    sqlite::database reader(Options::db_filename,
                            sqlite::sqlite_config { sqlite::OpenFlags::READONLY,
                                                    nullptr,
                                                    sqlite::Encoding::ANY });

    // Get maximum particle ID and use it for truncation
    int max_particle_id;
    reader << "SELECT MAX(id) FROM particles;" >> max_particle_id;

    // Load levels into a vector
    std::vector<double> level_logxs, level_num_particles;
    reader << "SELECT logx, num_particles\
               FROM levels l INNER JOIN particles_per_level ppl\
               ON l.id = ppl.level;" >>
        [&](double logx, int n)
        {
            level_logxs.push_back(logx);
            level_num_particles.push_back(n);
        };

    // Compute log-mass between levels
    std::vector<double> level_logms;
    // m_i = X_i - X_{i+1}
    // log m_i = log(exp(logx_i) - exp(logx_{i+1}))
    for(int i=0; i<int(level_logxs.size())-1; ++i)
        level_logms.push_back(logdiffexp(level_logxs[i], level_logxs[i+1]));
    level_logms.push_back(logdiffexp(level_logxs.back(), minus_infinity));
/*
    # Level num_particles
    num_particles = []
    for row in db.execute("SELECT num_particles\
                           FROM particles_per_level;"):
        num_particles.append(row[0])

    # Compute the logms
    particles = dict()
    old_level = 0
    rank = 0
    for row in db.execute("SELECT p.id, llp.level, p.logl, p.params IS NOT NULL\
                            FROM particles p INNER JOIN\
                            levels_leq_particles llp ON p.id=llp.particle\
                            WHERE p.id <= ? AND llp.level < ?\
                            ORDER BY llp.level, logl, tb;",
                            (max_particle_id, min(len(logms), len(logxs), len(num_particles)))):
        particle_id, level, logl, full = row
        if level != old_level:
            rank = 0
            old_level = level

        particles[particle_id] = dict()
        particles[particle_id]["logm"] = logms[level]\
                                            - np.log(num_particles[level])
        particles[particle_id]["logl"] = logl
        # X_particle = X_level - (rank+0.5)*M_particles
        logx = logdiffexp(logxs[level], np.log(rank + 0.5) + particles[particle_id]["logm"])
        particles[particle_id]["logx"] = logx
        particles[particle_id]["full"] = bool(full)
        rank += 1

    results = dict(logz=logsumexp([particles[pid]["logm"]\
                                    + particles[pid]["logl"]\
                                  for pid in particles]))

    # Parallel lists
    fp_ids = []
    fp_logps = []

    # Posterior weights and information
    results["h"] = 0.0
    for particle_id in particles:
        particle = particles[particle_id]
        particle["logp"] = particle["logm"] + particle["logl"] - results["logz"]
        if particle["full"]:
            fp_ids.append(particle_id)
            fp_logps.append(particle["logp"])
        results["h"] += np.exp(particle["logp"])*(particle["logl"] - results["logz"])
    fp_ids = np.array(fp_ids)
    fp_logps = np.array(fp_logps)

    # Normalise
    fp_logps -= logsumexp(fp_logps)
    ess = np.exp(-np.sum(np.exp(fp_logps)*fp_logps))
    top = np.max(fp_logps)
    results["full_particle_ess"] = ess

    # Generate posterior
    conn2 = apsw.Connection("posterior.db")
    db2 = conn2.cursor()
    db2.execute("PRAGMA JOURNAL_MODE=OFF;")
    db2.execute("PRAGMA SYNCHRONOUS=0;")
    db2.execute("BEGIN;")
    db2.execute("CREATE TABLE IF NOT EXISTS particles\
                    (id     INTEGER NOT NULL PRIMARY KEY,\
                     params BLOB NOT NULL);")
    db2.execute("DELETE FROM particles;")
    db2.execute("COMMIT;")
    db2.execute("BEGIN;")
    rng.seed(rng_seed)
    saved = 0
    while saved < int(ess) + 1:
        k = rng.randint(fp_ids.size)
        if rng.rand() <= np.exp(fp_logps[k] - top):
            blob = db.execute("SELECT params FROM particles WHERE id = ?;",
                              (int(fp_ids[k]), )).fetchone()[0]
            db2.execute("INSERT INTO particles (params) VALUES (?);", (blob, ))
            saved += 1
    db2.execute("COMMIT;")
    conn2.close()
    print("Wrote posterior samples to posterior.db.")
*/

}


} // namespace

#endif

