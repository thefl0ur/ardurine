import requests
from datetime import datetime
from pathlib import Path
from string import Template


PROVIDER = "https://api.sunrisesunset.io/json"

req_params = {
    "lat": "35.76956",
    "lng": "-5.803091",
    "date_start": datetime(datetime.now().year, 1, 1).strftime("%Y-%m-%d"),
    "date_end": datetime(datetime.now().year, 12, 31).strftime("%Y-%m-%d"),
    "time_format": "24",
    "time_zone": "Africa/Casablanca",
}

resp = requests.get(PROVIDER, req_params, timeout=30)
resp.raise_for_status()

data = []
for item in resp.json()["results"]:
    date_value = datetime.strptime(item["date"], "%Y-%m-%d").date()
    data.append(
        (
            datetime.combine(
                date_value, datetime.strptime(item["sunrise"], "%H:%M:%S").time()
            ).timestamp(),
            datetime.combine(
                date_value, datetime.strptime(item["sunset"], "%H:%M:%S").time()
            ).timestamp(),
        )
    )

with open("sunrise_data.h", "w") as f:
    template = Template(Path("sunrise_data_template.tpl").read_text())
    f.write(
        template.substitute(
            {
                "data_length": len(data),
                "data_items": ",\r\n    ".join(
                    f"{{{int(x[0])}, {int(x[1])} }}" for x in data
                ),
            }
        )
    )
