/*
 * Created by ArduinoGetStarted.com
 *
 * This example code is in the public domain
 *
 * Tutorial page: https://arduinogetstarted.com/tutorials/arduino-gps
 */

#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <stdio.h>

#include <SPI.h>
#include <SD.h>

 //int MOSI = 11;
 //int MISO = 12;
 int CLK = 13;
 int CS = 7; // (for MKRZero SD: SDCARD_SS_PIN)

const int RXPin = 3, TXPin = 4;
const uint32_t GPSBaud = 9600;  //Default baud of NEO-6M is 9600


TinyGPSPlus gps;                         // the TinyGPS++ object
SoftwareSerial gpsSerial(RXPin, TXPin);  // the serial interface to the GPS device
File myFile;

char FILENAME[20] = "gps.csv";



void setup() {

  Serial.begin(9600);
  gpsSerial.begin(GPSBaud);

  Serial.println(F("Arduino - GPS module"));
}


void writeToSd(String &csv){
  Serial.print("Initializing SD card...");
  if (!SD.begin(CS)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open(FILENAME, FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing to test.txt...");
    myFile.println(csv);
    // close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
}

void readFromSd(){
  if (!SD.begin(CS)) {
    Serial.println("initialization failed!");
    while (1);
  }
    // re-open the file for reading:
  myFile = SD.open(FILENAME);
  if (myFile) {
    Serial.println(FILENAME);

    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
}

String gpsTimeString(){
  // 2023-10-17T16:55:18Z
  char str[80];
  sprintf(str, "%d-%d-%dT%d:%d:%dZ", gps.date.year(), gps.date.month(), gps.date.day(), gps.time.hour(), gps.time.minute(), gps.time.second());
  return String(str);
}

String gpsAsCsv(){
  String comma = String(",");
  return String(gps.location.lat(), 8) + comma + 
    String(gps.location.lng(), 8) + comma +
    String(gps.altitude.meters()) + comma + 
  
    String(gps.satellites.value()) + comma +
    String(gpsTimeString()) + comma
    ;
}

void loop() {
  bool write = true;
  bool read = false;
  if (gpsSerial.available() > 0) {
    char r = gpsSerial.read();

    if (gps.encode(r)) {
      
      if (gps.location.isValid()) {
        String csvText = gpsAsCsv();
        //Serial.println(csvText);
        if (write)
          writeToSd(csvText);
        if (read)
          readFromSd();
        delay(2000);
      }
    }
  }
}
