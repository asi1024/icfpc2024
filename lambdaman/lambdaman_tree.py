import os
import sys

from lambdaman.bigint_code import base94
from convert_string import to_icfp

sys.setrecursionlimit(10 ** 6)


def load_board(id: int) -> list[str]:
    path = os.path.join(os.path.dirname(__file__), f"../lambdaman_data/{id:02d}.txt")
    with open(path, "r") as fp:
        res = fp.readlines()
    res = list(map(str.strip, res))
    return res


def find_start_pos(board: list[str]) -> tuple[int, int]:
    for i, row in enumerate(board):
        for j, cell in enumerate(row):
            if cell == "L":
                return i, j
    return -1, -1


DIR_NAME = ["U", "L", "D", "R"]
DIRS = [(-1, 0), (0, -1), (1, 0), (0, 1)]


def is_path(board: list[str], y: int, x: int) -> bool:
    return 0 <= y < len(board) and 0 <= x < len(board[0]) and board[y][x] != "#"


def dfs(board: list[str], y: int, x: int, initial_dir: int, to_right: bool, last_dir: int, maybe_last: bool, buf: list[int]):
    n_dir = 4 if last_dir == -1 else 3
    dirs = []
    if last_dir == -1:
        last_dir = initial_dir

    for i in range(n_dir):
        if to_right:
            cand = (last_dir - 1 + i) % 4
        else:
            cand = (last_dir + 1 - i) % 4
        y2 = y + DIRS[cand][0]
        x2 = x + DIRS[cand][1]
        if is_path(board, y2, x2):
            dirs.append(cand)

    for i, d in enumerate(dirs):
        y2 = y + DIRS[d][0] * 2
        x2 = x + DIRS[d][1] * 2

        buf.append(d)
        is_last = maybe_last and i == len(dirs) - 1
        dfs(board, y2, x2, initial_dir, to_right, d, is_last, buf)
        if not is_last:
            buf.append((d + 2) % 4)


def run_compress(seq: list[int], to_right: bool) -> list[int]:
    ret = []
    i = 0
    while i < len(seq):
        if i + 1 < len(seq) and seq[i + 1] == (seq[i] - (1 if to_right else -1)) % 4:
            ret.append(seq[i])
            i += 2
        else:
            ret.append(seq[i])
            i += 1
    return ret


def solve(id: int) -> str:
    board = load_board(id)
    height = len(board)
    width = len(board[0])

    sy, sx = find_start_pos(board)

    best_initial_dir = -1
    best_to_right = True
    best_buf = []

    for initial_dir in range(4):
        for to_right in [True, False]:
            buf = []
            dfs(board, sy, sx, initial_dir, to_right, -1, True, buf)
            buf = run_compress(buf, to_right)

            if best_initial_dir == -1 or len(buf) < len(best_buf):
                best_initial_dir = initial_dir
                best_to_right = to_right
                best_buf = buf

    if best_to_right:
        convert_str = "OOLL>>FF"
        best_buf = [(4 - d) % 4 for d in best_buf]
    else:
        convert_str = "OOFF>>LL"

    if best_buf[-1] == 0:
        best_buf = [(d - 1) % 4 for d in best_buf]
        convert_str = convert_str[2:] + convert_str[:2]

    convert_str = convert_str + convert_str[:2]

    n = 0
    for i in range(len(best_buf) - 1, -1, -1):
        n = n * 4 + best_buf[i]
    prefix = to_icfp(f"solve lambdaman{id} ")[1:]
    num = base94(n)
    prog = f"""B. S{prefix} B$ B$ Lf B$ vf vf Lf Ln ? B= vn I! S B. BT I% BD B* I# B% vn I% S{convert_str} B$ B$ vf vf B/ vn I% I{num}"""
    return prog


if __name__ == "__main__":
    id = int(sys.argv[1])
    solve(id)
