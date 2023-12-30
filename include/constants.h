/*ДЛЯ ДЕБАГА УБРАТЬ СЛЕДУЮЩИЙ ОДНОСТРОЧНЫЙ КОММЕНТ*/
//#define BOT_DEBUG
//#define MQTT_DEBUG


/*АВТОРИЗАЦИОННЫЕ ДАННЫЕ*/
#define MQTT_ID                 "mqtt-kolya_bondar_yt-3f68k5"                       //Брать у dev.rightech.io
#define WIFI_SSID               "ASOG BBW 56"                                       //SSID Вашей WI-FI сети
#define WIFI_PASSWORD           "viA12439#56"                                       //Пароль Вашей WI-FI сети
#define BOT_TOKEN               "6853517492:AAF0C3XLpF0qb-2vHPXHlgiEasl4dGBcoVg"    //Брать у https://t.me/BotFather
#define ADMIN_ID                "6632566922"                                        //Брать у https://t.me/getmyid_bot. Можно вписать ID чата

/*ПИНЫ*/
#define LED_PIN                 2           //Пин светодиода

/*ЗАДЕРЖКИ (MS)*/
#define BOT_MSG_DELAY           2000         //Задержка проверки сообщений (между ними)
#define LED_DELAY               1000         //Задержка мигания светодиодом
#define UPDATE_DELAY            2000         //Задержка опроса датчиков
#define BOT_ALLERT_DELAY        3000         //Задержка проверки и отправка алертов
#define REBOOT_DELAY            20000        //Задержка для перезагрузки. Если после перезагрузки через бота уходит в цикл - увеличиваем
#define PUB_DELAY               10000        //Задержка публикации на MQTT-сервер

/*ТОПИКИ*/
#define TEMP_TOPIC              "temperature"    //Топик для температуры
#define PPM_TOPIC               "ppm"            //Топик для ппм
#define PRES_TOPIC              "pressure"       //Топик для давления
#define HUM_TOPIC               "humidity"       //Топик для влажности

/*НАСТРОЙКИ БОТА*/
#define MAX_MESSAGE_LENGTH      100         //Максимальная длина сообщения

/*КРИТИЧЕСКИЕ ЗНАЧЕНИЯ*/
#define MAX_TEMP                24          //Максимальная температура, С
#define MIN_TEMP                18          //Минимальная температура, С
#define MAX_HYM                 60          //Максимальная влажность, %
#define MIN_HYM                 30          //Минимальная влажность, %
#define MAX_PPM                 1000        //Максимальная концентрация углекислого газа, ppm
#define MAX_PRES                765         //Максимальное давление, ММРС
#define MIN_PRES                755         //Минимальное давление, ММРС

    