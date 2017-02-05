#include <UniversalTelegramBot.h>

#include <ESP8266WiFi.h>

//SSL Client
#include <WiFiClientSecure.h>

//#include <ArduinoJson.h>

// -------- Define Pins -----------

//RGB Pins (All have a 1k resistor)
#define RGB_RED_PIN D7
#define RGB_BLUE_PIN D0
#define RGB_GREEN_PIN D6

//Button Pin (pulled down to ground)
#define BUTTON_PIN D7

// LDR Pin
// (3.3v) ----[LDR]-----[100K resistor]---- (GND)
//                   |
//  (A0) ------------
#define LDR_PIN A0

// -------- ---------- -----------

// Initialize Telegram BOT
#define botToken "XXXXXXXX:AAHq8-PxnPuU0C_YYYYYYYYYYYYYYY"  // your Bot Token (Get from Botfather)
WiFiClientSecure client;
UniversalTelegramBot bot(botToken, client);
long checkTelegramDueTime;
int checkTelegramDelay = 1000; //  1000 (1 second)

// The person/group you want the alert messages to go to.
// You must start a chat with a bot first before it can send you messages
String defaultChatId = "-128380507"; //This can be got by using a bot called "myIdBot"

char ssid[] = "My Network Name";       // your network SSID (name)
char password[] = "hunter1";  // your network key

volatile bool buttonPressedFlag = false;
int onboardLedState = LOW; //LED is active low


long ldrDueTime;
int checkLDRDelay = 250; // 1000/4 (1/4 of a second)

void setup() {

  Serial.begin(115200);

  // Initialize the LED pins as an outputs
  pinMode(RGB_RED_PIN, OUTPUT); 
  pinMode(RGB_BLUE_PIN, OUTPUT); 
  pinMode(RGB_GREEN_PIN, OUTPUT);

  // Initlaze the button
  pinMode(BUTTON_PIN, INPUT);
  // Using onboard LED to demo button
  pinMode(LED_BUILTIN, OUTPUT);

  attachInterrupt(BUTTON_PIN, interuptButtonPressed, RISING);
  
  // Attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);

}

void interuptButtonPressed() {
  Serial.println("interuptBoostButton");
  int button = digitalRead(BUTTON_PIN);
  if(button == HIGH)
  {
    buttonPressedFlag = true;
  }
  return;
}

void turnLEDRed() {
  digitalWrite(RGB_RED_PIN, HIGH);
  digitalWrite(RGB_BLUE_PIN, LOW);
  digitalWrite(RGB_GREEN_PIN, LOW);
}

void turnLEDGreen() {
  digitalWrite(RGB_RED_PIN, LOW);
  digitalWrite(RGB_BLUE_PIN, LOW);
  digitalWrite(RGB_GREEN_PIN, HIGH);
}

void turnLEDBlue() {
  digitalWrite(RGB_RED_PIN, LOW);
  digitalWrite(RGB_BLUE_PIN, HIGH);
  digitalWrite(RGB_GREEN_PIN, LOW);
}

void toggleOnBoardLED() {
  if ( onboardLedState == HIGH ) {
    Serial.println("Turning LED on");
    onboardLedState = LOW;
    digitalWrite(LED_BUILTIN, LOW); //LED is active low
  } else {
    Serial.println("Turning LED off");
    onboardLedState = HIGH;
    digitalWrite(LED_BUILTIN, HIGH);
  }
}

void handleButtonPressed() {

  // Changed to send a Telegram message
  //toggleOnBoardLED();
  bot.sendMessage(defaultChatId, "Button was pressed!");
  buttonPressedFlag = false;
}

int checkLDR() {
  int value = analogRead(LDR_PIN);
  Serial.print("LDR value: ");
  Serial.println(value);
  return value;
}


// --- Telgram functions ---

void handleNewMessages(int numNewMessages) {
  for (int i=0; i<numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;

    String from_name = bot.messages[i].from_name;
    if (from_name == "") from_name = "Guest";

    if (text == "/red") {
      turnLEDRed();
      bot.sendMessage(chat_id, "LED is now RED");
    }

    if (text == "/blue") {
      turnLEDBlue();
      bot.sendMessage(chat_id, "LED is now BLUE");
    }

    if (text == "/green") {
      turnLEDGreen();
      bot.sendMessage(chat_id, "LED is now GREEN");
    }

    if (text == "/toggle") {
      toggleOnBoardLED();
      bot.sendMessage(chat_id, "Toggled onboard LED, status is now " + String(onboardLedState));
    }

    if (text == "/ldr") {
      int value = checkLDR();
      bot.sendMessage(chat_id, "LDR reading is " + String(value));
    }

    if (text == "/options") {
      String keyboardJson = "[[\"/red\", \"/green\", \"/blue\"],[\"/toggle\"],[\"/ldr\"]]";
      bot.sendMessageWithReplyKeyboard(chat_id, "Choose from one of the following options", "", keyboardJson, true);
    }

    if (text == "/start" || text == "/help") {
      String welcome = "Welcome to Universal Arduino Telegram Bot library, " + from_name + ".\n";
      welcome += "The following options are available.\n\n";
      welcome += "/red : to switch RBG LED red\n";
      welcome += "/green : to switch RBG LED green\n";
      welcome += "/blue : to switch RBG LED blue\n";
      welcome += "/toggle : to toggle onboard LED\n";
      welcome += "/ldr : to return the current value of the LDR\n";
      welcome += "/options : returns a custom reply keyboard\n";
      welcome += "/help : displays this message again\n";
      bot.sendMessage(chat_id, welcome, "Markdown");
    }
  }
}


// --- ----------------- ---

void loop() {
  if ( buttonPressedFlag ) {
    handleButtonPressed();
  }
  
  long now = millis();
  if(now >= checkTelegramDueTime) {
    Serial.println("---- Checking Telegram -----");
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while(numNewMessages) {
      Serial.println("Bot recieved a message");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    checkTelegramDueTime = now + checkTelegramDelay;
  }
  now = millis();
  if(now >= ldrDueTime) {
    int ldrValue = checkLDR();
    if (ldrValue < 600) {
     bot.sendMessage(defaultChatId, "Low LDR value detected: " + String(ldrValue));
    }
    ldrDueTime = now + checkLDRDelay;
  }
}
