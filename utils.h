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

#define DHTPIN 12     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11

// Buttons
#define BTN1_PIN 2
#define BTN2_PIN 3
#define BTN3_PIN 4
#define BTN4_PIN 5

int getOffset(char str[],int *width,int *textSize);