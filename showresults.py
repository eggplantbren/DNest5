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


if __name__ == "__main__":
    conn = apsw.Connection("output/dnest5.db", flags=apsw.SQLITE_OPEN_READONLY)
    db = conn.cursor()

    figure_1(db)
    figure_2(db)
    figure_3(particles)
    plt.show()

    conn.close()

