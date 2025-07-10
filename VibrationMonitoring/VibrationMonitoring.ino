#include <Wire.h>
#include <MPU6050.h>
#include "arduinoFFT.h"
#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "ssid";
const char* password = "password';

String apiKey = "N6QYHUPPY7PO2A9D";
const char* server = "http://api.thingspeak.com/update";

#define SAMPLES 128              
#define SAMPLING_FREQUENCY 500  

MPU6050 mpu;
ArduinoFFT<double> FFT = ArduinoFFT<double>();

unsigned int samplingPeriod_us;
unsigned long microseconds;

double vReal[SAMPLES];
double vImag[SAMPLES];

double motorRPM = 1800; 
double motorFreq_Hz;

double faultThreshold = 7000; 

void setup() {
  Serial.begin(115200);
  Wire.begin();
  mpu.initialize();
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  if (!mpu.testConnection()) {
    Serial.println("MPU6050 connection failed");
    while (1);
  }

  samplingPeriod_us = round(1000000 * (1.0 / SAMPLING_FREQUENCY));
  motorFreq_Hz = motorRPM / 60.0; 
}


// Add this function to send data to ThingSpeak
void sendDataToThingSpeak(double peakFreq, double ampAtRPM, int faultStatus) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = String(server) + "?api_key=" + apiKey + 
                 "&field1=" + String(peakFreq) +
                 "&field2=" + String(ampAtRPM) +
                 "&field3=" + String(faultStatus);

    http.begin(url);
    int httpResponseCode = http.GET();
    
    if (httpResponseCode > 0) {
      Serial.print("Data sent successfully: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.print("Error sending data: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  }
}
void loop() {
  // Step 1: Collect Samples
  for (int i = 0; i < SAMPLES; i++) {
    microseconds = micros();

    int16_t ax, ay, az;
    mpu.getAcceleration(&ax, &ay, &az);

    vReal[i] = (double)ax;  // Raw acceleration data
    vImag[i] = 0;

    while (micros() < (microseconds + samplingPeriod_us)) {
      // Wait until the next sampling point
    }
  }

  // Step 2: Apply FFT
  FFT.windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.compute(vReal, vImag, SAMPLES, FFT_FORWARD);
  FFT.complexToMagnitude(vReal, vImag, SAMPLES);

  // Step 3: Find 1x RPM Peak
  double peakFrequency = FFT.majorPeak(vReal, SAMPLES, SAMPLING_FREQUENCY);
  double peakAmplitude = findAmplitudeAtFrequency(motorFreq_Hz);

  Serial.print("Peak Frequency: ");
  Serial.print(peakFrequency);
  Serial.print(" Hz, Amplitude at 1x RPM: ");
  Serial.println(peakAmplitude);

  // Step 4: Fault Detection
  if (peakAmplitude > faultThreshold) {
    Serial.println("⚠️ Fault Detected: Possible Unbalance!");
  } else {
    Serial.println("✅ System Normal.");
  }
  sendDataToThingSpeak(peakFrequency, peakAmplitude, (peakAmplitude > faultThreshold) ? 1 : 0);
  delay(14000); // Check every second
}

double findAmplitudeAtFrequency(double targetFreq) {
  int index = (targetFreq * SAMPLES) / SAMPLING_FREQUENCY;
  if (index >= SAMPLES) index = SAMPLES - 1;
  return vReal[index];
}
