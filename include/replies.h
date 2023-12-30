const char COMMAND_ERR[]                    PROGMEM = "Даної команди не існує, або сталася інша помилка! Напишіть /start для виклику меню!";

const char BIG_MSG_ERR[]                    PROGMEM = "Ваше повідомлення не було оброблене через його великий обсяг!";
const char BIG_MSG_IMG[]                    PROGMEM = "https://i.imgur.com/aGliUE2.jpg"; //link

const char REBOOT_MSG[]                     PROGMEM = "Ви дійсно хочете перезавантажити пристрій?";
const char REBOOTED_MSG[]                   PROGMEM = "Пристрій перезавантажено!";

const char YOU_ARE_NOT_ADMIN[]              PROGMEM = "Ви не є адміністратором! Ця команда Вам недоступна!";
const char INVALID_FORMAT[]                 PROGMEM = "Неправильний формат команди!";
const char INVALID_ID[]                     PROGMEM = "Неправильний ID користувача!";
const char SENDED_TO_USER[]                 PROGMEM = "Повідомлення відправлено до id";
const char SENDED_TO_ADMIN[]                PROGMEM = "<i>Скарга надіслана адміністратору.</i>";
const char UNKNOWN_ERR[]                    PROGMEM = "Сталася невідома помилка";
const char WELCOME_MENU[]                   PROGMEM = "Вітаємо в головному меню!";


const char STATUS_HEADER_STRING[]           PROGMEM = "<b>Інформація:</b>\n";
const char STATUS_TEMP_STRING[]             PROGMEM = "\n🌡️<b>Температура:</b> ";
const char STATUS_HUMIDITY_STRING[]         PROGMEM = "\n💧<b>Вологість:</b> ";
const char STATUS_PPM_STRING[]              PROGMEM = "\n💨<b>Концентрація СО2:</b> ";
const char STATUS_PRESSURE_STRING[]         PROGMEM = "\n⛅️<b>Атмосферний тиск:</b> ";

const char STATUS_MQTT_CONNECTED[]          PROGMEM = "🟢MQTT: <i>Онлайн</i>";
const char STATUS_MQTT_DISCONNECTED[]       PROGMEM = "🔴MQTT: <i>Офлайн</i>";

//const char STATUS_BAD_FOOTER_STRING         PROGMEM = "\n\n<i>Погода може зіпсуватися</i>";
//const char STATUS_GOOD_FOOTER_STRING        PROGMEM = "\n\n<i>Погода залишається стабільною</i>";

const char MSG_HIGH_TEMP[]                  PROGMEM = "\n⛅️<b>Температура перевалила за встановленний максимум!</b> ";
const char MSG_HIGH_HUMIDITY[]              PROGMEM = "\n⛅️<b>Вологість перевалила за встановленний максимум!</b> ";
const char MSG_HIGH_PPM[]                   PROGMEM = "\n⛅️<b>Концентрація вуглекислого газу перевалила за встановленний максимум!</b> ";
const char MSG_HIGH_PRESSURE[]              PROGMEM = "\n⛅️<b>Атмосферний тиск перевалив за встановленний максимум!</b> ";

const char MSG_LOW_TEMP[]                   PROGMEM = "\n⛅️<b>Температура впала нижче встановленного мінімуму!</b> ";
const char MSG_LOW_HUMIDITY[]               PROGMEM = "\n⛅️<b>Вологість впала нижче встановленного мінімуму!</b> ";
const char MSG_LOW_PPM[]                    PROGMEM = "\n⛅️<b>Концентрація вуглекислого газу впала нижче встановленного мінімуму!</b> ";
const char MSG_LOW_PRESSURE[]               PROGMEM = "\n⛅️<b>Атмосферний тиск впав нижче встановленного мінімуму!</b> ";

const char MSG_FROM_ADMIN[]                 PROGMEM = "\n<b>Повідомлення від адміну:</b>";
