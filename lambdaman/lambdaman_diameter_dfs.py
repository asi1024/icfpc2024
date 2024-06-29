import sys

sys.setrecursionlimit(10 ** 6)


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

moves = [
    ("L", 0, -1, "R"),
    ("R", 0, 1, "L"),
    ("U", -1, 0, "D"),
    ("D", 1, 0, "U"),
]


visited = set()
childs = {}

def visit(y, x):
    global visited
    global childs

    visited.add((y, x))
    dist = 0
    child = []
    for i, (_dir, dy, dx, _opp) in enumerate(moves):
        ny, nx = y + dy, x + dx
        if ny < 0 or ny >= H or nx < 0 or nx >= W:
            continue
        if data[ny][nx] == "#":
            continue
        if (ny, nx) in visited:
            continue
        d = visit(ny, nx)
        child.append((d, i))
        dist = max(dist, 1 + d)
    child.sort()
    childs[(y, x)] = child
    return dist


d = visit(*pos)

def visit2(y, x):
    global childs
    res = ""
    for _, i in childs[(y, x)]:
        dir, dy, dx, opp = moves[i]
        ny, nx = y + dy, x + dx
        res += dir + visit2(ny, nx) + opp
    return res


ans = visit2(*pos)[:-d]
print(ans)
