#include <ESP8266WiFi.h>

// -------- Define Pins -----------

//RGB Pins (All have a 1k resistor)
#define RGB_RED_PIN D7
#define RGB_BLUE_PIN D5
#define RGB_GREEN_PIN D6

//Button Pin (pulled down to ground)
#define BUTTON_PIN D0

// LDR Pin
// (3.3v) ----[LDR]-----[100K resistor]---- (GND)
//                   |
//  (A0) ------------
#define LDR_PIN A0

// -------- ---------- -----------

char ssid[] = "My Network Name";       // your network SSID (name)
char password[] = "hunter1";  // your network key

volatile bool buttonPressedFlag = false;
int onboardLedState = LOW; //LED is active low




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
  toggleOnBoardLED();
  buttonPressedFlag = false;
}

int checkLDR() {
  int value = analogRead(LDR_PIN);
  Serial.print("LDR value: ");
  Serial.println(value);
  return value;
}

void loop() {

  turnLEDRed();
  delay(1000);
  turnLEDGreen();
  delay(1000);
  turnLEDBlue();
  delay(1000);

  if ( buttonPressedFlag ) {
    handleButtonPressed();
  }

  if (checkLDR() < 600) {
    toggleOnBoardLED();
  }
  
  
}
