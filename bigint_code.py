import sys
import convert_string


def base94(n: int) -> str:
    if n == 0:
        return "!"
    s = ""
    while n:
        s += chr(n % 94 + 33)
        n //= 94
    return s[::-1]


def generate_code_by_one(problem_id: int, sol: str) -> str:
    convert_map = {"U": 0, "D": 1, "L": 2, "R": 3}
    convert_str = "SO>FL"

    if sol[-1] == "U":
        convert_map = {"U": 1, "D": 0, "L": 2, "R": 3}
        convert_str = "S>OFL"

    n = 0
    for i in range(len(sol) - 1, -1, -1):
        v = convert_map[sol[i]]
        n = n * 4 + v

    prefix = convert_string.to_icfp(f"solve lambdaman{problem_id} ")
    num = base94(n)
    prog = f"""B. S{prefix} B$ B$ Lf B$ Lx B$ vf B$ vx vx Lx B$ vf B$ vx vx Lf Ln ? B= vn I! S B. BT I" BD B% vn I% {convert_str} B$ vf B/ vn I% I{num}"""
    return prog


def generate_code_by_two(problem_id: int, sol: str) -> str:
    convert_map = {"U": 0, "D": 1, "L": 2, "R": 3}
    convert_str = "SOO>>FFLL"

    if sol[-1] == "U":
        convert_map = {"U": 1, "D": 0, "L": 2, "R": 3}
        convert_str = "S>>OOFFLL"

    n = 0
    for i in range(len(sol) - 1, -1, -1):
        v = convert_map[sol[i]]
        n = n * 4 + v

    prefix = convert_string.to_icfp(f"solve lambdaman{problem_id} ")
    num = base94(n)
    prog = f"""B. S{prefix} B$ B$ Lf B$ Lx B$ vf B$ vx vx Lx B$ vf B$ vx vx Lf Ln ? B= vn I! S B. BT I# BD B* I# B% vn I% {convert_str} B$ vf B/ vn I% I{num}"""
    return prog


def generate_code(problem_id: int, sol: str) -> str:
    s1 = sol[::2]
    s2 = sol[1::2]
    if s1 == s2:
        return generate_code_by_two(problem_id, s1)
    else:
        return generate_code_by_one(problem_id, sol)


if __name__ == "__main__":
    s = input().strip()
    id = int(sys.argv[1])

    prog = generate_code(id, s)
    print(prog, end="")
