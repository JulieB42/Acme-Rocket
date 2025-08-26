#include <esp_now.h>
#include <WiFi.h>
//#include <Wire.h>
//Code for big red button to begin the sequence.
//Code by Julie and Paul Barrett
//Using a Xaio ESP-32 C-3

#define buttonPin D4

const char nom[10] = "button";
// REPLACE WITH YOUR RECEIVER MAC Address
uint8_t broadcastAddress[] = { 0x98, 0x3D, 0xAE, 0xAC, 0xF4, 0x98 };  //seeed C3 countdown board

int buttonNew;
int buttonOld;
int buttonPress;


// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {

  int buttonPress;

} struct_message;

// Create a struct_message called myData
struct_message dataSent;
struct_message dataRcv;

esp_now_peer_info_t peerInfo;
// Variable to store if sending data was successful

// Callback function called when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(115200);
  Serial.println("button serial ready");
  // initialize the pushbutton pin as an pull-up input
  // the pull-up input pin will be HIGH when the switch is open and LOW when the switch is closed.
  pinMode(buttonPin, INPUT_PULLUP);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
}
void loop() {
  // Press the button then reset it so the button press
  // isn't sent over and over in the loop.


  buttonOld = buttonNew;
  buttonNew = digitalRead(buttonPin);
  if (buttonOld == 1 && buttonNew == 0) {
    Serial.println("button pressed!");

    if (buttonNew == 0) {
      buttonNew = 1;
      Serial.println("buttonNew ");
      Serial.println(buttonNew);
    } else {

      Serial.println("ButtonNew!");
      Serial.println(buttonNew);
    }
    delay(1000);
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&dataSent, sizeof(dataSent));
  }
}