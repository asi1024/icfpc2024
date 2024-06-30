import sys
from typing import Optional

import cspuz

sys.setrecursionlimit(10 ** 6)


def load_from_stdin():
    data = []
    for line in sys.stdin:
        line = line.strip()
        if line == "":
            break
        x, y = map(int, line.split())
        data.append((x, y))
    return data


def compute_accelerate(points: list[tuple[int, int]], num_steps: int) -> Optional[list[tuple[int, int]]]:
    solver = cspuz.Solver()
    acc_x = solver.int_array(num_steps, -1, 1)
    acc_y = solver.int_array(num_steps, -1, 1)
    cx = []
    cy = []
    for i in range(num_steps):
        s = (i + 1) * (i + 2) // 2
        x = solver.int_var(-s, s)
        cx.append(x)

        ex = 0
        for j in range(i + 1):
            for _ in range(i + 1 - j):
                ex += acc_x[j]
        solver.ensure(ex == x)

        y = solver.int_var(-s, s)
        cy.append(y)

        ey = 0
        for j in range(i + 1):
            for _ in range(i + 1 - j):
                ey += acc_y[j]
        solver.ensure(ey == y)

    for i in range(len(points)):
        cond = []
        for j in range(num_steps):
            cond.append((cx[j] == points[i][0]) & (cy[j] == points[i][1]))
        solver.ensure(cspuz.fold_or(cond))

    is_sat = solver.find_answer()
    if not is_sat:
        return None

    ret = []
    for i in range(num_steps):
        ret.append((acc_x[i].sol, acc_y[i].sol))

    return ret


def main():
    max_steps = int(sys.argv[1])
    print(f"max_steps = {max_steps}", file=sys.stderr)

    pts = load_from_stdin()

    accs = compute_accelerate(pts, max_steps)
    if accs is None:
        print("no solution", file=sys.stderr)
        return

    for i in range(max_steps):
        print(accs[i][0], accs[i][1])


if __name__ == "__main__":
    main()
