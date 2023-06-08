#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EasyButton.h>
#include "RTClib.h"
#include "DHT.h"

// ######## OLED ########
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET 5 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define BTN1_PIN 2
#define BTN2_PIN 3
#define BTN3_PIN 4
#define BTN4_PIN 5

EasyButton btn1(BTN1_PIN);
EasyButton btn2(BTN2_PIN);
EasyButton btn3(BTN3_PIN);
EasyButton btn4(BTN4_PIN);

bool btn1Pressed = false;
bool btn2Pressed = false;
bool btn3Pressed = false;
bool btn4Pressed = false;

// Callback.
void onbtn1Pressed() {
  btn1Pressed = true;
}
void onbtn2Pressed() {
  btn2Pressed = true;
}
void onbtn3Pressed() {
  btn3Pressed = true;
}
void onbtn4Pressed() {
  btn4Pressed = true;
}

int menuCounter = 0;
bool inMainMenu = true;
bool inClock = false;
bool inDHT11 = false;
bool inDHT11AndClock = false;
bool inAdjustClock = false;

RTC_DS3231 rtc;
int updateDatetimeDelay = 1000; // milli Seconds
unsigned int lastDateTimeUpdate = 0;
DateTime currentTime; // Stores the time read from the clock module

#define DHTPIN 12     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11
// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE);
int updateDHTDelay = 1000; // milli Seconds
unsigned int lastDHTUpdate = 0;
float tempC;
float humidity;

// Chenge between DHT11 and Clock
int updateDisplayStateDelay = 3000; // milli Seconds
unsigned int lastDisplayStateUpdate = 0;
bool stateDisplay = false;

int updateDisplayDelay = 1000; // milli Seconds
unsigned int lastDisplayUpdate = 0;

char *adjustTimeFields[] = {"Day", "Month", "Year", "Hour", "Minutes", "Seconds"};
int adjustTimeValues[] = {1, 1, 2023, 0, 0, 0};
int clockFieldSelected = 0;

void setup() {
  setupDisplay();

  // Setup Serial Monitor
	Serial.begin(9600);
  btn1.begin();
  btn2.begin();
  btn3.begin();
  btn4.begin();

  btn1.onPressed(onbtn1Pressed);
  btn2.onPressed(onbtn2Pressed);
  btn3.onPressed(onbtn3Pressed);
  btn4.onPressed(onbtn4Pressed);

  setupRTC();
  dht.begin();
}

void loop() {
  btn1.read();
  btn2.read();
  btn3.read();
  btn4.read();

  updateLastDateTime();
  updateDHTData();

  // put your main code here, to run repeatedly:
  if (inMainMenu){
    displayMainManu();
  }
  else if (inClock){
    displayClock();
  }
  else if (inDHT11){
    displayDHT11();
  }
  else if (inDHT11AndClock){
    displayDHT11AndClock();
  }
  else if (inAdjustClock){
    displayAdjustClock();
  }
}

void setupRTC(){
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  // lostPowerAdjustRTCTime();
  // rtc.adjust(DateTime(2023, 6, 7, 13, 4, 0));
}

void updateLastDateTime() {
  if ((millis() - lastDateTimeUpdate) > updateDatetimeDelay) {
    lastDateTimeUpdate = millis();
    currentTime = rtc.now();
  }
}

void updateDHTData(){
  if ((millis() - lastDHTUpdate) > updateDHTDelay) {
    lastDHTUpdate = millis();
    tempC = dht.readTemperature();        // Read temperature
	  humidity = dht.readHumidity();           // Read humidity
  }
}

void displayClock() {
  checkbtnClock();
  display.clearDisplay();
  char str[32];
  displayTextCenter("Date", 0, 0, 1);
  sprintf(str, "%02d/%02d/%02d", currentTime.day(), currentTime.month(), currentTime.year());
  displayTextCenter(str, 0, 10, 2);
  displayTextCenter("Time", 0, 35, 1);
  sprintf(str, "%02d:%02d:%02d",  currentTime.hour(), currentTime.minute(), currentTime.second());
  displayTextCenter(str, 0, 45, 2);
  display.display();
}

void checkbtnClock(){
  if (btn1Pressed)
  {
    Serial.println("Button1 was pressed");
    inClock = false;
    inMainMenu = true;
    btn1Pressed = false;
  }
  if (btn2Pressed)
  {
    Serial.println("Button2 was pressed");
    btn2Pressed = false;
  }
  if (btn3Pressed)
  {
    Serial.println("Button3 was pressed");
    btn3Pressed = false;
  }
  if (btn4Pressed)
  {
    Serial.println("Button4 was pressed");
    btn4Pressed = false;
  }
}

