## Finder

**Finder** is a compact Python script designed to identify geographical locations based on MAC addresses. This tool works in conjunction with the "St4lker" project by utilizing MAC address data.

### How It Works

1. **Data Transfer**: Copy the `history.txt` file from your "St4lker" project via UART.
2. **Integration**: Paste the contents into the `history.txt` file of the Finder script.
3. **Execution**: Run the Finder script to lookup and identify the geographical locations associated with the IP addresses from the provided MAC addresses.



### Installation

```bash
cd finder
pip install -r requirements.txt
```



### Usage

1. Copy and paste the history.txt content

2. Put your Geolocation API Key in the .env file

3. Run the python script:

   ```bash
   python finder.py
   ```

   

### How to get a Geolocation API KEY

1. Open your Firefox web browser;

2. Intercept the traffic with a proxy;

3. Run the following command in your console:

   ```javascript
   navigator.geolocation.getCurrentPosition(function(){})
   ```

   

4. Allow the location access;
5. Verify the API Key in your proxy history.