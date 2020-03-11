// BAJA TEST CODE REV. 7: 11/MAR/2020
// CONTAINS GPS, SD, AND LCD TEST CODE
// Testing LCDs - Juan 11MAR
#define gpsPort Serial1
#define GPS_PORT_NAME "Serial1"
#define DEBUG_PORT Serial

// LIBRARY INCLUSION
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <SD.h>
#include <NMEAGPS.h>

// INITIALIZING VARIABLES

//    GPS Variables
const int gpsBaud = 9600;
NMEAGPS gps;
gps_fix fix;

//    SD Variables
File dataFile;

//    LCD Variables
LiquidCrystal_I2C lcd1 = LiquidCrystal_I2C(0x27, 16, 2);
LiquidCrystal_I2C lcd2 = LiquidCrystal_I2C(0x26, 16, 2); // CHANGE PORT FOR SECOND LCD

// SETUP
void setup() {

//    SD CARD SETUP
  DEBUG_PORT.begin(9600);
  if (!SD.begin(53)) {
    DEBUG_PORT.println("SD DOES NOT WORK!");
  } else {
    DEBUG_PORT.println("SD works!");
  }
  dataFile = SD.open("dataFile.txt", FILE_WRITE);
  if (dataFile) {
    DEBUG_PORT.println("Writing!");
    dataFile.println("FixIndex, Latitude, Longitude, Speed, Heading, DDMMYYYYHHMMSS");
    dataFile.close();
    DEBUG_PORT.println("Done!");
  } else {
    DEBUG_PORT.println("Error opening file!");
  }

  // GPS SETUP
  gpsPort.begin(gpsBaud);
  // LCD SETUP
  lcd1.init();
  lcd1.backlight();
  lcd1.setCursor(2,0);  lcd1.print("ROUGH RIDER");
  lcd1.setCursor(5,1);  lcd1.print("HUD 1");
  lcd2.init();
  lcd2.backlight();
  lcd2.setCursor(2,0);  lcd2.print("ROUGH RIDER");
  lcd2.setCursor(5,1);  lcd2.print("HUD 2");
  DEBUG_PORT.print("Should have printed!");
  lcd1.clear();
  lcd2.clear();
}

//  FUNCTIONS

//    GPSWrite: Writes GPS data to sd card and outputs relevant data to LCD monitors

void GPSWrite() {
  int i = 0;
  while (gps.available(gpsPort)) {
    fix = gps.read();
    dataFile = SD.open("dataFile.txt", FILE_WRITE);
    dataFile.print(i); // print fix index

    if (fix.valid.location) {
      dataFile.print(',');
      dataFile.print(fix.latitude(), 6 );
      dataFile.print(',' );
      dataFile.print(fix.longitude(), 6 );
    }

    if (fix.valid.speed) {
      dataFile.print(',');
      dataFile.print(fix.speed_kph());
      lcd1.setCursor(0,0); lcd1.print("SPEED: ");
      lcd1.setCursor(7,0); lcd1.print(round(fix.speed_kph()*100)/100);
    }

    if (fix.valid.heading) {
      dataFile.print(',');
      dataFile.print(fix.heading());
    }

    if (fix.valid.date) {
      dataFile.print(','));
      dataFile.print(fix.dateTime.day); dataFile.print(fix.dateTime.month); dataFile.print(fix.dateTime.year); 
      dataFile.print(fix.dateTime.hours); dataFile.print(fix.dateTime.minutes); dataFile.println(fix.dateTime.seconds); 
      lcd1.setCursor(0,1);  lcd1.print("TIME: ");
      lcd1.setCursor(6,1);  lcd1.print(fix.dateTime.hours); 
      lcd1.setCursor(8,1);  lcd1.print(":");
      lcd1.setCursor(9,1);  lcd1.print(fix.dateTime.minutes); 
      lcd1.setCursor(11,1); lcd1.print(":");
      lcd1.setCursor(12,1); lcd1.print(fix.dateTime.seconds); 
    }
    dataFile.close();
    i++;
  }
}

//  LOOP: MAIN FUNCTIONS

void loop() {
  GPSWrite();
  dataFile = SD.open("newFile.txt", FILE_WRITE);
  dataFile.println(millis());
  dataFile.close();
  delay(10);
}