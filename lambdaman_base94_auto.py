import requests
import json
import subprocess
import sys

from bigint_code import generate_code
from convert_string import from_icfp


def main():
    for i in range(1, 21):
        print("Problem ID:", i)
        with open(f"lambdaman_data/{i:02d}.txt") as f:
            text = f.read().strip()
        res = subprocess.check_output([sys.executable, "lambdaman_naive_dfs.py"], input=text, text=True).strip()

        code = generate_code(i, res)

        headers = {
            'content-type': 'application/json',
            "Authorization": "Bearer ad978f90-614e-47e4-8f75-f013be2cac97",
        }
        resp = requests.post("https://boundvariable.space/communicate", headers=headers, data=code)
        resp = from_icfp(resp.text)
        print(resp)


if __name__ == "__main__":
    main()
