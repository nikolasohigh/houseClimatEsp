#include <pgmspace.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <EEPROM.h>
#include "EspMQTTClient.h"
#include "ArduinoOTA.h"
#include "replies.h"
#include "constants.h"

WiFiClientSecure client;
X509List cert(TELEGRAM_CERTIFICATE_ROOT);
UniversalTelegramBot bot(BOT_TOKEN, client);
EspMQTTClient MQTTClient (WIFI_SSID, WIFI_PASSWORD, "dev.rightech.io", MQTT_ID);

String temp, pres, hum, ppm;
uint16_t bot_lastmsg, esp_lastled, mqtt_lastsend, esp_lastupdate;
bool status;

void handleNewMessages(int numNewMessages);
void onConnectionEstablished(){}


void setup() {
  //ОБОЗНАЧЕНИЕ ПИНОВ
  pinMode(LED_PIN, OUTPUT);
  
  #ifdef MQTT_DEBUG
    MQTTClient.enableDebuggingMessages(true);
  #endif

  #ifdef BOT_DEBUG
    Serial.begin(9600);
    Serial.print("Подключение к Wi-Fi: ");
    Serial.print(WIFI_SSID);
  #endif

  //ПОДКЛЮЧЕНИЕ К СЕТИ
  client.setTrustAnchors(&cert);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(2, !digitalRead(2));
    delay(100);
  }

  
  #ifdef BOT_DEBUG
    Serial.print("\nWi-Fi подключен. IP: ");
    Serial.println(WiFi.localIP());
  #endif

  //НАСТРОЙКА ВРЕМЕНИ
  configTime(0, 0, "pool.ntp.org");
  time_t now = time(nullptr);
  while (now < 24 * 3600) {
    delay(100);
    now = time(nullptr);
  }
  
  bot.setMyCommands(F("["
                        "{\"command\":\"start\", \"description\":\"Початок роботи\"},"
                        "{\"command\":\"reboot\",\"description\":\"Перезавантаження, на випадок відвалу MQTT-серверу\"},"
                        "{\"command\":\"report\",\"description\":\"Поскаржитися: /report [скарга]\"}"
                      "]"));


 
}



void loop() {
  MQTTClient.loop();
  
  //ПРОВЕРКА СООБЩЕНИЙ В БОТЕ
  if (millis() - bot_lastmsg > BOT_MSG_DELAY) {

    bot_lastmsg = millis();
    uint8_t numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages) {
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
      /*ПРОВЕРКА НА ФЛУД*/
      if(numNewMessages >= 3) break;
    }
  }

  //МИГАНИЕ СВЕТОДИОДОМ (ПО ПРИКОЛУ)
  if (millis() - esp_lastled > LED_DELAY) {
    esp_lastled = millis();
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  }

  //ОТПРАВКА НА MQTT-СЕРВЕР
  if (MQTTClient.isConnected() && (millis() - mqtt_lastsend > PUB_DELAY)) {
      mqtt_lastsend = millis();
      MQTTClient.publish(TEMP_TOPIC, temp);
      MQTTClient.publish(HUM_TOPIC, hum);
      MQTTClient.publish(PRES_TOPIC, pres);
      MQTTClient.publish(PPM_TOPIC, ppm);
    }
  //ОБРАБОТКА ДАТЧИКОВ
  if (millis() - esp_lastupdate > UPDATE_DELAY) {
    esp_lastupdate = millis();
    temp = String(random(20, 30));
    hum = String(random(0, 100));
    pres = String(random(100, 300));
    ppm = String(random(0, 700));
  }
}




