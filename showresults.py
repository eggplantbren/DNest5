import apsw
import matplotlib.pyplot as plt
import numpy as np
import subprocess

def figure_1(db):
    """ The equivalent of DNest4's Figure 1. """

    print("Creating Figure 1: ", end="", flush=True)
    plt.figure()
    ids = []
    levels = []
    for row in db.execute("SELECT id, level FROM particles;"):
        ids.append(row[0])
        levels.append(row[1])

    plt.plot(ids, levels, alpha=0.6)
    plt.xlabel("Iteration")
    plt.ylabel("Level")
    plt.savefig("output/figure1.pdf")
    print("done.", flush=True)


def figure_2(db):
    """ The equivalent of DNest4's Figure 2. """

    print("Creating Figure 2: ", end="", flush=True)

    levels, logxs, acceptance_rates = [], [], []
    for row in db.execute("SELECT id, logx, accepts, tries FROM levels;"):
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
    plt.savefig("output/figure2.pdf")
    print("done.", flush=True)

def figure_3(db):
    """ The equivalent of DNest4's Figure 3. """

    print("Creating Figure 3: ", end="", flush=True)

    # Connect to posterior.db as well
    db.execute("ATTACH DATABASE 'output/posterior.db' AS posterior;")

    logxs = []
    logls = []
    logps = []
    for row in db.execute("SELECT logx, logl, logp FROM posterior.particles\
                            INNER JOIN main.particles\
                            ON posterior.particles.id = main.particles.id\
                            ORDER BY logl, tb;"):
        logxs.append(row[0])
        logls.append(row[1])
        logps.append(row[2])
    logxs = np.array(logxs)
    logls = np.array(logls)
    logps = np.array(logps)

    db.execute("DETACH DATABASE posterior;")

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
    plt.plot(logxs, np.exp(logps - logps.max()), alpha=0.6)
    plt.xlabel("log(X)")
    plt.ylabel("Posterior Weight (Relative)")
    plt.xlim(left=logxs.min()-0.5)
    plt.xlim(right=0.5)
    plt.ylim([0.0, 1.05])
    plt.gcf().align_ylabels()
    plt.savefig("output/figure3.pdf")
    print("done.", flush=True)



if __name__ == "__main__":
    subprocess.run("./postprocess")

    conn = apsw.Connection("output/dnest5.db", flags=apsw.SQLITE_OPEN_READONLY)
    db = conn.cursor()

    print("------------------------")
    print("Creating figures")
    print("------------------------", flush=True)
    figure_1(db)
    figure_2(db)
    figure_3(db)
    conn.close()
    print("------------------------", flush=True)
    plt.show()

