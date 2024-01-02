#include <SoftwareSerial.h>
#include <Wire.h>
#include <virtuabotixRTC.h>  //
#include <DFRobotDFPlayerMini.h>

#include <Wire.h>
#include "TSL2561.h"

// Example for demonstrating the TSL2561 library - public domain!

// connect SCL to analog 5
// connect SDA to analog 4
// connect VDD to 3.3V DC
// connect GROUND to common ground
// ADDR can be connected to ground, or vdd or left floating to change the i2c address

// The address will be different depending on whether you let
// the ADDR pin float (addr 0x39), or tie it to ground or vcc. In those cases
// use TSL2561_ADDR_LOW (0x29) or TSL2561_ADDR_HIGH (0x49) respectively
TSL2561 tsl(TSL2561_ADDR_FLOAT);


#define ECHO1 4
#define TRIG1 5
#define btn1 11

#define df_tx 6
#define df_rx 7
#define ESP_tx 2
#define ESP_rx 3
#define RPWM 8
#define LPWM 9

String mode = "0", mute = "1", mute_distance = "1";
bool check = true;

bool timeCheck = true;
int oldhour;
virtuabotixRTC myRTC(10, 12, 13);

int ledCount = 0;
uint16_t lux = 0, lastcheck;

SoftwareSerial softwareSerial(df_rx, df_tx);
SoftwareSerial mySerial(ESP_rx, ESP_tx);  //
DFRobotDFPlayerMini player;

int btn1State = LOW;


int lastBtn1State = LOW;


unsigned long btn1PressTime = 0;


const unsigned long longPressDuration = 2000;

String hoursSet = "0", minsSet = "0", data[5];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  mySerial.begin(9600);
  pinMode(ECHO1, INPUT);
  pinMode(TRIG1, OUTPUT);
  pinMode(RPWM, OUTPUT);
  pinMode(LPWM, OUTPUT);
  pinMode(btn1, INPUT_PULLUP);
  // seconds, minutes, hours, day of the week, day of the month, month, year
  //myRTC.setDS1302Time(0, 11, 19, 3, 26, 9, 2023);

  if (player.begin(softwareSerial)) {

    // Set volume to maximum (0 to 30).
    player.volume(30);
    // Play   the first MP3 file on the SD card
  }


  if (tsl.begin()) {
    Serial.println("Found sensor");
  } else {
    Serial.println("No sensor?");
    while (1)
      ;
  }

  // You can change the gain on the fly, to adapt to brighter/dimmer light situations
  //tsl.setGain(TSL2561_GAIN_0X);         // set no gain (for bright situtations)
  tsl.setGain(TSL2561_GAIN_16X);  // set 16x gain (for dim situations)

  // Changing the integration time gives you a longer time over which to sense light
  // longer timelines are slower, but are good in very low light situtations!
  tsl.setTiming(TSL2561_INTEGRATIONTIME_13MS);  // shortest integration time (bright light)
  //tsl.setTiming(TSL2561_INTEGRATIONTIME_101MS);  // medium integration time (medium light)
  //tsl.setTiming(TSL2561_INTEGRATIONTIME_402MS);  // longest integration time (dim light)

  // Now we're ready to get readings!
}

void loop() {
  // put your main code here, to run repeatedly:
  myRTC.updateTime();
  luxRead();
  ds1302_update();
  distance();


  int PWM = map(lux, 450, 360, 100, 255);  //  cần thay đổi giá trị lux và PWM
  if (PWM < 100) {
    PWM = 100;
  } else if (PWM > 255) {
    PWM = 255;
  }

  //
  btn1State = digitalRead(btn1);  // nhanduoi  3s ==> doi mode // nhan tren 3s

  if (btn1State != lastBtn1State) {
    if (btn1State == LOW) {
      btn1PressTime = millis();
    } else {
      if (millis() - btn1PressTime < longPressDuration) {
        // Xử lý btn 1 nhấn ngắn (< 3 giây)
        ledCount++;
        if (ledCount == 5) {
          ledCount = 0;
        }

      } else {
        // Xử lý btn 1 nhấn lâu (>= 3 giây) taat am thanh

        player.pause();
        // Serial.println("Caammmmmmm");
      }
    }
  }


  if (ledCount == 0) {  // led sang tu dong
     ledControl(0);
  } else if (ledCount == 1) {  // led sang muc1
    ledControl(100);
  } else if (ledCount == 2) {  // led sang muc2
    ledControl(180);
  } else if (ledCount == 3) {  // led sang muc3
    ledControl(255);
  } else {
   
    ledControl(PWM);
  }

  Serial.print(ledCount);
  Serial.print("   ");
  Serial.println(PWM);

  if (String(myRTC.hours) == hoursSet && String(myRTC.minutes) == minsSet && mute == "1") {
     music(2, 3000);  // nhac nho hoc tap
    //Serial.println("Dax bat bao thuc");
  } else {
    // Serial.println("Dax tat bao thuc");
  }




  nhan_dulieu();
  lastBtn1State = btn1State;
}

