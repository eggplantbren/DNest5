#ifndef DNest5_Database_h
#define DNest5_Database_h

#include <deque>
#include "CommandLineOptions.h"
#include "Options.h"
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <sqlite_modern_cpp/hdr/sqlite_modern_cpp.h>
#include <sstream>
#include <string>
#include <Tools/Misc.hpp>
#include <Tools/RNG.hpp>

namespace DNest5
{

// Using declarations
using Tools::logdiffexp;
using Tools::logsumexp;
using Tools::minus_infinity;
using Tools::RNG;

// A postprocessing function
template<typename T>
inline void postprocess(const CommandLineOptions& options);

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

        int num_full_particles(int sampler_id);

        // Friend
        template<typename T>
        friend class Sampler;
};


/* Template implementations */

template<typename T>
inline void postprocess(const CommandLineOptions& options)
{
    std::cout << "--------------------\n";
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
    db <<
"CREATE TABLE IF NOT EXISTS particles\n\
    (id   INTEGER NOT NULL PRIMARY KEY,\n\
     logx REAL NOT NULL,\n\
     logm REAL NOT NULL,\n\
     logp REAL);";
    db << "DELETE FROM particles;";
    db << "COMMIT;";
    db << "VACUUM;";

    // Get maximum particle ID and number of levels and use it for truncation
    int max_particle_id, max_level_id;
    reader << "BEGIN;";
    reader << "SELECT MAX(id) FROM particles;" >> max_particle_id;
    reader << "SELECT MAX(id) FROM levels;" >> max_level_id;

    // Load levels into a vector
    std::vector<double> level_logxs, level_num_particles;
    reader << "SELECT logx, num_particles\
               FROM levels l LEFT JOIN particles_per_level ppl\
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
                            WHERE p.id <= ? AND llp.level <= ?\
                            ORDER BY llp.level, logl, tb;"
       << max_particle_id << max_level_id >>
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
            logls.push_back(logl / options.get_temperature());

            // X_particle = X_level - (rank+0.5)*N_level * M_level
            double logx = logdiffexp(level_logxs[level],
                                     log((rank + 0.5)/level_num_particles[level])
                                         + level_logms[level]);
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
    std::cout << '\n' << std::endl;

    std::cout << "Computing results..." << std::flush;

    // For ABC, replace log likelihoods
    if(options.get_abc())
    {
        for(size_t i=0; i<logls.size(); ++i)
        {
            if(i < options.get_abc_fraction()*logls.size())
                logls[i] = minus_infinity;
            else
                logls[i] = -logms[i];
        }
    }

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
        if(p > 0.0)
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
        double p = exp(lp);
        if(p > 0.0)
            ess += -p*lp;
    }
    ess = exp(ess);
    double max_logp_full = *max_element(full_logps.begin(), full_logps.end());
    std::cout << "done.\n" << std::endl;
    std::cout << "--------------------------------" << std::endl;

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
    sout << "ess: " << int(ess) + 1 << std::endl;

    std::fstream fout("output/results.yaml", std::ios::out);
    fout << sout.str();
    fout.close();

    // And to stdout
    std::cout << sout.str() << std::endl;

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
    reader << "COMMIT;";
}


} // namespace

#endif

