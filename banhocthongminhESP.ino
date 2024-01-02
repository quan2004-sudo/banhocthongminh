/*************************************************************

  This is a simple demo of sending and receiving some data.
  Be sure to check out other examples!
 *************************************************************/

/* Fill-in information from Blynk Device Info here */

/// EEP rom
#define BLYNK_TEMPLATE_ID "TMPL6MB7gPyEC"
#define BLYNK_TEMPLATE_NAME "Quickstart Template"
#define BLYNK_AUTH_TOKEN "L4-Y7i0NuBM6HsgZWzmP2xVVMJ6vx1S-"
#define EEPROM_SIZE 12
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <WiFiManager.h>
#include <SoftwareSerial.h>

int lastBtn1State = HIGH;
int btn1State;
int addr = 0;
unsigned long btn1PressTime = 0;


const unsigned long longPressDuration = 1000;


uint32_t lasttime = 0;
/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial
String data[3];
SoftwareSerial mySerial(D6, D5);
// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "AmericanStudy T1";
char pass[] = "66668888";

BlynkTimer timer;
String oldTerminalValue;
WiFiManager wifiManager;
int gio1, phut1, gio, phut, mode, mute, mode1, mute_distance;

// This function is called every time the Virtual Pin 0 state changes
BLYNK_WRITE(V7) {
  // Set incoming value from pin V0 to a variable

  String TerminalValue = param.asStr();
  if (TerminalValue == "Set" || oldTerminalValue == "Set") {
    if (TerminalValue == "Set") { Blynk.virtualWrite(V7, "Nhập thời gian bạn muốn đặt báo thức."); }

    // Tìm vị trí của ký tự 'h' trong chuỗi
    int vi_tri_h = TerminalValue.indexOf('h');

    // Kiểm tra nếu ký tự 'h' được tìm thấy trong chuỗi
    if (vi_tri_h != -1) {
      // Tách phần trước 'h' thành giờ
      String gio_str = TerminalValue.substring(0, vi_tri_h);
      gio = gio_str.toInt();
      String phut_str = TerminalValue.substring(vi_tri_h + 1);
      phut = phut_str.toInt();

      EEPROM.put(0, gio);
      delay(10);
      EEPROM.put(1, phut);
      EEPROM.commit();
      // In giờ và phút
      if (oldTerminalValue == "Set" && isInt(gio) && isInt(phut)) {
        Blynk.virtualWrite(V7, "Đã cài nhắc nhở vào lúc " + TerminalValue);

        oldTerminalValue == "";
      }
    }
  } else if (TerminalValue == "Save") {
    Blynk.virtualWrite(V7, "Đã lưu thành công. ");

  } else if (TerminalValue == "TimeSet") {
    Blynk.virtualWrite(V7, "Giờ báo thức hiện tại là: " + String(gio1) + "h" + String(phut1) + ".");
  } else {
    Blynk.virtualWrite(V7, "Lỗi cú pháp.");
  }

  oldTerminalValue = TerminalValue;
  // Update state
  //Blynk.virtualWrite(V1, value);
}
BLYNK_WRITE(V8) {



  mode = param.asInt();
  delay(20);
}
BLYNK_WRITE(V9) {
  mute = param.asInt();
  if (mute == 1)
    Blynk.virtualWrite(V7, "Đã bật nhắc nhở.");
  else {
    Blynk.virtualWrite(V7, "Đã tắt nhắc nhở.");
  }
}
BLYNK_WRITE(V10) {
  mute_distance = param.asInt();
  if (mute_distance == 1)
    Blynk.virtualWrite(V7, "Đã bật cảnh báo khoảng cách.");
  else {
    Blynk.virtualWrite(V7, "Đã tắt cảnh báo khoảng cách.");
  }
}



// This function is called every time the device is connected to the Blynk.Cloud
BLYNK_CONNECTED() {
  // Change Web Link Button message to "Congratulations!"
  Blynk.virtualWrite(V7, "Lựa chọn mục tương ứng:");
  Blynk.virtualWrite(V7, "\n");
  Blynk.virtualWrite(V7, "Nhập 'Set' để cài đặt thời gian báo thức.");
  Blynk.virtualWrite(V7, "\n");
  Blynk.virtualWrite(V7, "Nhập 'Save' để lưu các thay đổi.");
  Blynk.virtualWrite(V7, "\n");
  Blynk.virtualWrite(V7, "Nhập 'TimeSet' để xem thời gian nhắc nhở hiện tại.");
}

// This function sends Arduino's uptime every second to Virtual Pin 2.
void myTimerEvent() {
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V6, millis() / 1000);
}

void setup() {
  // Debug console
  Serial.begin(9600);
  mySerial.begin(9600);
 wifiManager.autoConnect("Ban hoc thong minh");
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  // You can also specify server:
  //Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, "blynk.cloud", 80);
  //Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, IPAddress(192,168,1,100), 8080);
 
  // Setup a function to be called every second
  timer.setInterval(000L, myTimerEvent);
  EEPROM.begin(EEPROM_SIZE);
}

void loop() {
  Blynk.run();
  timer.run();

  EEPROM.get(0, gio1);
  EEPROM.get(1, phut1);

  // You can inject your own code or combine it with other sketches.
  // Check other examples on how to communicate with Blynk. Remember

  btn1State = mode;  // nhan tren 3s ==> doi mode // nhan duoi 3s doi che do xem man hinh

  if (btn1State != lastBtn1State) {
    if (btn1State == HIGH) {
      btn1PressTime = millis();
    } else {
      if (millis() - btn1PressTime > longPressDuration) {
        Serial.println("111111111111");
        mode1 = 1;
      }
    }
  }

  gio1 = EEPROM.read(0);
  delay(5);
  phut1 = EEPROM.read(1);


  // to avoid delay() function!
  Serial.print("Giờ: ");
  Serial.print(gio1);
  Serial.print("\t Phút: ");
  Serial.print(phut1);
  Serial.print("\t mode: ");
  Serial.print(mode);
  Serial.print("\t mute_distance : ");
  Serial.print(mute_distance);
  Serial.print("\t mute : ");
  Serial.println(mute);
  if (millis() - lasttime > 1000) {
    sendUart();
    Serial.println("done");
    lasttime = millis();
    mode1 = 0;
  }

  lastBtn1State = btn1State;
}

boolean isInt(float value) {
  return value == int(value);
}

void sendUart() {
  String data[5] = { String(gio1), String(phut1), String(mode1), String(mute), String(mute_distance) };


  String combinedData = "";

  // Kết hợp các dữ liệu vào chuỗi
  for (int i = 0; i < 5; i++) {
    combinedData += data[i];
    if (i < 4) {
      combinedData += ",";  //
    }
  }
  combinedData += "\n";

  // Gửi chuỗi dữ liệu qua UART
  mySerial.print(combinedData);
}
