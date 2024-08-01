import re
import requests
import os
import json
from dotenv import load_dotenv

load_dotenv()
api_key = os.getenv('GEOLOCATION_KEY')
file_path = 'history.txt'

def geolocation(mac_address):
    url = "https://www.googleapis.com:443/geolocation/v1/geolocate?key="+ api_key
    headers = {"Content-Type": "application/json; charset=UTF-8"}
    json={"wifiAccessPoints": [{"macAddress": mac_address}]}
    response = requests.post(url, headers=headers, json=json)
    print(" ======== MAC ADDRESS: " + mac_address + " ======== ")
    print("Latitude: " + str(response.json()["location"]["lat"]))
    print("Logitude: " + str(response.json()["location"]["lng"]))

# Read the file and extract MAC addresses
mac_address = ""
with open(file_path, 'r') as file:
    for line in file:
        match = re.search(r'([0-9a-fA-F]{2}:[0-9a-fA-F]{2}:[0-9a-fA-F]{2}:[0-9a-fA-F]{2}:[0-9a-fA-F]{2}:[0-9a-fA-F]{2})', line)
        if match:
            mac_address = match.group(1)
            mac_address = mac_address.replace(":","-")
            geolocation(mac_address)
