/*
  ESP-Now starting-point code from:
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp-now-esp32-arduino-ide/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/

//Code by Julie and Paul Barrett

// This version running on a Seeed Studio Xiao ESP32S3
// When it receives a launch message or test-button, it runs a fire-simulation on a neopixel ring for several seconds.

#include <FastLED.h>
#include <esp_now.h>
#include <WiFi.h>
#define LED_PIN     D0
#define TST_PIN     D1
#define COLOR_ORDER GRB
#define CHIPSET     WS2811
#define NUM_LEDS    9

#define BRIGHTNESS  200
#define FRAMES_PER_SECOND 60
CRGB leds[NUM_LEDS];
bool gReverseDirection = false;
bool bTrigger = false;

//address of receiver
uint8_t broadcastAddress[] = {0x30, 0xC6, 0xF7, 0x00, 0x4F, 0x44};

typedef struct struct_message {
  int launch;
//int buttonNew = 0;  //I don't think I need this one. We'll see.
  int oops;
} struct_message;

//message for incoming data
struct_message incomingReadings;

//message for outgoing data
//struct_message DataToSend;

esp_now_peer_info_t peerInfo;

void Fire2012();

// Callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
  Serial.printf("Bytes received=%d\n", sizeof(incomingReadings));
  Serial.printf("launch=%d\n", incomingReadings.launch);

  if (incomingReadings.launch = 1)
  {
    Serial.println("Initiate launch sequence.");
    bTrigger = true;
    incomingReadings.launch=0;
  }
}

//void sendOops() {
  //Send message via ESP-NOW
  //DataToSend.oops = 1;
  //esp_err_t result = esp_now_send(0, (uint8_t *) &DataToSend, sizeof(DataToSend));
  
  // if (result == ESP_OK) {
  //    Serial.println("Sent with success");
  //    Serial.println("Sent: ");
  //    Serial.print("oops: ");
  //    Serial.println(oops);
  //  }
  //else {
  //   Serial.println("Error sending the data");
  //}
//}


void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  pinMode(TST_PIN, INPUT_PULLUP);
//pinMode(LED_BUILTIN, OUTPUT);

  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness( BRIGHTNESS );
  showReady(CRGB(0,100,0));
  
  Serial.println("launch board init...");

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  // Once ESPNow is successfully Init, we will register for recv CB to get recv packet info
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));

  // Register peer (not needed unless we want to send)
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
  showReady(CRGB(0,0,0));
  Serial.println("launch board ready");
}


void showLeds(){
// Add entropy to random number generator; we use a lot of it.
  // random16_add_entropy( random());

  for (int i=0; i<400; i++){
    Fire2012();     // run simulation frame  
    FastLED.show(); // display this frame
    FastLED.delay(1000 / FRAMES_PER_SECOND);
  }
  FastLED.clear();
  FastLED.show();
}

void loop() {
  CheckButton();
  if (bTrigger)
  {
    showLeds();

    //sendOops();

    bTrigger = false;
  }
  delay(10);
}


#define COOLING  55
#define SPARKING 120
void Fire2012() {

// Array of temperature readings at each simulation cell
  static uint8_t heat[NUM_LEDS];

  // Step 1.  Cool down every cell a little
    for( int i = 0; i < NUM_LEDS; i++) {
      heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
    }
  
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= NUM_LEDS - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }
    
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < SPARKING ) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(160,255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < NUM_LEDS; j++) {
      CRGB color = HeatColor( heat[j]);
      int pixelnumber;
      if( gReverseDirection ) {
        pixelnumber = (NUM_LEDS-1) - j;
      } else {
        pixelnumber = j;
      }
      
      leds[pixelnumber] = color;
    }
}

// check and debounce button, set bTrigger if pressed
void CheckButton()
{
  bool bPressed = (digitalRead(TST_PIN) == LOW);
  if (bPressed)
  {
    while (digitalRead(TST_PIN) == LOW); // Wait for release
    delay(100); // debounce
    bTrigger = true;
  }
}

void showReady(CRGB color) {
  for (int i=0; i<NUM_LEDS; i++){
    leds[i] = color;
    FastLED.show();
    delay(20);
  }
}