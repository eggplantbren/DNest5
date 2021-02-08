import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

def plot_data():
    data = np.loadtxt("road.txt")
    plt.plot(data[:,0], data[:,1], "o", alpha=0.8)


def plot_lines():
    """
    Plot up to 100 lines from the posterior distribution.
    """
    x = np.linspace(0.0, 100.0, 101)
    params = pd.read_csv("../output/posterior.csv")
    num = min(params.shape[0], 100)
    for i in range(num):
        row = params.iloc[i, :]
        plt.plot(x, row["m"]*x + row["b"], "k", alpha=0.1)
    

def tweaks():
    plt.xlabel("Age (years)")
    plt.ylabel("Distance (feet)")
    plt.axis([0.0, 100.0, 0.0, 800.0])


if __name__ == "__main__":
    plot_lines()
    plot_data()
    tweaks()
    plt.show()