void displayDHT11() {
  checkbtnDHT11();
  display.clearDisplay();
  char str[] = "";
  displayTextCenter("Temperature", 0, 0, 1);
  sprintf(str, "%d,%02d", (int)tempC, (int)(tempC*100)%100);
  displayTextCenter(str, 0, 10, 2);
  // Draw the degree symbol, by getting the current cursor position
  display.drawCircle(display.getCursorX()+3, display.getCursorY(), 2, WHITE);
  display.setCursor(display.getCursorX()+6,display.getCursorY());
  display.print("C");

  displayTextCenter("Humidity", 0, 35, 1);
  sprintf(str, "%d", (int)humidity);
  Serial.println(str);
  displayTextCenter(str, 0, 45, 2);
  display.print('%');
  display.display();
}

void checkbtnDHT11(){
  if (btn1Pressed)
  {
    Serial.println("Button1 was pressed");
    inDHT11 = false;
    inMainMenu = true;
    btn1Pressed = false;
  }
  if (btn2Pressed)
  {
    Serial.println("Button2 was pressed");
    btn2Pressed = false;
  }
  if (btn3Pressed)
  {
    Serial.println("Button3 was pressed");
    btn3Pressed = false;
  }
  if (btn4Pressed)
  {
    Serial.println("Button4 was pressed");
    btn4Pressed = false;
  }
}
void displayDHT11AndClock() {
  DHT11AndClock();
  if ((millis() - lastDisplayStateUpdate) > updateDisplayStateDelay) {
    lastDisplayStateUpdate = millis();
    stateDisplay = !stateDisplay;
    Serial.println(stateDisplay);
  }
  if ((millis() - lastDisplayUpdate) > updateDisplayDelay) {
    lastDisplayUpdate = millis();
    if (stateDisplay) {
      displayClock();
    }
    else {
      displayDHT11();
    }
  }
}

void DHT11AndClock(){
  if (btn1Pressed)
  {
    Serial.println("Button1 was pressed");
    inDHT11AndClock = false;
    inMainMenu = true;
    btn1Pressed = false;
  }
  if (btn2Pressed)
  {
    Serial.println("Button2 was pressed");
    btn2Pressed = false;
  }
  if (btn3Pressed)
  {
    Serial.println("Button3 was pressed");
    btn3Pressed = false;
  }
  if (btn4Pressed)
  {
    Serial.println("Button4 was pressed");
    btn4Pressed = false;
  }
}
void displayAdjustClock() {
  char str[32];
  checkAdjustClock();
  display.clearDisplay();
  display.setTextSize(1); // Normal 1:1 pixel scale
	display.setTextColor(WHITE); // Draw white text

  display.setCursor(0,0);
  display.print("Date:");
  display.setCursor(0,10);
  display.setTextSize(2);
  for (int i = 0; i < 3; i++){
    // if (i == clockFieldSelected){
    //   display.setTextColor(BLACK);
    // }
    // else {
    //   display.setTextColor(WHITE);
    // }
    sprintf(str, "%02d", adjustTimeValues[i]);
    display.print(str);
    if (i < 2){
      display.print("/");
    } 
  }

  display.setTextSize(1);
  display.setCursor(0,35);
  display.print("Time:");

  display.setTextSize(2);
  display.setCursor(0,45);

  for (int i = 3; i < 6; i++){
    // if (i == clockFieldSelected){
    //   display.setTextColor(BLACK);
    // }
    // else {
    //   display.setTextColor(WHITE);
    // }
    sprintf(str, "%02d", adjustTimeValues[i]);
    display.print(str);
    if (i < 5){
      display.print(":");
    } 
  }
  display.display();
}

