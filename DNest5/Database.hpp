#ifndef DNest5_Database_hpp
#define DNest5_Database_hpp

#include <deque>
#include <DNest5/Misc.hpp>
#include <DNest5/Options.hpp>
#include <DNest5/RNG.hpp>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <sqlite_modern_cpp/hdr/sqlite_modern_cpp.h>
#include <sstream>
#include <string>

namespace DNest5
{

// A postprocessing function
template<typename T>
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
    clear_previous();
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
            (id                     INTEGER NOT NULL PRIMARY KEY\
                                    CHECK (id = 1),\
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
             FOREIGN KEY (level) REFERENCES levels (id));";

    db << "CREATE TABLE IF NOT EXISTS levels\
            (id      INTEGER NOT NULL PRIMARY KEY,\
             logx    REAL NOT NULL,\
             logl    REAL NOT NULL,\
             tb      REAL NOT NULL,\
             exceeds INTEGER NOT NULL DEFAULT 0,\
             visits  INTEGER NOT NULL DEFAULT 0,\
             accepts INTEGER NOT NULL DEFAULT 0,\
             tries   INTEGER NOT NULL DEFAULT 0);";
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
                   (SELECT id FROM levels l\
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

template<typename T>
void postprocess()
{
    std::cout << "Begin postprocessing\n";
    std::cout << "--------------------\n" << std::endl;

    // A read-only database connection
    sqlite::database reader(Options::db_filename,
                            sqlite::sqlite_config { sqlite::OpenFlags::READONLY,
                                                    nullptr,
                                                    sqlite::Encoding::ANY });

    // A writing connection to posterior.db
    sqlite::database db("output/posterior.db");
    db << "PRAGMA SYNCHRONOUS = 0;";
    db << "PRAGMA JOURNAL_MODE = WAL;";
    db << "BEGIN;";
    db << "CREATE TABLE IF NOT EXISTS particles\
            (id   INTEGER NOT NULL PRIMARY KEY,\
             logx REAL NOT NULL,\
             logm REAL NOT NULL,\
             logp REAL);";
    db << "DELETE FROM particles;";
    db << "COMMIT;";

    // Get maximum particle ID and number of levelsand use it for truncation
    int max_particle_id, num_levels;
    reader << "SELECT MAX(id) FROM particles;" >> max_particle_id;
    reader << "SELECT COUNT(id) FROM levels;" >> num_levels;

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

    // Compute log-masses of the particles
    int rank = 0;
    int old_level = 0;
    // Parallel vectors of particle information
    std::vector<int> particle_ids; std::vector<double> logms, logls, logxs;
    std::deque<bool> is_full;
    reader << "SELECT p.id, llp.level, p.logl, p.params IS NOT NULL\
                            FROM particles p INNER JOIN\
                            levels_leq_particles llp ON p.id=llp.particle\
                            WHERE p.id <= ? AND llp.level < ?\
                            ORDER BY llp.level, logl, tb;"
       << max_particle_id << num_levels >>
        [&](int particle_id, int level, double logl, bool full)
        {
            if(level != old_level)
            {
                rank = 0;
                old_level = level;
            }

            particle_ids.push_back(particle_id);
            logms.emplace_back(level_logms[level]
                                    - log(level_num_particles[level]));
            logls.push_back(logl);

            // X_particle = X_level - (rank+0.5)*M_particles
            double logx = logdiffexp(level_logxs[level],
                                     log(rank + 0.5) + logms.back());
            logxs.emplace_back(logx);
            is_full.push_back(full);
            ++rank;

            db << "INSERT INTO particles (id, logx, logm) VALUES (?, ?, ?);"
               << particle_id << logx << logms.back();

            if(int(logxs.size()) == max_particle_id ||
                int(logxs.size()) % 1000 == 0)
            {
                std::cout << "\rProcessed " << logxs.size() << " particles.";
                std::cout << std::flush;
            }
        };
    std::cout << std::endl;

    // Prior times likelihood, posterior, information, etc
    std::vector<double> loghs(logms.size()), logps(logms.size());
    for(int i=0; i<int(logms.size()); ++i)
        loghs[i] = logms[i] + logls[i];
    double logz = logsumexp(loghs);
    for(int i=0; i<int(logms.size()); ++i)
    {
        logps[i] = loghs[i] - logz;
        db << "UPDATE particles SET logp = ? WHERE id = ?;"
           << logps[i] << particle_ids[i];
    }
    double H = 0.0;
    for(int i=0; i<int(loghs.size()); ++i)
    {
        double p = exp(logps[i]);
        H +=  p*(logps[i] - logms[i]);
    }

    // Log-probs of full particles
    std::vector<double> full_logps;
    std::vector<int> full_pids;
    for(int i=0; i<int(loghs.size()); ++i)
    {
        if(is_full[i])
        {
            full_logps.push_back(logps[i]);
            full_pids.push_back(particle_ids[i]);
        }
    }
    double tot = logsumexp(full_logps);
    double ess = 0.0;
    for(double& lp: full_logps)
    {
        lp -= tot;
        ess += -exp(lp)*lp;
    }
    ess = exp(ess);
    double max_logp_full = *max_element(full_logps.begin(), full_logps.end());

    // Save results
    std::stringstream sout;
    sout << std::setprecision(Options::stdout_precision);
    sout << "# Results file written by DNest5\n";
    sout << "---\n\n";
    sout << "# Natural log of the marginal likelihood\n";
    sout << "logz: " << logz << "\n\n";
    sout << "# Prior-to-posterior Kullback-Leibler divergence, in nats\n";
    sout << "info: " << H << "\n\n";
    sout << "# Effective posterior sample size (full particles)\n";
    sout << "ess: " << int(ess) + 1 << "\n\n";

    std::fstream fout("output/results.yaml", std::ios::out);
    fout << sout.str();
    fout.close();

    // And to stdout
    std::cout << sout.str();

    // Get posterior samples and output as CSV
    RNG rng; rng.set_seed(0);
    int count = 0;
    fout.open("output/posterior.csv", std::ios::out);
    fout << std::setprecision(Options::stdout_precision);
    fout << T::parameter_names.csv_header() << std::endl;
    T t(rng);
    db << "BEGIN;";
    while(count < int(ess) + 1)
    {
        int k = rng.rand_int(full_pids.size());
        if(rng.rand() <= exp(full_logps[k] - max_logp_full))
        {
            std::vector<char> blob;
            reader << "SELECT params FROM particles WHERE id = ?;"
                   << full_pids[k] >> blob;
            t.from_blob(blob);
            fout << t.to_string() << std::endl;
            ++count;
        }
    };
    db << "COMMIT;";
    db << "VACUUM;";
    db << "PRAGMA main.WAL_CHECKPOINT(TRUNCATE);";

    std::cout << "--------------------\n";
    std::cout << "Finished." << std::endl;
}


} // namespace

#endif

