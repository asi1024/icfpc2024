import argparse
import os
import requests

from convert_string import from_icfp


def add_pad(s: str, pad: int) -> str:
    return "? F S" + "a" * max(0, pad - 6) + " " + s


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--target", type=str)
    parser.add_argument("--dry-run", action="store_true")
    parser.add_argument("--pad", type=int)
    args = parser.parse_args()

    if args.target is not None:
        target = []
        for tok in args.target.split(","):
            if "-" in tok:
                a, b = map(int, tok.split("-"))
                target += list(range(a, b + 1))
            else:
                target.append(int(tok))
    else:
        target = list(range(1, 22))

    for i in target:
        print("Problem ID:", i)
        sol_path = os.path.join(os.path.dirname(__file__), f"../lambdaman_sol/randomwalk/{i:02d}.txt")
        if not os.path.exists(sol_path):
            print("Solution file not found")
            continue

        best_sol = None
        with open(sol_path, "r") as f:
            for line in f:
                s = line.strip()
                if s == "":
                    continue
                if best_sol is None or len(s) < len(best_sol):
                    best_sol = s

        if best_sol is None:
            print("sol not found")
            continue

        if args.pad is not None:
            best_sol = add_pad(best_sol, args.pad)
        headers = {
            'content-type': 'application/json',
            "Authorization": "Bearer ad978f90-614e-47e4-8f75-f013be2cac97",
        }
        print("Solution length:", len(best_sol))
        if args.dry_run:
            continue

        resp = requests.post("https://boundvariable.space/communicate", headers=headers, data=best_sol)
        resp = from_icfp(resp.text)
        print(resp)


if __name__ == "__main__":
    main()
