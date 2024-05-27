#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <time.h>
#include <ESP8266HTTPClient.h>
#include "GyverTimer.h"
#include <GyverOLED.h>
#include <GyverBME280.h>
#include <SoftwareSerial.h>
#include "MHZ19.h"
#include "constants.h"


GyverBME280 bme;
GyverOLED<SSD1306_128x64, OLED_BUFFER> screen;
MHZ19 *mhz19_uart = new MHZ19(RX_PIN, TX_PIN);

WiFiClient client;
HTTPClient http;

int humidity, temperature, pressure, ppm;

GTimer sendDataTimer(MS);
GTimer readSensorsTimer(MS);
GTimer getTimeTimer(MS);


String getResponse();
void connectToWiFi();
void HTTPPost(int numFields , String fieldData[]);
void readSensors();
void sendData();
void showOffline();
void getTime();
void updateScreen();

void setup() {
    Serial.begin(9600);
    screen.init();  
    bme.begin();
    mhz19_uart -> begin(RX_PIN, TX_PIN);
    mhz19_uart -> setAutoCalibration(false);        
    connectToWiFi();
    readSensorsTimer.setInterval(5000);
    
    if (WiFi.status() == WL_CONNECTED) {
        configTime(3600*UTC, 3600*UTCS, "pool.ntp.org");
        sendDataTimer.setInterval(60000);
        getTimeTimer.setInterval(1000);
    }
    screen.clear();
    screen.fastLineH(8, 0, 128, 1);
}

void loop() {
    if(readSensorsTimer.isReady()) readSensors();
    if(sendDataTimer.isReady()) sendData();
    if(getTimeTimer.isReady()) getTime();
}

void getTime() {
    char formattedTime[22];
    time_t now = time(nullptr);
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);
    Serial.printf("Дата: %04d-%02d-%02d %02d:%02d:%02d\n",
                timeinfo.tm_year + 1900,
                timeinfo.tm_mon,
                timeinfo.tm_mday,
                timeinfo.tm_hour,
                timeinfo.tm_min,
                timeinfo.tm_sec);
    
    screen.setCursor(0, 0);
    snprintf(formattedTime, sizeof(formattedTime), "%02d:%02d      %02d.%02d.%04d",
            timeinfo.tm_hour,
            timeinfo.tm_min,
            timeinfo.tm_mday,
            timeinfo.tm_mon+1,
            timeinfo.tm_year + 1900);

    screen.print(formattedTime);
    screen.update();
    }

void connectToWiFi() {
  WiFi.begin(WIFI_SSID , WIFI_PASSWORD);

    while (WiFi.status() == WL_DISCONNECTED) {
        Serial.println("Підключення до Wi-Fi мережі");
        screen.clear();
        screen.setScale(1);
        screen.setCursor(5, 0);
        screen.print("Пiдключення до Wi-Fi");
        screen.update(); 
        delay(2000);
    }

    if (WiFi.status() != WL_DISCONNECTED) {
        switch(WiFi.status()) {
            case WL_NO_SSID_AVAIL:
                Serial.println("WiFi мережа з іменем " + String(WIFI_SSID) + " не знайдена");
                showOffline();
                break;
            case WL_CONNECT_FAILED:
                Serial.println("Помилка підключення до мережі");
                showOffline();
                break;
            case WL_WRONG_PASSWORD:
                Serial.println("Ви ввели неправильний пароль до мережі " + String(WIFI_SSID));
                showOffline();
                break;
            case WL_CONNECTED:
                Serial.println("\nПідключено до мережі " + String(WIFI_SSID));
                Serial.println("Локальна IP-адреса: " + WiFi.localIP().toString());
                Serial.println("MAC-адреса: " + WiFi.macAddress());
                Serial.println("Адреса шлюзу: " + WiFi.gatewayIP().toString());
                Serial.println("Адреса DNS-серверу: " + WiFi.dnsIP().toString());
                Serial.println("Адреса шлюзу: " + WiFi.gatewayIP().toString());
                screen.setCursor(35, 2);
                screen.print("Пiдключено");
                screen.update();
                delay(1000);
                screen.clear();
                screen.setCursor(0, 0);
                screen.print("IP: " + WiFi.localIP().toString());
                screen.setCursor(0, 2);
                screen.print("MAC:" + WiFi.macAddress());
                screen.setCursor(0, 4);
                screen.print("Шлюз: " + WiFi.gatewayIP().toString());
                screen.setCursor(0, 6);
                screen.print("DNS: " + WiFi.dnsIP().toString());
                screen.update();
                break;
            default:
                Serial.println("Невідома помилка. Повідомте розробнику");
                showOffline();
                break;
        }
    }
}

void readSensors() {
    measurement_t m = mhz19_uart -> getMeasurement();
    humidity = (int)bme.readHumidity();
    temperature = (int)bme.readTemperature();
    pressure = (int)pressureToMmHg(bme.readPressure());
    ppm = (int)m.co2_ppm;

    screen.setCursor(0, 2);
    screen.print("Температура: " + String(temperature));
    screen.setCursor(0, 3);
    screen.print("Тиск:        " + String(pressure));
    screen.setCursor(0, 4);
    screen.print("Вологiсть:   " + String(humidity));
    screen.setCursor(0, 5);
    screen.print("PPM:         " + String(ppm));

}

void showOffline() {
    Serial.println("Запуск у офнлайн режимі");
    screen.setScale(2);
    screen.clear();
    screen.setCursor(29, 3);
    screen.print("Офлайн");
    screen.update();
    delay(1000);
}

void sendData() {
    String fieldData[NUM_FIELDS]; 
    fieldData[TEMPERATURE_FIELD] = String(temperature);
    fieldData[HUMIDITY_FIELD]    = String(humidity);
    fieldData[PRESSURE_FIELD]    = String(pressure);
    fieldData[PPM_FIELD]         = String(ppm);
    HTTPPost(NUM_FIELDS, fieldData);
}

void HTTPPost(int numFields , String fieldData[]) {
    if (client.connect(THING_SPEAK_ADDRESS, 80)) {
       String postData = WRITE_API_KEY;
       for (int fieldNumber = 1; fieldNumber < numFields+1; fieldNumber++) {
            String fieldName = "field" + String(fieldNumber);
            postData += "&" + fieldName + "=" + fieldData[fieldNumber-1];
            }

        Serial.println("Підключення до серверу ThingSpeak...");
        Serial.println();
        client.println("POST /update HTTP/1.1");
        client.println("Host: api.thingspeak.com");
        client.println("Connection: close");
        client.println("Content-Type: application/x-www-form-urlencoded");
        client.println("Content-Length: " + String(postData.length()));
        client.println();
        client.println(postData);
        Serial.println(postData);
        String answer = getResponse();
        Serial.println(answer);
    }
    else {
      Serial.println ("Помилка підключення");
    }
}

String getResponse() {
  String response;
  long startTime = millis();
  delay(200);
  while (client.available() < 1 && ((millis() - startTime) < TIMEOUT)) {
        delay(5);
  }
  if (client.available() > 0) {
     char charIn;
     do {
         charIn = client.read();
         response += charIn; 
        } while (client.available() > 0);
    }
  client.stop();
  return response;
}

