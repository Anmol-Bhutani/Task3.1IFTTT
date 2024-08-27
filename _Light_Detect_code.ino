#include <Wire.h>
#include <BH1750.h>
#include <WiFiNINA.h>
#include <ArduinoHttpClient.h>

BH1750 lightMeter;

const char ssid[] = "Redmi";
const char pass[] = "123456789";
const char iftttUrl[] = "maker.ifttt.com";
const char iftttEvent[] = "/trigger/Sunlight/with/key/hkPO-rc2BtnEodiO7m4UjHLGQwGsIyKV8B8L4RQtI7C";

WiFiClient wifiClient;
HttpClient client = HttpClient(wifiClient, iftttUrl, 80);

void setup() {
  Serial.begin(9600);
  
  Wire.begin();
  if (!lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println(F("Error initializing BH1750!"));
    while (1);
  }

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi!");
}

void loop() {
  float lux = lightMeter.readLightLevel();
  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lx");
  
  // For sending notifications
  if (lux > 100.0) {
    sendIFTTTTrigger("Light Detected");
    delay(10000);
  } else {
    sendIFTTTTrigger("Not Detected");
    delay(10000);
  }
}

void sendIFTTTTrigger(const char* message) {
  client.beginRequest();
  client.post(iftttEvent);
  client.sendHeader("Content-Type", "application/json");
  
  String payload = "{\"value1\":\"";
  payload += message;
  payload += "\"}";
  
  client.sendHeader("Content-Length", payload.length());
  client.beginBody();
  client.print(payload);
  client.endRequest();
  
  int statusCode = client.responseStatusCode();
  String response = client.responseBody();
  Serial.println("Status code: " + String(statusCode));
  Serial.println("Response: " + response);
}
