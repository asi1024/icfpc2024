import requests
import json
import pandas
from datetime import datetime, timedelta

headers = {
    'content-type': 'application/json',
    "Authorization": "Bearer ad978f90-614e-47e4-8f75-f013be2cac97",
}

data = json.dumps({"body": "S'%4}).$%8"})
resp = requests.post("https://boundvariable.space/communicate", headers=headers, data=data)
print(resp.text)


