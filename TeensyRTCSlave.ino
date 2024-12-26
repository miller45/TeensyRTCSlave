// i2c teensy rtc slave

#define  SLAVE_ADDRESS        0x68  //slave address for DS1307
#define  REG_MAP_SIZE         10
#define  MAX_SENT_BYTES       3
#define TIME_BYTES_SIZE 7

/********* Global  Variables  ***********/
byte timeBytes[TIME_BYTES_SIZE];
byte registerMap[REG_MAP_SIZE];

byte registerMapTemp[REG_MAP_SIZE - 1];

byte receivedCommands[MAX_SENT_BYTES];

#include <Wire.h>
#include <RTCMisc.h>
#include <TimeLib.h>

void setup() {

  //Setup clock
  setSyncProvider(getTeensy3Time);

  Serial.begin(9600);
  while (!Serial);  // Wait for Arduino Serial Monitor to open
  delay(1000);
  if (timeStatus() != timeSet) {
    Serial.println("Unable to sync with the RTC");
  } else {
    Serial.println("RTC has set the system time");
    digitalClockDisplay();
  }

  Wire.begin(SLAVE_ADDRESS);
  timeBytes[0] = 83;
  timeBytes[1] = 51;
  timeBytes[2] = 34;
  timeBytes[3] = 0;
  timeBytes[4] = 20;
  timeBytes[5] = 1;
  timeBytes[6] = 52;
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);
  pinMode(13, OUTPUT);
}

boolean flip=false;

void loop() {
  long mill = millis();
  int m100slc = mill % 1000;
  if (m100slc == 0) {
    ////    PutTimeToRegs();
    //digitalClockDisplay();
    flip=!flip;
    //digitalWrite(13,flip);
  }
  delay(1);
}

void requestEvent()
{
  nowTimeToTimeBytes();
  Wire.write(timeBytes, TIME_BYTES_SIZE);  //Set the buffer up to send all x bytes of data
}

void receiveEvent(int bytesReceived)
{
  byte acmd = Wire.read();
  switch (acmd) {
    case 0:
      fakeAdjust();
      break;
    case 1: //led
      opLED();
      break;
    default:
      break;
  }
 

}
//I2C ops
void fakeAdjust() {
  while (Wire.available()) {
    //fake adjust
    Wire.read();
  }
}

void opLED() {
  if (Wire.available()) {
    byte onoff = Wire.read();
    digitalWrite(13,onoff);
  }
}



uint8_t xbin2bcd (uint8_t val) {
  return val + 6 * (val / 10);
}

void nowTimeToTimeBytes() {
  time_t atime = now();
  timeBytes[0] = xbin2bcd(second(atime));
  timeBytes[1] = xbin2bcd(minute(atime));
  timeBytes[2] = xbin2bcd(hour(atime));
  timeBytes[3] = xbin2bcd(0);
  timeBytes[4] = xbin2bcd(day(atime));
  timeBytes[5] = xbin2bcd(month(atime));
  timeBytes[6] = xbin2bcd(year(atime) - 2000);
}
//Teensy TIme

time_t getTeensy3Time()
{
  return Teensy3Clock.get();
}

void digitalClockDisplay() {
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year());
  Serial.println();
}


void printDigits(int digits) {
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}




