/* Simpel demo af OLED LCD Display med I2C */
#include <SPI.h>
#include <Wire.h>
// OLED module
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// Clock module
#include <virtuabotixRTC.h>
// DHT11 module
#include <dht.h>

// Creation of the Real Time Clock Object
virtuabotixRTC myRTC(6, 7, 8); // SCL SDA RST

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET 4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define DHT_PIN 12
dht DHT;                // Creates a DHT object
int dhtData;
float tempC;
float humidity;

int updateDatetimeDelay = 1000; // milli Seconds
unsigned int lastDateTimeUpdate = 0;

int updateDisplayDelay = 1000; // milli Seconds
unsigned int lastDisplayUpdate = 0;

int updateDHTDelay = 1000; // milli Seconds
unsigned int lastDHTUpdate = 0;

void setup() 
{
	Serial.begin(9600);

  setupDisplay();
  // myRTC.setDS1302Time(18, 10, 10, 7, 18, 9, 2020);
}

void loop() 
{
  updateLastDateTime();
  updateDHTData();
  updateDisplay();
  Serial.println((int)(millis()/1000));
}

void updateDHTData(){
  if ((millis() - lastDHTUpdate) > updateDHTDelay) {
    lastDHTUpdate = millis();
    dhtData = DHT.read11(DHT_PIN);
    tempC = DHT.temperature;        // Read temperature
	  humidity = DHT.humidity;           // Read humidity
    Serial.println(humidity);
  }
}

void updateLastDateTime() {
  if ((millis() - lastDateTimeUpdate) > updateDatetimeDelay) {
    lastDateTimeUpdate = millis();
    myRTC.updateTime();
  }
}

void updateDisplay() {
  if ((millis() - lastDisplayUpdate) > updateDisplayDelay) {
    lastDisplayUpdate = millis();
    display.clearDisplay();

    // displayDateTime();

    displayDHTData();

    display.display();
  }
}

void setupDisplay(){
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
}

void serialPrintDateTime(){
  Serial.print("Current Date / Time: ");
  Serial.print(myRTC.dayofmonth);
  Serial.print("/");
  Serial.print(myRTC.month);
  Serial.print("/");
  Serial.print(myRTC.year);
  Serial.print("  ");
  Serial.print(myRTC.hours);
  Serial.print(":");
  Serial.print(myRTC.minutes);
  Serial.print(":");
  Serial.println(myRTC.seconds);
}

void displayDHTData(){
  char str[20] = "";
  sprintf(str, "%d.%02dC - %d.%02d", (int)tempC, (int)(tempC*100)%100, (int)humidity, (int)(humidity*100)%100);
  Serial.println(str);
  displayTextCenter(str, 0, 0, 1);
}

void displayDateTime(){
  char str[32];
  sprintf(str, "%02d/%02d/%02d",myRTC.dayofmonth, myRTC.month, myRTC.year);
  displayTextCenter(str, 0, 0, 2);
  sprintf(str, "%02d:%02d:%02d",  myRTC.hours, myRTC.minutes, myRTC.seconds);
  displayTextCenter(str, 0, 20, 2);
}

void displayTextCenter(char str[], int x, int y, int textSize){
  display.setTextSize(textSize); // Normal 1:1 pixel scale
	display.setTextColor(WHITE); // Draw white text
  int offset = getOffset(str, &x, &textSize);
	display.setCursor(offset,y); // Start at top-left corner
	display.println(str);
}

int getOffset(char str[], int *width, int *textSize){
  // We fist calculate the midle of the area
  // then we take the text size and multiply the size by 6, because 1 = 6 pixels
  // when we have pixel by char, the we multiply the string length with the pixels
  // then we divide by 2, to get the middle of the word, and then we extract that
  // from the screen area, we extract the string middle, and get the offset.
  return ((SCREEN_WIDTH - *width) / 2 ) - (strlen(str)*(6*(*textSize)) / 2);
}