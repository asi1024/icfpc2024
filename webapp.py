
import requests
import json
import convert_string as C
import re

headers = {
    'content-type': 'application/json',
    "Authorization": "Bearer ad978f90-614e-47e4-8f75-f013be2cac97",
}
from flask import Flask, render_template
from markupsafe import Markup
from markdown import markdown
app = Flask(__name__)


link_pat = re.compile(r"\[([a-zA-Z0-9 .,]+)\](?!\()")

@app.route("/<page>", methods=["GET"])
def get_page(page: str) -> str:
    text = f"get {page}"
    data = C.to_icfp(text)
    print(data)
    resp = requests.post("https://boundvariable.space/communicate", headers=headers, data=data)
    print(resp.text)
    resp = C.from_icfp(resp.text)
    print(resp)

    ret = link_pat.sub(r"[\1](\1)", resp)
    print(markdown(ret))
    return markdown(ret)

@app.route("/")
def index():
    return get_page("index")



if __name__ == "__main__":
    # webサーバー立ち上げ
    app.run()