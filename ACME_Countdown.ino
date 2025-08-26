/*
Base ESP-now Project:
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://randomnerdtutorials.com/esp-now-two-way-communication-esp32/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
Base code for LED matrix text:
https://forums.adafruit.com/viewtopic.php?t=115535

Additions and modifications by Julie Barrett
I used a Seed Studios Xiao ESP32-C3

*/

#include <esp_now.h>
#include <WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_NeoMatrix.h>


// MATRIX DECLARATION:
// Parameter 1 = width of NeoPixel matrix
// Parameter 2 = height of matrix
// Parameter 3 = pin number (most are valid)
// Parameter 4 = matrix layout flags, add together as needed:
//   NEO_MATRIX_TOP, NEO_MATRIX_BOTTOM, NEO_MATRIX_LEFT, NEO_MATRIX_RIGHT:
//     Position of the FIRST LED in the matrix; pick two, e.g.
//     NEO_MATRIX_TOP + NEO_MATRIX_LEFT for the top-left corner.
//   NEO_MATRIX_ROWS, NEO_MATRIX_COLUMNS: LEDs are arranged in horizontal
//     rows or in vertical columns, respectively; pick one or the other.
//   NEO_MATRIX_PROGRESSIVE, NEO_MATRIX_ZIGZAG: all rows/columns proceed
//     in the same order, or alternate lines reverse direction; pick one.
//   See example below for these values in action.
// Parameter 5 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)

int matrixW = 16;
int matrixH = 16;
#define PIN D10  // OUTPUT PIN FROM ARDUINO TO MATRIX D-In

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(matrixW, matrixH, PIN,
                                               NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
                                               NEO_GRB + NEO_KHZ800);

const uint16_t colors[] = {
  //matrix.Color(255, 0, 0), matrix.Color(0, 255, 0), matrix.Color(255, 255, 0), matrix.Color(0, 0, 255), matrix.Color(255, 0, 255), matrix.Color(0, 255, 255), matrix.Color(255, 255, 255)
  //leaving the above in for future modification. I'm only using red now.
  matrix.Color(255, 0, 0)
};

#define arr_len(x) (sizeof(x) / sizeof(*x))  // Calculation of Array Size;

int pixelPerChar = 6;    // Width of Standard Font Characters is 8X6 Pixels
int x = matrix.width();  // Width of the Display
int pass = 0;            // Counter
int i = 0;               // Counter
int clr = 0;             // Counter for Indexing Array of Colors
int launch;              //
int music = 0;

//address of reciever
uint8_t broadcastAddress1[] = { 0x30, 0xC6, 0xF7, 0x00, 0x4F, 0x44 };  //wroom gantry
uint8_t broadcastAddress2[] = { 0xD8, 0x3B, 0xDA, 0x46, 0xDB, 0x88 };  //c6 rocket
// Variable to store if sending data was successful
String success;
char msg[] = "-------------------";  // BLANK Message of Your Choice; (for the matrix)
// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
  int buttonNew = 0;
  int oops = 0;
  int launch = 1;

} struct_message;
struct_message dataSent;
struct_message dataRcv;
struct_message incomingReadings;


esp_now_peer_info_t peerInfo;



// callback function that will be executed when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&dataRcv, incomingData, sizeof(dataRcv));
  Serial.print("Bytes received: ");
  Serial.println(sizeof(dataRcv));
  if (dataRcv.buttonNew = 1) {
    Serial.print("buttonNew: ");
    Serial.println(dataRcv.buttonNew);
    dataRcv.buttonNew = 0;  //reset the button status to zero
    dataRcv.launch = 1;     //flag to send to launch board

    esp_err_t result = esp_now_send(0, (uint8_t *)&dataSent, sizeof(dataSent));
    
    if (result == ESP_OK) {
      Serial.println("Sent with success");

    } else {
      Serial.println("Error sending the data");
    }
 
  }

  countdown();

}


