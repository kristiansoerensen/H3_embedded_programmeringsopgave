/* Simpel demo af OLED LCD Display med I2C */
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "RTClib.h"
#include "DHT.h"

#define DHTPIN 2     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11
// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE);

// ######## OLED ########
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET 4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
// ###### OLED end ######

// ######## RTC ########
RTC_DS3231 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"}; // Day of the week, that matches the index from the clock module
// ###### RTC end ######


// ## Other variables ##
String myStr = "Hello, world!";

DateTime currentTime; // Stores the time read from the clock module

float tempC;
float humidity;

int updateDatetimeDelay = 1000; // milli Seconds
unsigned int lastDateTimeUpdate = 0;

int updateDisplayDelay = 1000; // milli Seconds
unsigned int lastDisplayUpdate = 0;

int updateDHTDelay = 1000; // milli Seconds
unsigned int lastDHTUpdate = 0;

int updateDisplayStateDelay = 3000; // milli Seconds
unsigned int lastDisplayStateUpdate = 0;
bool stateDisplay = false;

void setup() 
{
	Serial.begin(9600);

  setupDisplay();

  setupRTC();

  dht.begin();
}

void loop() 
{
  updateLastDateTime();
  // serialPrintDateTime();

  updateDHTData();

	updateDisplay();
}

void setupDisplay(){
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
	if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) 
	{ // Address for 128x64
		Serial.println(F("SSD1306 allocation failed"));
		for(;;); // Don't proceed, loop forever
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

void updateDisplay() {
  if ((millis() - lastDisplayStateUpdate) > updateDisplayStateDelay) {
    lastDisplayStateUpdate = millis();
    stateDisplay = !stateDisplay;
    Serial.println(stateDisplay);
  }
  if ((millis() - lastDisplayUpdate) > updateDisplayDelay) {
    lastDisplayUpdate = millis();
    if (stateDisplay) {
      displayDateTime();
    }
    else {
      displayDHTData();
    }
    // displayDateTime();

    // displayDHTData();
  }
}

void updateDHTData(){
  if ((millis() - lastDHTUpdate) > updateDHTDelay) {
    lastDHTUpdate = millis();
    tempC = dht.readTemperature();        // Read temperature
	  humidity = dht.readHumidity();           // Read humidity
  }
}

void updateLastDateTime() {
  if ((millis() - lastDateTimeUpdate) > updateDatetimeDelay) {
    lastDateTimeUpdate = millis();
    currentTime = rtc.now();
  }
}

void displayDHTData(){
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

void displayDateTime(){
  display.clearDisplay();
  char str[32];
  displayTextCenter("Date", 0, 0, 1);
  sprintf(str, "%02d/%02d/%02d", currentTime.year(), currentTime.month(), currentTime.day());
  displayTextCenter(str, 0, 10, 2);
  displayTextCenter("Time", 0, 35, 1);
  sprintf(str, "%02d:%02d:%02d",  currentTime.hour(), currentTime.minute(), currentTime.second());
  displayTextCenter(str, 0, 45, 2);
  display.display();
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

void serialPrintDateTime(){
  DateTime now = currentTime;

  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" (");
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
  Serial.print(") ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();

  Serial.print(" since midnight 1/1/1970 = ");
  Serial.print(now.unixtime());
  Serial.print("s = ");
  Serial.print(now.unixtime() / 86400L);
  Serial.println("d");

  // calculate a date which is 7 days, 12 hours, 30 minutes, 6 seconds into the future
  DateTime future (now + TimeSpan(7,12,30,6));

  Serial.print(" now + 7d + 12h + 30m + 6s: ");
  Serial.print(future.year(), DEC);
  Serial.print('/');
  Serial.print(future.month(), DEC);
  Serial.print('/');
  Serial.print(future.day(), DEC);
  Serial.print(' ');
  Serial.print(future.hour(), DEC);
  Serial.print(':');
  Serial.print(future.minute(), DEC);
  Serial.print(':');
  Serial.print(future.second(), DEC);
  Serial.println();

  Serial.print("Temperature: ");
  Serial.print(rtc.getTemperature());
  Serial.println(" C");

  Serial.println();
}