/*ОБРАБОТКА СООБЩЕНИЙ*/
void handleNewMessages(int numNewMessages) {
    #ifdef BOT_DEBUG
    Serial.print("Новое сообщение\n");
    #endif
  String answer;
  for (int i = 0; i < numNewMessages; i++) {
    telegramMessage& msg = bot.messages[i];
    
      
     /*ПРОВЕРКА КОЛБЕК-КОМАНД НА ИХ ВАЛИДНОСТЬ*/
     if(msg.type == "callback_query") {
        if (msg.text == "CALLBACK_reboot") {
          if (millis() > REBOOT_DELAY) ESP.reset();
          if (millis() < REBOOT_DELAY) {
            bot.sendMessage(msg.chat_id, String(FPSTR(REBOOTED_MSG)), "HTML");
            continue;
          } 
        }
        if (msg.text == "continue") {
          String keyboardJson = "[[\"Статус\", \" Перезавантажити\"]]";
          bot.sendMessageWithReplyKeyboard(msg.chat_id, "Вітаємо в головному меню боту", "", keyboardJson, true);
          continue;
        }
    }

    /*ПРОВЕРКА НА ДЛИННУ СООБЩЕНИЯ*/
    if (msg.text.length() > MAX_MESSAGE_LENGTH) {
      bot.sendMessage(msg.chat_id, String(FPSTR(BIG_MSG_ERR)), "HTML");
      bot.sendPhoto(msg.chat_id, BIG_MSG_IMG);

      #ifdef BOT_DEBUG
        Serial.println("Слишком большое сообщение от " + msg.from_name + ". id: " + msg.chat_id + "\n");
      #endif

      continue;
    }

    #ifdef BOT_DEBUG
      Serial.println("Получено " + msg.text + " от " + msg.from_name + ". id: " + msg.chat_id + "\n");
    #endif
    
    
    /*ОБРАБОТКА КОМАНДЫ СТАРТ, /start*/
    if (msg.text == "/start") {
      answer = "";
      String keyboardJson = "[[\"Статус\", \" Перезавантажити\"]]";
          bot.sendMessageWithReplyKeyboard(msg.chat_id, "Вітаємо в головному меню боту", "", keyboardJson, true);
    } 
    
    /*ОБРАБОТКА КОМАНДЫ СТАТУС, /status*/
    else if (msg.text == "Статус" || msg.text == "/status") {
      answer += String(FPSTR(STATUS_HEADER_STRING));
      answer += String(FPSTR(STATUS_TEMP_STRING)) + temp;                              // + String(getTemp());
      answer += String(FPSTR(STATUS_HUMIDITY_STRING)) + hum;                          // + String(getHumigity());
      answer += String(FPSTR(STATUS_PPM_STRING)) + ppm;                               // + String(getPPM());
      answer += String(FPSTR(STATUS_PRESSURE_STRING)) + pres;                          // + String(getPressure());
      
      answer += "\n\n";

      if (MQTTClient.isMqttConnected()) answer+= String(FPSTR(STATUS_MQTT_CONNECTED));  
      else answer+= String(FPSTR(STATUS_MQTT_DISCONNECTED));
    } 
    
    /*ОБРАБОТКА КОМАНДЫ /send*/
    else if (msg.text.startsWith("/send")) {
      //Проверка на админа
      if (msg.chat_id != ADMIN_ID) 
      bot.sendMessage(msg.chat_id, String(FPSTR(YOU_ARE_NOT_ADMIN)), "HTML");

      String sendCommand = msg.text.substring(6);
      int spaceIndex = sendCommand.indexOf(' ');

      if (spaceIndex != -1) {
        String userIdStr = sendCommand.substring(0, spaceIndex);
        String adminMsg = sendCommand.substring(spaceIndex + 1);

        if (userIdStr.toInt() == 0)                                                                                           answer = String(FPSTR(INVALID_ID));
        else if (userIdStr.toInt() != 0 && bot.sendMessage(userIdStr, String(FPSTR(MSG_FROM_ADMIN)) + adminMsg, "Markdown"))  answer = String(FPSTR(SENDED_TO_USER)) + userIdStr;
      } 
      else answer = String(FPSTR(INVALID_FORMAT));
    }

    /*ОБРАБОТКА КОМАНДЫ ОБРАТНОЙ СВЯЗИ /report*/
    else if (msg.text.startsWith("/report ")) {
      if (bot.sendMessage(ADMIN_ID, "Скарга від користувача: <b>" + msg.from_name + "</b> \nid: <b>" + String(msg.chat_id) + ".</b>\nТекст скарги: " + msg.text.substring(8), "HTML"))
        answer = String(FPSTR(SENDED_TO_ADMIN));
      else
        answer = String(FPSTR(UNKNOWN_ERR));
    }

    /*ОБРАБОТКА ПЕРЕЗАГРУЗКИ*/
    else if (msg.text == "/reboot" || msg.text == "Перезавантажити") {
      if (msg.chat_id != ADMIN_ID) bot.sendMessage(msg.chat_id, String(FPSTR(YOU_ARE_NOT_ADMIN)), "HTML");
      else {
        String keyboardJson = "[[{ \"text\" : \"Так\", \"callback_data\" : \"CALLBACK_reboot\" }],[{ \"text\" : \"Hi\", \"callback_data\" : \"continue\" }]]";
        bot.sendMessageWithInlineKeyboard(msg.chat_id, String(FPSTR(REBOOT_MSG)), "", keyboardJson);
      }
    } 
    /*ОБРАБОТКА НОВЫХ КОМАНД*/


    else {
      answer = String(FPSTR(COMMAND_ERR));
    }
    if (bot.sendMessage(msg.chat_id, answer, "HTML")) answer = "";
    else answer = String(FPSTR(UNKNOWN_ERR)); 
  }
}

