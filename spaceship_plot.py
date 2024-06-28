#%%

import os
import matplotlib.pyplot as plt

import numpy as np


# file = "spaceship_data/08.txt"
spaceship_data_dir = "spaceship_data"
spaceship_fig_dir = "spaceship_fig"
files = os.listdir("spaceship_data")

for file in files:
    pos = np.loadtxt(f"{spaceship_data_dir}/{file}", dtype=np.int64)
    fig = plt.figure()
    ax = fig.add_subplot(111, aspect="equal")
    ax.plot(pos[:, 0], pos[:, 1], ".")
    ax.set_title(file)
    fig.savefig(f"{spaceship_fig_dir}/{file}.png")

# %%
