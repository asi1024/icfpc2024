import os
import subprocess

import numpy as np

spaceship_data_dir = "spaceship_data"
files = os.listdir("spaceship_data")
for file in files:
    if file!="22.txt":
        continue
    pos = np.loadtxt(f"{spaceship_data_dir}/{file}", dtype=np.int64)
    print(len(pos))
    s = f"NAME : {file}\n" \
    f"TYPE : TSP\n" \
    f"DIMENSION : {len(pos) + 1}\n" \
    f"EDGE_WEIGHT_TYPE : EUC_2D\n" \
    f"NODE_COORD_SECTION\n" \
    f"1 0 0\n"
    for i in range(len(pos)):
        s += f"{i + 2} {pos[i, 0]} {pos[i, 1]}\n"
    s += "EOF\n"

    save_path = f"spaceship/ats5515/tsp_files/{file}.tsp"
    with open(save_path, mode='w') as f:
        f.write(s)
    command = f'./concorde/LINKERN/linkern -Q -t 60 -o spaceship/ats5515/tsp_solver_order/{file} spaceship/ats5515/tsp_files/{file}.tsp'
    ret = subprocess.run(command, shell=True)
    # print(ret)
