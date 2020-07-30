import apsw
import matplotlib.pyplot as plt
import numpy as np

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
    plt.ylim(bottom=-0.05, top=1.05)
    plt.gcf().align_ylabels()





if __name__ == "__main__":
    conn = apsw.Connection(".db/dnest5.db", flags=apsw.SQLITE_OPEN_READONLY)
    db = conn.cursor()

    figure_1(db)
    figure_2(db)
    plt.show()


    conn.close()

