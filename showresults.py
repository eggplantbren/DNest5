import apsw
import matplotlib.pyplot as plt
import numpy as np

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
        acceptance_rates.append(accepts/tries)

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


def particle_logms(db):
    """
    Estimate particle log-prior-passes
    """
    logxs = db.execute("SELECT logx FROM levels;").fetchall()
    logxs = [logx[0] for logx in logxs]

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
    for row in db.execute("SELECT p.id, llp.level, p.logl FROM particles p INNER JOIN\
                            levels_leq_particles llp ON p.id=llp.particle;"):
        particle_id, level, logl = row
        particles[particle_id] = dict()
        particles[particle_id]["logm"] = logms[level]\
                                            - np.log(num_particles[level])
        particles[particle_id]["logl"] = logl

    # Now compute logxs as well
    # X_{i+1} = X_i - m_i
    # logX_{i+1} = log(exp(logX_i) - exp(logm_i))
#    logx = 0.0
#    for row in db.execute("SELECT id, logl FROM particles ORDER BY logl, tb;"):
#        particle_id, logl = row
#        print(logx, particles[particle_id]["logm"])
#        logx = logdiffexp(logx, particles[particle_id]["logm"])
##        particles[particle_id]["logx"] = logx
#    plt.plot([particles[p]["logm"] for p in particles])
#    print(logsumexp([particles[p]["logm"] for p in particles]))
#    plt.show()

    result = dict(logz=logsumexp(\
                            [particles[pid]["logm"]\
                             + particles[pid]["logl"] for pid in particles]))
    return result

if __name__ == "__main__":
    conn = apsw.Connection(".db/dnest5.db", flags=apsw.SQLITE_OPEN_READONLY)
    db = conn.cursor()

    results = particle_logms(db)
    print(results)

    figure_1(db)
    figure_2(db)
    plt.show()


    conn.close()

