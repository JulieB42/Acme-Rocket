/*
Base ESP-now Project:
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://randomnerdtutorials.com/esp-now-two-way-communication-esp32/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
Base code for LED matrix text:
https://forums.adafruit.com/viewtopic.php?t=115535

Additions and modifications by Julie and Paul Barrett
I used an ESP-32 C6

*/

#include <esp_now.h>
#include <WiFi.h>
int sound;
int launch;

  typedef struct struct_message {
  int buttonNew=0;
  int oops=0;
  int launch=0;
  int sound = 0;
  int trigger = 0;
} struct_message;
struct_message dataSent;
struct_message dataRcv;
struct_message incomingReadings;

//callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
  Serial.print("Bytes received: ");
  Serial.println(sizeof(incomingReadings));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("sound: ");
  Serial.println(incomingReadings.sound);
  Serial.print("trigger: ");

  Serial.println(incomingReadings.launch);
  Serial.println();
  if (incomingReadings.launch= 1) {
doSound();
delay(5000);//long enough for the sound to play before the rocket drops.
            //obviously we can play with delays in different places.
rocketDrop();
  }
}
void doSound(){
  Serial.println("sound function goes here");
}
void rocketDrop(){
  Serial.println("Solenoid trigger here");
}
void setup() {
  //Initialize Serial Monitor
  Serial.begin(115200);
  delay(3000);
  Serial.println("gantry ready");
  
  //Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  //Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));

}
void loop(){
//Serial.println("hi");
//delay(1000);
}