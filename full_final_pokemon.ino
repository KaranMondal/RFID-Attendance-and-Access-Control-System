// ---------------- Include Libraries ----------------
#include <WiFi.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <MFRC522.h>
#include "time.h"
#include <HTTPClient.h>

// ---------------- WiFi ----------------
char ssid[] = "Your SSID";
char pass[] = "Your PASS";

// ---------------- NTP / IST ----------------
const char* ntpServer = "time.google.com";
const long  gmtOffset_sec = 5*3600 + 1800;
const int daylightOffset_sec = 0;

// ---------------- OLED ----------------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_SDA 21
#define OLED_SCL 22
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ---------------- RFID ----------------
#define RST_PIN 4
#define SS_PIN 15
MFRC522 mfrc522(SS_PIN, RST_PIN);

// ---------------- Ultrasonic ----------------
#define TRIG_PIN 5
#define ECHO_PIN 18
#define PRESENCE_DISTANCE 50  // cm threshold for user presence

// ---------------- LEDs ----------------
#define LED_BLUE 25
#define LED_GREEN 26
#define LED_RED 27
#define LED_YELLOW 32

unsigned long previousMillis = 0;
const long heartbeatInterval = 1000; // 1-second blink for yellow LED

// ---------------- MongoDB REST API ----------------
const char* serverURL = "MONGO API";

// ---------------- Users ----------------
struct User {
  byte uid[10];
  byte uidSize;
  const char* name;
  bool checkedIn;
};

User users[] = {
  {{0xA3,0x84,0x72,0x0D}, 4, "Nitish Singh", false},
  {{0x23,0x05,0xDE,0x11}, 4, "Karan Mondal", false}
};
const int NUM_USERS = sizeof(users)/sizeof(users[0]);

// ---------------- Setup ----------------
void setup() {
  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);

  Wire.begin(OLED_SDA, OLED_SCL);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.display();

  SPI.begin(14,12,13,SS_PIN);
  mfrc522.PCD_Init();

  // ---------------- Wi-Fi ----------------
  WiFi.begin(ssid, pass);
  Serial.print("Connecting to WiFi");
  int retryCount = 0;
  while (WiFi.status() != WL_CONNECTED && retryCount < 20) {
    delay(500);
    Serial.print(".");
    retryCount++;
  }
  if(WiFi.status() == WL_CONNECTED){
    Serial.println("WiFi connected");
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  } else {
    Serial.println("WiFi connection failed!");
  }

  displayDefault();  // Show "4-Pokemons" on startup
}

// ---------------- Loop ----------------
void loop() {
  // Heartbeat LED
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis >= heartbeatInterval){
    previousMillis = currentMillis;
    digitalWrite(LED_YELLOW, !digitalRead(LED_YELLOW)); // Toggle heartbeat
  }

  long distance = getDistance();

  // Presence LED
  if(distance > 0 && distance <= PRESENCE_DISTANCE) digitalWrite(LED_BLUE, HIGH);
  else digitalWrite(LED_BLUE, LOW);

  if(distance > 0 && distance <= PRESENCE_DISTANCE) { // User detected
    displayMessage("Approach RFID", "to Check-In/Out");

    // Check for RFID card
    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
      int userIndex = checkUID(mfrc522.uid.uidByte, mfrc522.uid.size);
      if(userIndex >= 0){
        handleUser(userIndex, distance);
        digitalWrite(LED_GREEN, HIGH); // Success LED
        delay(500);
        digitalWrite(LED_GREEN, LOW);
      } else {
        displayMessage("RFID Error","Try Again");
        digitalWrite(LED_RED, HIGH); // Error LED
        delay(500);
        digitalWrite(LED_RED, LOW);
      }
      mfrc522.PICC_HaltA();
      delay(2000);
    }
  } else {
    displayDefault();  // Default display
  }

  delay(200); // Stabilize readings
}

// ---------------- Functions ----------------
long getDistance() {
  digitalWrite(TRIG_PIN, LOW); delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH, 20000);
  long distance = (duration * 0.0343)/2;
  if(duration==0) return -1;
  return distance;
}

int checkUID(byte* uid, byte size) {
  for(int i=0;i<NUM_USERS;i++){
    if(size==users[i].uidSize){
      bool match=true;
      for(int j=0;j<size;j++) if(uid[j]!=users[i].uid[j]) match=false;
      if(match) return i;
    }
  }
  return -1;
}

void handleUser(int index, long distance){
  String action;
  if(!users[index].checkedIn){ users[index].checkedIn=true; action="Check-In"; }
  else { users[index].checkedIn=false; action="Check-Out"; }

  // IST Time
  struct tm timeinfo;
  int timeRetry = 0;
  while(!getLocalTime(&timeinfo) && timeRetry < 5){
    Serial.println("Waiting for NTP time...");
    delay(1000);
    timeRetry++;
  }
  char timeStr[16];
  if(getLocalTime(&timeinfo)) strftime(timeStr,sizeof(timeStr),"%H:%M:%S",&timeinfo);
  else strcpy(timeStr,"00:00:00");

  // Serial debug
  Serial.println("------ User Scan ------");
  Serial.print("User: "); Serial.println(users[index].name);
  Serial.print("Action: "); Serial.println(action);
  Serial.print("Time: "); Serial.println(timeStr);
  Serial.print("Distance: "); Serial.println(distance);
  Serial.println("-----------------------");

  // OLED
  displayUserMessage(action, users[index].name, String(timeStr));

  // Send to MongoDB REST API
  sendToMongo(users[index].name, action, String(timeStr), distance);
}

void displayMessage(String line1,String line2){
  display.clearDisplay();
  display.setTextSize(2); display.setCursor(0,10); display.println(line1);
  display.setTextSize(1); display.setCursor(0,40); display.println(line2);
  display.display();
}

void displayUserMessage(String action,String user,String timeStr){
  display.clearDisplay();
  display.setTextSize(1); display.setCursor(0,0); display.println(action);
  display.setTextSize(2); display.setCursor(0,15); display.println(user);
  display.setTextSize(1); display.setCursor(0,45); display.print("Time: "); display.println(timeStr);
  display.display();
}

void displayDefault(){
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0,0);
  display.println("");
  display.display();
}

void sendToMongo(String user,String action,String time,long distance){
  if(WiFi.status()!=WL_CONNECTED){
    Serial.println("WiFi not connected, cannot send data to MongoDB");
    return;
  }

  const int maxRetries = 3;
  int attempt = 0;
  bool sent = false;

  while(attempt < maxRetries && !sent){
    HTTPClient http;
    Serial.print("Attempt "); Serial.print(attempt+1); 
    Serial.print(" connecting to: "); Serial.println(serverURL);
    http.begin(serverURL);
    http.addHeader("Content-Type","application/json");
    String payload="{\"user\":\""+user+"\",\"action\":\""+action+"\",\"time\":\""+time+"\",\"distance\":"+String(distance)+"}";
    int code = http.POST(payload);

    if(code>0){
      Serial.println("Data sent to MongoDB successfully");
      sent = true;
    } else {
      Serial.print("Error sending data (HTTP code): "); Serial.println(code);
      Serial.println("Retrying in 2 seconds...");
      delay(2000);
      attempt++;
    }

    http.end();
  }

  if(!sent){
    Serial.println("Failed to send data after 3 attempts. Data lost.");
  }
}
