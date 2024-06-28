#%%

data = []
try:
    while True:
        line = input()
        if line == "":
            break
        data.append(line)
except EOFError:
    pass


W = len(data[0])
H = len(data)

pos = None
for i in range(H):
    for j in range(W):
        if data[i][j] == "L":
            pos = (i, j)

moves = {
    "L": (0, -1, "R"),
    "R": (0, 1, "L"),
    "U": (-1, 0, "D"),
    "D": (1, 0, "U"),
}

visited = set()
def dfs(p):
    visited.add(p)
    i, j = p
    
    m = ""
    for (k, (di, dj, opp)) in moves.items():
        ni = i + di
        nj = j + dj
        if ni < 0 or ni >= H or nj < 0 or nj >= W:
            continue
        if data[ni][nj] == "#":
            continue
        if (ni, nj) in visited:
            continue
        m += k + dfs((ni, nj)) + opp

    return m

print(dfs(pos))
    
# %%
