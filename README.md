
# Wireless Monitoring System

This system aim to detect unbalance in a motor via ArduinoFFT library at 1x RPM.





## Installation

Connect circuit following 

picture

Open GY-521 File

Change the Wi-Fi Details

```bash
  const char* ssid = "ssid";
  const char* password = "password';
```

Change the apiKey

```bash
  String apiKey = "Api";
```

Adjust the RPM and the faultThreshold based on system
```bash
  double motorRPM = 1800; 
  double faultThreshold = 7000; 
```

Upload code 
