import apsw
import json
import matplotlib.pyplot as plt
import numpy as np
import numpy.random as rng

def logsumexp(ls):
    top = np.max(ls)
    return top + np.log(np.sum(np.exp(ls - top)))

def logdiffexp(x, y):
    ratio = np.exp(y - x)
    return x + np.log(1.0 - ratio)

def figure_1(db):
    """ The equivalent of DNest4's Figure 1. """

    plt.figure()
    ids = []
    levels = []
    for row in db.execute("SELECT id, level FROM particles;"):
        ids.append(row[0])
        levels.append(row[1])

    plt.plot(ids, levels, alpha=0.6)
    plt.xlabel("Iteration")
    plt.ylabel("Level")


def figure_2(db):
    """ The equivalent of DNest4's Figure 2. """

    levels, logxs, acceptance_rates = [], [], []
    for row in db.execute("SELECT level, logx, accepts, tries FROM levels;"):
        level, logx, accepts, tries = row
        levels.append(level)
        logxs.append(logx)
        acceptance_rates.append((accepts+0.5)/(tries+1))

    plt.figure()
    plt.subplot(2, 1, 1)
    diff = np.diff(logxs)
    plt.plot(np.array(levels[0:-1]) + 0.5, diff, "o-", alpha=0.6)
    plt.axhline(-1.0, linestyle="--", color="k")
    plt.xlim(left=-0.5, right=np.max(levels)+0.5)
    plt.ylim(bottom=1.05*np.min(diff), top=0.05)
    plt.ylabel("Compression")

    plt.subplot(2, 1, 2)
    plt.plot(levels, acceptance_rates, "o-", alpha=0.6)
    plt.xlabel("Level")
    plt.ylabel("Acceptance Rate")
    plt.xlim(left=-0.5, right=np.max(levels)+0.5)
    plt.ylim(bottom=0.0, top=1.0)
    plt.gcf().align_ylabels()

def figure_3(particles):
    """ The equivalent of DNest4's Figure 3. """

    logxs = np.array([particles[p]["logx"] for p in particles])
    logls = np.array([particles[p]["logl"] for p in particles])
    logps = np.array([particles[p]["logp"] for p in particles])
    ps = np.exp(logps - np.max(logps))

    plt.figure()
    plt.subplot(2, 1, 1)
    plt.plot(logxs, logls, alpha=0.6)
    plt.ylabel("log(L)")
    plt.xlim(left=logxs.min()-0.5)
    plt.xlim(right=0.5)
    s = np.sort(logls)
    bottom=s[int(0.05*len(s))]
    top=s[-1] + 0.05*(s[-1] - bottom)
    plt.ylim([bottom, top])

    plt.subplot(2, 1, 2)
    plt.plot(logxs, ps, alpha=0.6)
    plt.xlabel("log(X)")
    plt.ylabel("Posterior Weight")
    plt.xlim(left=logxs.min()-0.5)
    plt.xlim(right=0.5)
    plt.ylim([0.0, 1.05])
    plt.gcf().align_ylabels()


def postprocess(db, rng_seed=0):
    """
    Estimate particle log-prior-passes
    """
    # Get max particle ID and trucate with it
    max_particle_id = db.execute("SELECT MAX(id) FROM particles;").fetchone()[0]
    logxs = []
    for row in db.execute("SELECT logx FROM levels;"):
        logxs.append(row[0])

    # Level logms
    logms = []
    # m_i = X_i - X_{i+1}
    # log m_i = log(exp(logx_i) - exp(logx_{i+1}))
    for i in range(len(logxs) - 1):
        logms.append(logdiffexp(logxs[i], logxs[i+1]))
    logms.append(logdiffexp(logxs[-1], -np.inf))

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

    return particles, results

if __name__ == "__main__":
    conn = apsw.Connection(".db/dnest5.db", flags=apsw.SQLITE_OPEN_READONLY)
    db = conn.cursor()

    particles, results = postprocess(db)
    print(json.dumps(results, indent=4))

    figure_1(db)
    figure_2(db)
    figure_3(particles)
    plt.show()

    conn.close()