void music(int num, int time) {
  player.play(num);
  delay(time);
}

float get_distance1() {
  digitalWrite(TRIG1, LOW);
  delayMicroseconds(5);
  digitalWrite(TRIG1, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG1, LOW);
  float duration = pulseIn(ECHO1, HIGH);
  float distance = duration / 2 / 29.412;
  return distance;
}




void ds1302_update() {
  myRTC.updateTime();
  // Start printing elements as individuals
  // Serial.print("Current Date / Time: ");
  // Serial.print(myRTC.dayofmonth);
  // Serial.print("/");
  // Serial.print(myRTC.month);
  // Serial.print("/");
  // Serial.print(myRTC.year);
  // Serial.print("  ");
  // Serial.print(myRTC.hours);
  // Serial.print(":");
  // Serial.print(myRTC.minutes);
  // Serial.print(":");
  // Serial.println(myRTC.seconds);
  // Delay so the program doesn't print non-stop
}



void luxRead() {
  // Simple data read example. Just read the infrared, fullspecrtrum diode
  // or 'visible' (difference between the two) channels.
  // This can take 13-402 milliseconds! Uncomment whichever of the following you want to read
  uint16_t x = tsl.getLuminosity(TSL2561_VISIBLE);
  //uint16_t x = tsl.getLuminosity(TSL2561_FULLSPECTRUM);
  //uint16_t x = tsl.getLuminosity(TSL2561_INFRARED);

  //Serial.println(x, DEC);

  // More advanced data read example. Read 32 bits with top 16 bits IR, bottom 16 bits full spectrum
  // That way you can do whatever math and comparisons you want!
  uint32_t lum = tsl.getFullLuminosity();
  uint16_t ir, full;
  ir = lum >> 16;
  full = lum & 0xFFFF;
  lux = tsl.calculateLux(full, ir);
}


void distance() {
  if (get_distance1() < 30 && mute_distance == "1") {
      music(1, 3000);
    //  Serial.println("ngang dau len");
  }
}

void ledControl(int dosang) {
  analogWrite(LPWM, dosang);
  digitalWrite(RPWM, 0);
}

void nhan_dulieu() {
  if (mySerial.available()) {                              // Kiểm tra xem có dữ liệu đang được gửi đến không
    String receivedData = mySerial.readStringUntil('\n');  // Đọc chuỗi dữ liệu cho đến khi gặp ký tự '\n'

    // Tách dữ liệu bằng dấu phẩy

    int startIndex = 0;
    int commaIndex;

    for (int i = 0; i < 5; i++) {
      commaIndex = receivedData.indexOf(',', startIndex);
      if (commaIndex != -1) {
        data[i] = receivedData.substring(startIndex, commaIndex);
        startIndex = commaIndex + 1;
      } else {
        data[i] = receivedData.substring(startIndex);
        break;
      }
    }
    hoursSet = data[0];
    minsSet = data[1];
    mode = data[2];
    mute = data[3];
    mute_distance = data[4];
    // Serial.println("done");
    // Serial.print("ledCount ");
    // Serial.print(ledCount);

    // Serial.print("\t Lux: ");
    // Serial.print(lux);
    // Serial.print("\t mode ");
    // Serial.print(mode);
    // Serial.print("\t mute ");
    // Serial.print(mute);
    //     Serial.print("\t mute_distance ");
    // Serial.print(mute_distance);
    // Serial.print(" \t ");
    // Serial.print(hoursSet);
    // Serial.print(" h ");
    // Serial.println(minsSet);
    //delay(100);

    if (mode == "1") {  // nút dao mode tu blynk
      ledCount++;
      if (ledCount == 5) {
        ledCount = 0;
      }
    }
  }
}