void countdown() {
  int arrSize = arr_len(colors);
  String count = String(13);
  // text to send to countdown clock
  //char Text[] = "10....9....8....7....6....5....4....3....2....1....0      0      I said      ZERO! "; // SCROLLING Message of Your Choice;
  char Text[] = "10";  // The longer text strings really slow things down. See the comments in the matrix function.
                       //can I make this a loop?
  writeText(Text);
  char Text9[] = "9";
  writeText(Text9);
  char Text8[] = "8";
  writeText(Text8);
  char Text7[] = "7";
  writeText(Text7);
  char Text6[] = "6";
  writeText(Text6);
  char Text5[] = "5";
  writeText(Text5);
  char Text4[] = "4";
  writeText(Text4);
  char Text3[] = "3";
  writeText(Text3);
  char Text2[] = "2";
  writeText(Text2);
  char Text1[] = "1";
  writeText(Text1);
  char Text0[] = "0";
  writeText(Text0);


  delay(1000);
  char TextI[] = "I said";
  writeText(TextI);
  char TextZ[] = "ZERO!";
  writeText(TextZ);

launch =0;
}

void oops() {
  if (launch == 0){
  char TextU[] = "uh....";
  writeText(TextU);
  char TextOops[] ="oops.";
  writeText(TextOops);
  launch = 1;
  }
  else
  {return;}
}


void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  Serial.println("countdown board ready");

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packet info

  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
  esp_now_register_send_cb(esp_now_send_cb_t(OnDataSent));
  //register peers. One peer per broadcastAddressX above.
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

 // register first peer
    memcpy(peerInfo.peer_addr, broadcastAddress1, 6);
    Serial.println("Peer 1");
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer 1");

     }
    // register second peer
    memcpy(peerInfo.peer_addr, broadcastAddress2, 6);
    Serial.println("Peer 2");
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
      Serial.println("Failed to add peer 2");
    }

    matrix.begin();
    matrix.setTextWrap(false);
    matrix.setBrightness(10);
    matrix.setTextColor(colors[0]);
  }

  void writeText(String msg) {

    int arrSize = arr_len(colors);  // Array of Text Colors;
    // The next line is the original code used to calculate the size of the string. It really slowed
    // things down. A series of short messages is a lot faster.
    //int msgSize = (msg.length() * pixelPerChar) + (2 * pixelPerChar); // CACULATE message length;
    int msgSize = 9;
    int scrollingMax = (msgSize) + matrix.width();  // ADJUST Displacement for message length;

    x = matrix.width();  // RESET Cursor Position and Start Text String at New Position on the Far Right;
    clr = 0;             // RESET Color/Repeat Index;

    while (clr <= arrSize) {
      /* Change Color with Each Pass of Complete Message */
      matrix.setTextColor(colors[clr]);

      matrix.fillScreen(0);    // BLANK the Entire Screen;
      matrix.setCursor(x, 4);  // Set Starting Point for Text String;
      matrix.print(msg);       // Set the Message String;

      /* SCROLL TEXT FROM RIGHT TO LEFT BY MOVING THE CURSOR POSITION */
      if (--x < -scrollingMax) {
        /*  ADJUST FOR MESSAGE LENGTH  */
        // Decrement x by One AND Compare New Value of x to -scrollingMax;
        // This Animates (moves) the text by one pixel to the Left;

        x = matrix.width();  // After Scrolling by scrollingMax pixels, RESET Cursor Position and Start String at New Position on the Far Right;
        ++clr;               // INCREMENT COLOR/REPEAT LOOP COUNTER AFTER MESSAGE COMPLETED;
      }
      matrix.show();  // DISPLAY the Text/Image
      delay(23);      // SPEED OF SCROLLING or FRAME RATE;
    }
    clr = 0;  // Reset Color/Loop Counter;
  }

  void loop() {
  }
//  Callback when data is sent 
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("\r\nLast Packet Send Status:\t");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
    char macStr[18];
    Serial.print("Packet to: ");
    // Copies the sender mac address to a string
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
             mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    Serial.print(macStr);
    Serial.print(" send status:\t");

    Serial.print("launch ");
    Serial.println(launch);

    delay(14000);  // We're just going to wait while the launch board does its thing. Then:
    oops();        // Send the next message to the display.
    Serial.println("at data sent");
  }