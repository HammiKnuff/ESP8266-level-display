//made by Hammi 16.02.2018 https://github.com/HammiKnuff


#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_SSD1306.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>

#define OLED_RESET 0  // GPIO0
Adafruit_SSD1306 display(OLED_RESET);

// WiFi Parameters
//**** Change setting for WIFI ******
const char* ssid = "SSID";
const char* password = "PASSWORD";

void setup() {

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(WHITE, BLACK);
  display.setCursor(0, 0);
  display.display();

  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");

    display.print("Connected to");
    display.setCursor(0, 9);
    display.print(ssid);
    delay(5000);
    display.clearDisplay();
  }
}

void loop() {

  //OLED

  // Check WiFi Status
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;  //Object of class HTTPClient
    http.begin("http://www.pegelonline.wsv.de/webservices/rest-api/v2/stations/HAMBURG-ST.PAULI.json?includeTimeseries=true&includeCurrentMeasurement=true");
    int httpCode = http.GET();
    Serial.print("http.GET() = ");
    Serial.println(httpCode);
    //Check the returning code
    if (httpCode > 0) {
      // Parsing
      const size_t bufferSize = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(6) + JSON_OBJECT_SIZE(10) + 540;
      DynamicJsonBuffer jsonBuffer(bufferSize);
      String payload;


      payload = http.getString();
      char json[2048];
      payload.toCharArray(json, sizeof(json));

      JsonObject& root = jsonBuffer.parseObject(json);

      const char* uuid = root["uuid"]; // "d488c5cc-4de9-4631-8ce1-0db0e700b546"
      const char* number = root["number"]; // "5952050"
      const char* shortname = root["shortname"]; // "HAMBURG-ST.PAULI"
      const char* longname = root["longname"]; // "HAMBURG ST. PAULI"
      float km = root["km"]; // 623.1
      const char* agency = root["agency"]; // "HAMBURG PORT AUTHORITY"
      float longitude = root["longitude"]; // 9.969996726842329
      float latitude = root["latitude"]; // 53.54568502657209


      const char* water_longname = root["water"]["longname"]; // "ELBE"

      JsonObject& timeseries0 = root["timeseries"][0];
      const char* timeseries0_longname = timeseries0["longname"]; // "WASSERSTAND ROHDATEN"
      const char* timeseries0_unit = timeseries0["unit"]; // "cm"

      JsonObject& timeseries0_currentMeasurement = timeseries0["currentMeasurement"];
      const char* timeseries0_currentMeasurement_timestamp = timeseries0_currentMeasurement["timestamp"]; // "2018-02-16T03:19:00+01:00"
      int timeseries0_currentMeasurement_value = timeseries0_currentMeasurement["value"]; // 763

      JsonObject& timeseries0_gaugeZero = timeseries0["gaugeZero"];
      const char* timeseries0_gaugeZero_unit = timeseries0_gaugeZero["unit"]; // "m. ü. NHN"
      // Output to serial monitor
      Serial.print(water_longname);
      Serial.println(longname);
      Serial.print(timeseries0_currentMeasurement_value);
      Serial.println(timeseries0_gaugeZero_unit);
      Serial.println(timeseries0_unit);
      //OLED

      display.display();
      display.setCursor(0, 0);
      display.setTextSize(1);
      display.print(longname);
      
      display.setCursor(0, 9);
      display.setTextSize(2);
      display.print(timeseries0_currentMeasurement_value);
      display.setCursor(35, 16);
      display.setTextSize(1);
      display.print(timeseries0_unit);
      display.setCursor(55, 9);
      display.setTextSize(2);
      display.print("NHN"); //units. json has umlauts problems
      display.setCursor(10, 25);
      display.setTextSize(0);
      display.print(timeseries0_currentMeasurement_timestamp);


    }
    http.end();   //Close connection
  }

  // Delay
  delay(10000);
}