void checkAdjustClock(){
  if (btn1Pressed)
  {
    Serial.println("Button1 was pressed");
    inAdjustClock = false;
    inMainMenu = true;
    btn1Pressed = false;
    rtc.adjust(DateTime(adjustTimeValues[2], adjustTimeValues[1], adjustTimeValues[0], adjustTimeValues[3], adjustTimeValues[4], adjustTimeValues[5]));
    clockFieldSelected = 0; // reset field
  }
  if (btn2Pressed)
  {
    clockFieldSelected++;
    if (clockFieldSelected > 5){
      clockFieldSelected=0;
    }
    Serial.println("Button2 was pressed");
    btn2Pressed = false;
  }
  if (btn3Pressed)
  {
    adjustTimeValues[clockFieldSelected] = adjustTimeValues[clockFieldSelected] - 1;
    // Year - just block!
    if (clockFieldSelected == 2 && adjustTimeValues[clockFieldSelected] < 0){
      adjustTimeValues[clockFieldSelected]=0;
    }
    // Month
    else if (clockFieldSelected == 1 && adjustTimeValues[clockFieldSelected] < 0) {
      adjustTimeValues[clockFieldSelected] = 12;
    }
    // Day
    else if (clockFieldSelected == 0 && adjustTimeValues[clockFieldSelected] < 0) {
      adjustTimeValues[clockFieldSelected] = 31;
    }
    // Hour
    else if (clockFieldSelected == 3 && adjustTimeValues[clockFieldSelected]  < 0) {
      adjustTimeValues[clockFieldSelected] = 23;
    }
    // min
    else if (clockFieldSelected == 4 && adjustTimeValues[clockFieldSelected]  < 0) {
      adjustTimeValues[clockFieldSelected] = 59;
    }
    // sec
    else if (clockFieldSelected == 5 && adjustTimeValues[clockFieldSelected]  < 0) {
      adjustTimeValues[clockFieldSelected] = 59;
    }
    Serial.println("Button3 was pressed");
    Serial.print("Counter value");
    Serial.println(adjustTimeValues[clockFieldSelected]);
    btn3Pressed = false;
  }
  if (btn4Pressed)
  {
    adjustTimeValues[clockFieldSelected] = adjustTimeValues[clockFieldSelected] + 1;
    // Month
    if (clockFieldSelected == 1 && adjustTimeValues[clockFieldSelected] > 12) {
      adjustTimeValues[clockFieldSelected] = 0;
    }
    // Day
    else if (clockFieldSelected == 0 && adjustTimeValues[clockFieldSelected] > 31) {
      adjustTimeValues[clockFieldSelected] = 0;
    }
    // Hour
    else if (clockFieldSelected == 3 && adjustTimeValues[clockFieldSelected] > 23) {
      adjustTimeValues[clockFieldSelected] = 0;
    }
    // min
    else if (clockFieldSelected == 4 && adjustTimeValues[clockFieldSelected] > 59) {
      adjustTimeValues[clockFieldSelected] = 0;
    }
    // sec
    else if (clockFieldSelected == 5 && adjustTimeValues[clockFieldSelected] > 59) {
      adjustTimeValues[clockFieldSelected] = 0;
    }
    Serial.println("Button4 was pressed");
    Serial.print("Counter value");
    Serial.println(adjustTimeValues[clockFieldSelected]);
    btn4Pressed = false;
  }
}

void setupDisplay(){
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
	if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) 
	{ // Address for 128x64
		Serial.println(F("SSD1306 allocation failed"));
		for(;;); // Don't proceed, loop forever
	}
}

void displayMainManu(){
  checkbtnMainMenu();
  display.clearDisplay();
  displayTextCenter("Main Menu", 0, 0, 2);

  display.setTextSize(1); // Normal 1:1 pixel scale
	display.setTextColor(WHITE); // Draw white text
  display.setCursor(10,20);
  display.print("Clock");
  display.setCursor(10,30);
  display.print("DHT11");
  display.setCursor(10,40);
  display.print("Clock & DHT11");
  display.setCursor(10,50);
  display.print("Adjust clock");

  display.setCursor(2, (menuCounter * 10) + 20);
  display.print(">");
  display.display();
}

void checkbtnMainMenu(){
  if (btn1Pressed)
  {
    Serial.println("Button1 was pressed");
    menuCounter++;
    if (menuCounter > 3){
      menuCounter=0;
    }
    btn1Pressed = false;
  }
  if (btn2Pressed)
  {
    Serial.println("Button2 was pressed");
    if (menuCounter == 0){
      inClock = true;
      inMainMenu = false;
    }
    else if (menuCounter == 1) {
      inDHT11 = true;
      inMainMenu = false;
    }
    else if (menuCounter == 2) {
      inDHT11AndClock = true;
      inMainMenu = false;
    }
    else if (menuCounter == 3) {
      inAdjustClock = true;
      inMainMenu = false;
    }
    else {
      inMainMenu = true;
    }
    btn2Pressed = false;
  }
  if (btn3Pressed)
  {
    Serial.println("Button3 was pressed");
    btn3Pressed = false;
  }
  if (btn4Pressed)
  {
    Serial.println("Button4 was pressed");
    btn4Pressed = false;
  }
}

void displayTextCenter(char str[], int x, int y, int textSize){
  display.setTextSize(textSize); // Normal 1:1 pixel scale
	display.setTextColor(WHITE); // Draw white text
  int offset = getOffset(str, &x, &textSize);
	display.setCursor(offset,y); // Start at top-left corner
	display.print(str);
}

int getOffset(char str[], int *width, int *textSize){
  // We fist calculate the midle of the area
  // then we take the text size and multiply the size by 6, because 1 = 6 pixels
  // when we have pixel by char, the we multiply the string length with the pixels
  // then we divide by 2, to get the middle of the word, and then we extract that
  // from the screen area, we extract the string middle, and get the offset.
  return ((SCREEN_WIDTH - *width) / 2 ) - (strlen(str)*(6*(*textSize)) / 2);
}