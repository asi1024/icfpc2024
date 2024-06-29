import argparse
import os
import requests
import json
import subprocess
import sys

from bigint_code import generate_code
from convert_string import from_icfp


NAIVE_DFS_PATH = os.path.join(os.path.dirname(__file__), "lambdaman_naive_dfs.py")
DIAM_DFS_PATH = os.path.join(os.path.dirname(__file__), "lambdaman_diameter_dfs.py")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--target", type=str)
    parser.add_argument("--dry-run", action="store_true")
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
        with open(f"lambdaman_data/{i:02d}.txt") as f:
            text = f.read().strip()
        res = subprocess.check_output([sys.executable, DIAM_DFS_PATH], input=text, text=True).strip()

        code = generate_code(i, res)
        print(code)
        if args.dry_run:
            continue

        headers = {
            'content-type': 'application/json',
            "Authorization": "Bearer ad978f90-614e-47e4-8f75-f013be2cac97",
        }
        resp = requests.post("https://boundvariable.space/communicate", headers=headers, data=code)
        resp = from_icfp(resp.text)
        print(resp)


if __name__ == "__main__":
    main()
