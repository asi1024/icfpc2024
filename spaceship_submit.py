import requests
import json
import subprocess
import sys

from bigint_code import generate_code
from convert_string import from_icfp
from convert_string import to_icfp


def main():
    for problem_id in range(3, 4):
        with open(f"spaceship/ats5515/out/{problem_id:02}.txt") as f:
            text = f.read()
        code = to_icfp(f"Ysolve spaceship{problem_id} " + text)
        headers = {
            'content-type': 'application/json',
            "Authorization": "Bearer ad978f90-614e-47e4-8f75-f013be2cac97",
        }
        resp = requests.post("https://boundvariable.space/communicate", headers=headers, data=code)
        resp = from_icfp(resp.text)
        print(resp)


if __name__ == "__main__":
    main()
