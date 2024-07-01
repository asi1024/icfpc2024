#%%

import os
import matplotlib.pyplot as plt
from pathlib import Path

import numpy as np


# file = "spaceship_data/08.txt"
spaceship_data_dir = "spaceship_data"
spaceship_fig_dir = "spaceship_fig"
files = os.listdir("spaceship_data")

for file in files:
    pos = np.loadtxt(f"{spaceship_data_dir}/{file}", dtype=np.int64)
    order_path = Path("spaceship/ats5515/order/" + file)
    linestyle = " "
    if order_path.exists():
        linestyle = "-"
        with order_path.open() as f:
            indices = np.array(list(map(int, f.readlines())))
            pos = pos[indices]
    fig = plt.figure()
    ax = fig.add_subplot(111, aspect="equal")
    ax.plot(pos[:, 0], pos[:, 1], ".", linestyle=linestyle)
    ax.set_title(file)
    fig.savefig(f"{spaceship_fig_dir}/{file}.png")

# %%
