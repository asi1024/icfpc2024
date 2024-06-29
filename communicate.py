import requests
import json
import convert_string as C

headers = {
    'content-type': 'application/json',
    "Authorization": "Bearer ad978f90-614e-47e4-8f75-f013be2cac97",
}

data = """B. S3/,6%},!-"$!-!.^} B$ LF B$ vF B. B$ vF SLL B$ vF SFF Lt B$ Lf B. B$ vf B$ vf B$ vf vt S> Ls B. vs B. vs vs"""
# data = C.to_icfp(text)
# data = f"B. {C.to_icfp('echo ')} {program}"
print(">", data)
resp = requests.post("https://boundvariable.space/communicate", headers=headers, data=data)
print("<", resp.text)
resp = C.from_icfp(resp.text)
print(resp)




