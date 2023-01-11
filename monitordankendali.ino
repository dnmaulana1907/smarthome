
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels



Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
// Initialize Wifi connection to the router
char ssid[] = "Jongkang";     // your network SSID (name)
char password[] = "Sate12345"; // your network key

// Initialize Telegram BOT
#define BOTtoken "2111047689:AAEsYw01ftnG9FWZdDXbWsubUfNJDcwY3gk"  // your Bot Token (Get from Botfather)
#define CHAT_ID "481755438"

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

int Bot_mtbs = 1000; //mean time between scan messages
long Bot_lasttime;   //last time messages' scan has been done
bool Start = true;

const int relayPin = 33;// Lamp
const int doorPin = 25;// Selenoid door lock

int doorState = 1;
int relayState = 1;

const int flameSensor = 39; // KY-026 Flame Sensor
bool flameState = 0;

const int gasSensor = 34; //MQ2 Gas Sensor
bool gasState = 0;

const int rangeSensor = 35;// HC-SR04 Ultrasonic Sensor
bool rangeState = 0;

const int ldrSensor = 32; // KY-018 Photoresistor module
bool ldrState = 0;

const int motionSensor = 15; // PIR Motion Sensor
bool motionDetected = false;

void IRAM_ATTR detectsMovement() {
  motionDetected = true;
}

void setup() {
  Serial.begin(115200);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Welcome to");
  display.println("Monitoring");
  display.println("Control");
  display.println("System");
  display.display();
  // Attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);

  // Set WiFi to station mode and disconnect from an AP if it was Previously
  // connected
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  pinMode(motionSensor, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(motionSensor), detectsMovement, RISING);

  pinMode(ldrSensor, INPUT);
  pinMode(flameSensor, INPUT);
  pinMode(gasSensor, INPUT);
  pinMode(rangeSensor, INPUT);
  pinMode(relayPin, OUTPUT);
  pinMode(doorPin, OUTPUT);
  delay(10);
  digitalWrite(doorPin, LOW); // initialize pin as off
  digitalWrite(relayPin, LOW);
  delay(2000);

}

void handleNewMessages(int numNewMessages) {
  //sw1ButtonState = !digitalRead(sw1Pin);
  //sw2ButtonState = !digitalRead(sw2Pin);
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;

    String from_name = bot.messages[i].from_name;
    if (from_name == "") from_name = "Guest";

    if (text == "/closedoor") {
      digitalWrite(doorPin, LOW);   // turn the LED on (HIGH is the voltage level)
      doorState = 0;
      bot.sendMessage(chat_id, "Door Locked", "");
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(0, 0);
      display.println("Door");
      display.println("Locked");
      display.display();
    }


    if (text == "/opendoor") {
      doorState = 0;
      digitalWrite(doorPin, HIGH);    // turn the LED off (LOW is the voltage level)
      bot.sendMessage(chat_id, "Door Opened", "");
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(0, 0);
      display.println("Door");
      display.println("Opened");
      display.display();
    }
    if (text == "/doorstate") {
      if (doorState) {
        bot.sendMessage(chat_id, "Door Locked Condition", "");
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(WHITE);
        display.setCursor(0, 0);
        display.println("Door");
        display.println("Closed");
        display.println("Condition ");
        display.display();
      } else {
        bot.sendMessage(chat_id, "Door Opened Condition", "");
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(WHITE);
        display.setCursor(0, 0);
        display.println("Door");
        display.println("Opened");
        display.println("Condition ");
        display.display();
      }
    }
    if (text == "/turnonlight") {
      digitalWrite(relayPin, LOW);   // Relay switch on in LOW condition
      doorState = 0;
      bot.sendMessage(chat_id, "Light On", "");
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(0, 0);
      display.println("Light ON");
      display.display();
    }

    if (text == "/turnofflight") {
      digitalWrite(relayPin, HIGH);   // Relay swich off in HIGH condition
      doorState = 1;
      bot.sendMessage(chat_id, "Light Off", "");
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(0, 0);
      display.println("Light Off");
      display.display();
    }


    if (text == "/start") {
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(0, 0);
      display.println("Welcome to");
      display.println("Monitoring");
      display.println("Control");
      display.println("System");

      display.display();
      String welcome = "Welcome to Monitoring and Controlling System " ".\n";
      welcome += "This a Bot for Monitoring and Controlling your house.\n\n";
      welcome += "/closedoor    : to lock the Door\n";
      welcome += "/opendoor     : to lock the Door\n";
      welcome += "/doorstate    : current status of Door\n";
      welcome += "/turnonlight : light on\n";
      welcome += "/turnofflight : light off\n";
      bot.sendMessage(chat_id, welcome, "Markdown");
    }
  }
}

void loop() {
  ldrState = digitalRead(ldrSensor);
  flameState = digitalRead(flameSensor);
  gasState = digitalRead(gasSensor);
  rangeState = digitalRead(rangeSensor);
  /*Serial.println(ldrState);
    Serial.print("status api =");
    Serial.print(flameState);
    Serial.println();
    Serial.print("status gas =");
    Serial.print(gasState);
    Serial.println();
    Serial.print("status jarak =");
    Serial.print(rangeState);
    Serial.println();
  */
  if (motionDetected) {
    bot.sendMessage(CHAT_ID, "Motion detected!!", "");
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("Motion");
    display.println("Detected");
    display.display();
    motionDetected = false;
  }

  if (ldrState == HIGH) {
    bot.sendMessage(CHAT_ID, "Your home on low light condition. click : /turnonlight if you want to turn light on", "");
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("Low Light");
    display.println("Condition");
    display.display();
    //delay(60*60*100);
  }

  if (flameState == HIGH) {

    bot.sendMessage(CHAT_ID, "Dangereous!!. Flame Detected", "");
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("Flame");
    display.println("Detected");
    display.display();
  }
  if (gasState == HIGH) {
    bot.sendMessage(CHAT_ID, "Dangereous!!. Methane Detected", "");
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("Methane");
    display.println("Detected");
    display.display();
  }

  if (rangeState == HIGH) {

    bot.sendMessage(CHAT_ID, "Dangereous!!. Flood Detected", "");
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("Flood");
    display.println("Detected");
    display.display();
  }

  if (millis() > Bot_lasttime + Bot_mtbs)  {

    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);


    }

    Bot_lasttime = millis();

  }
}
