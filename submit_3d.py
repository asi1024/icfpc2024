import requests
import json
import sys
import convert_string as C

headers = {
    'content-type': 'application/json',
    "Authorization": "Bearer ad978f90-614e-47e4-8f75-f013be2cac97",
}

problem_id = int(sys.argv[1])

sol = sys.stdin.read().strip()
print(sol)

text = f"solve 3d{problem_id}\n{sol}"

print(text)
data = C.to_icfp(text)

print(">", data)
resp = requests.post("https://boundvariable.space/communicate", headers=headers, data=data)
print("<", resp.text)
resp = C.from_icfp(resp.text)
print(resp)




