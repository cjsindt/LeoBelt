/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp-now-one-to-many-esp32-esp8266/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

#include <esp_now.h>
#include <WiFi.h>

int level;
int count = 0;
unsigned long t = micros();
unsigned long t1;

// REPLACE WITH YOUR ESP RECEIVER'S MAC ADDRESS
uint8_t broadcastAddress1[] = {0x4C, 0x11, 0xAE, 0x77, 0xA2, 0x74};
uint8_t broadcastAddress2[] = {0x24, 0x6F, 0x28, 0xA4, 0x49, 0xB8};
uint8_t broadcastAddress3[] = {0x24, 0x6F, 0x28, 0x5F, 0x12, 0x14};
uint8_t broadcastAddress4[] = {0x24, 0x6F, 0x28, 0x5F, 0x12, 0x28};
uint8_t broadcastAddress5[] = {0x24, 0x6F, 0x28, 0xA6, 0x8B, 0x94};
uint8_t broadcastAddress6[] = {0x4C, 0x11, 0xAE, 0x76, 0x8B, 0xAC};
uint8_t broadcastAddress7[] = {0x24, 0x6F, 0x28, 0xA2, 0xDE, 0x64};
uint8_t broadcastAddress8[] = {0x24, 0x6F, 0x28, 0x1E, 0x01, 0xC4};
// Mac Address character arrays
char bA1[18];
char bA2[18];
char bA3[18];
char bA4[18];
char bA5[18];
char bA6[18];
char bA7[18];
char bA8[18];

typedef struct test_struct {
  char intensity;
  int respond;
  esp_err_t result;
  bool active = true;
  bool warned = false;
} test_struct;
test_struct f1;
test_struct f2;
test_struct f3;
test_struct f4;
test_struct f5;
test_struct f6;
test_struct f7;
test_struct f8;

typedef struct response {
  bool received;
} response;
response receiveMessage;

// Callback when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&receiveMessage, incomingData, sizeof(receiveMessage));

  // Obtain mac address of sender
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

  // Activate feathers that send back
  if (strcmp(macStr, bA1) == 0) { f1.active = true; }
  else if (strcmp(macStr, bA2) == 0) { f2.active = true; }
  else if (strcmp(macStr, bA3) == 0) { f3.active = true; }
  else if (strcmp(macStr, bA4) == 0) { f4.active = true; }
  else if (strcmp(macStr, bA5) == 0) { f5.active = true; }
  else if (strcmp(macStr, bA6) == 0) { f6.active = true; }
  else if (strcmp(macStr, bA7) == 0) { f7.active = true; }
  else if (strcmp(macStr, bA8) == 0) { f8.active = true; }
}

// Send message to all feathers
void sendFeathers(){
    // Feather 1
    if (f1.active) {
      if (f1.respond == 0) { f1.active = false; }
      f1.result = esp_now_send(broadcastAddress1, (uint8_t *) &f1, sizeof(test_struct));
      while (f1.result != 0) {
        f1.result = esp_now_send(broadcastAddress1, (uint8_t *) &f1, sizeof(test_struct));
      }
    }

    // Feather 2
    if (f2.active) {
      if (f2.respond == 0) { f2.active = false; }
      f2.result = esp_now_send(broadcastAddress2, (uint8_t *) &f2, sizeof(test_struct));
      while (f2.result != 0) {
        f2.result = esp_now_send(broadcastAddress2, (uint8_t *) &f2, sizeof(test_struct));
      }
    }

    // Feather 3
    if (f3.active) {
      if (f3.respond == 0) { f3.active = false; }
      f3.result = esp_now_send(broadcastAddress3, (uint8_t *) &f3, sizeof(test_struct));
      while (f3.result != 0) {
        f3.result = esp_now_send(broadcastAddress3, (uint8_t *) &f3, sizeof(test_struct));
      }
    }

    // Feather 4
    if (f4.active) {
      if (f4.respond == 0) { f4.active = false; }
      f4.result = esp_now_send(broadcastAddress4, (uint8_t *) &f4, sizeof(test_struct));
      while (f4.result != 0) {
        f4.result = esp_now_send(broadcastAddress4, (uint8_t *) &f4, sizeof(test_struct));
      }
    }

    // Feather 5
    if (f5.active) {
      if (f5.respond == 0) { f5.active = false; }
      f5.result = esp_now_send(broadcastAddress5, (uint8_t *) &f5, sizeof(test_struct));
      while (f5.result != 0) {
        f5.result = esp_now_send(broadcastAddress5, (uint8_t *) &f5, sizeof(test_struct));
      }
    }

    // Feather 6
    if (f6.active) {
      if (f6.respond == 0) { f6.active = false; }
      f6.result = esp_now_send(broadcastAddress6, (uint8_t *) &f6, sizeof(test_struct));
      while (f6.result != 0) {
        f6.result = esp_now_send(broadcastAddress6, (uint8_t *) &f6, sizeof(test_struct));
      }
    }

    // Feather 7
    if (f7.active) {
      if (f7.respond == 0) { f7.active = false; }
      f7.result = esp_now_send(broadcastAddress7, (uint8_t *) &f7, sizeof(test_struct));
      while (f7.result != 0) {
        f7.result = esp_now_send(broadcastAddress7, (uint8_t *) &f7, sizeof(test_struct));
      }
    }

    // Feather 8
    if (f8.active) {
      if (f8.respond == 0) { f8.active = false; }
      f8.result = esp_now_send(broadcastAddress8, (uint8_t *) &f8, sizeof(test_struct));
      while (f8.result != 0) {
        f8.result = esp_now_send(broadcastAddress8, (uint8_t *) &f8, sizeof(test_struct));
      }
    }

    // Check every 100 frames (~3 sec) to see if feather is still active/reactivated
    count++;
    if (count%100 == 0) {
      f1.respond = 0; f1.active = true;
      f2.respond = 0; f2.active = true;
      f3.respond = 0; f3.active = true;
      f4.respond = 0; f4.active = true;
      f5.respond = 0; f5.active = true;
      f6.respond = 0; f6.active = true;
      f7.respond = 0; f7.active = true;
      f8.respond = 0; f8.active = true;
    }
    else {
      f1.respond = 1;
      f2.respond = 1;
      f3.respond = 1;
      f4.respond = 1;
      f5.respond = 1;
      f6.respond = 1;
      f7.respond = 1;
      f8.respond = 1;
    }
}

void alert() {
  f1.intensity = 255;
  f2.intensity = 255;
  f3.intensity = 255;
  f4.intensity = 255;
  f5.intensity = 255;
  f6.intensity = 255;
  f7.intensity = 255;
  f8.intensity = 255;
  for (int m = 0; m < 10; m++) {
    delay(100);
    sendFeathers();
    delay(250);
    silenceFeathers();
  }
}

void silenceFeathers() {
  f1.intensity = 0;
  f2.intensity = 0;
  f3.intensity = 0;
  f4.intensity = 0;
  f5.intensity = 0;
  f6.intensity = 0;
  f7.intensity = 0;
  f8.intensity = 0;
  sendFeathers();
}

// Store mac address in readable form
void getMacs() {
  snprintf(bA1, sizeof(bA1), "%02x:%02x:%02x:%02x:%02x:%02x",
           broadcastAddress1[0], broadcastAddress1[1], broadcastAddress1[2], broadcastAddress1[3], broadcastAddress1[4], broadcastAddress1[5]);
  snprintf(bA2, sizeof(bA2), "%02x:%02x:%02x:%02x:%02x:%02x",
           broadcastAddress2[0], broadcastAddress2[1], broadcastAddress2[2], broadcastAddress2[3], broadcastAddress2[4], broadcastAddress2[5]);
  snprintf(bA3, sizeof(bA3), "%02x:%02x:%02x:%02x:%02x:%02x",
             broadcastAddress3[0], broadcastAddress3[1], broadcastAddress3[2], broadcastAddress3[3], broadcastAddress3[4], broadcastAddress3[5]);
  snprintf(bA4, sizeof(bA4), "%02x:%02x:%02x:%02x:%02x:%02x",
             broadcastAddress4[0], broadcastAddress4[1], broadcastAddress4[2], broadcastAddress4[3], broadcastAddress4[4], broadcastAddress4[5]);
  snprintf(bA5, sizeof(bA5), "%02x:%02x:%02x:%02x:%02x:%02x",
             broadcastAddress5[0], broadcastAddress5[1], broadcastAddress5[2], broadcastAddress5[3], broadcastAddress5[4], broadcastAddress5[5]);
  snprintf(bA6, sizeof(bA6), "%02x:%02x:%02x:%02x:%02x:%02x",
             broadcastAddress6[0], broadcastAddress6[1], broadcastAddress6[2], broadcastAddress6[3], broadcastAddress6[4], broadcastAddress6[5]);
  snprintf(bA7, sizeof(bA7), "%02x:%02x:%02x:%02x:%02x:%02x",
             broadcastAddress7[0], broadcastAddress7[1], broadcastAddress7[2], broadcastAddress7[3], broadcastAddress7[4], broadcastAddress7[5]);
  snprintf(bA8, sizeof(bA8), "%02x:%02x:%02x:%02x:%02x:%02x",
             broadcastAddress8[0], broadcastAddress8[1], broadcastAddress8[2], broadcastAddress8[3], broadcastAddress8[4], broadcastAddress8[5]);
}

void setup() {
  Serial.begin(57600);
 
  WiFi.mode(WIFI_STA);
 
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  getMacs();
   
  // register peer
  esp_now_peer_info_t peerInfo = {};
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // register first peer  
  memcpy(peerInfo.peer_addr, broadcastAddress1, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  
  // register second peer  
  memcpy(peerInfo.peer_addr, broadcastAddress2, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  
  // register third peer  
  memcpy(peerInfo.peer_addr, broadcastAddress3, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

  // register fourth peer  
  memcpy(peerInfo.peer_addr, broadcastAddress4, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

  // register fifth peer  
  memcpy(peerInfo.peer_addr, broadcastAddress5, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

  // register sixth peer  
  memcpy(peerInfo.peer_addr, broadcastAddress6, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

  // register seventh peer  
  memcpy(peerInfo.peer_addr, broadcastAddress7, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

  // register eigth peer  
  memcpy(peerInfo.peer_addr, broadcastAddress8, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);
}
 
void loop() {
  while (Serial.available() > 0) { // if any data available
    char incomingByte = Serial.read(); // read byte0
    t1 = micros();
    if (incomingByte == 49) { // Feather 1
      f1.intensity = level;
      Serial.print("\n1");
    } else if (incomingByte == 50) { // Feather 2
      f2.intensity = level;
      Serial.print("\n2");
    } else if (incomingByte == 51) { // Feather 3
      f3.intensity = level;
      Serial.print("\n3");
    } else if (incomingByte == 52) { // Feather 4
      f4.intensity = level;
      Serial.print("\n4");
    } else if (incomingByte == 53) { // Feather 5
      f5.intensity = level;
      Serial.print("\n5");
    } else if (incomingByte == 54) { // Feather 6
      f6.intensity = level;
      Serial.print("\n6");
    } else if (incomingByte == 55) { // Feather 7
      f7.intensity = level;
      Serial.print("\n7");
    } else if (incomingByte == 56) { // Feather 8
      f8.intensity = level;
      Serial.print("\n8");
      sendFeathers();
    } else { // Intensity of vibration -- 5 levels of offset
        if (incomingByte == 35) { // #
          level = level;
        } else if (incomingByte == 36) { // $
          level = level + 52;
        } else if (incomingByte == 37) { // %
          level = level + 104;
        } else if (incomingByte == 38) { // &
          level = level + 156;
        } else if (incomingByte == 39) { // '
          level = level + 208;
        } else { // Intensity of vibration -- 52 levels
            if (incomingByte == 65) { // A 
              level = 0;
            } else if (incomingByte == 66) { // B
              level = 1;
            } else if (incomingByte == 67) { // C
              level = 2;
            } else if (incomingByte == 68) { // D
              level = 3;
            } else if (incomingByte == 69) { // E
              level = 4;
            } else if (incomingByte == 70) { // F
              level = 5;
            } else if (incomingByte == 71) { // G
              level = 6;
            } else if (incomingByte == 72) { // H
              level = 7;
            } else if (incomingByte == 73) { // I
              level = 8;
            } else if (incomingByte == 74) { // J
              level = 9;
            } else if (incomingByte == 75) { // K
              level = 10;
            } else if (incomingByte == 76) { // L
              level = 11;
            } else if (incomingByte == 77) { // M 
              level = 12;
            } else if (incomingByte == 78) { // N
              level = 13;
            } else if (incomingByte == 79) { // O
              level = 14;
            } else if (incomingByte == 80) { // P
              level = 15;
            } else if (incomingByte == 81) { // Q 
              level = 16;
            } else if (incomingByte == 82) { // R
              level = 17;
            } else if (incomingByte == 83) { // S
              level = 18;
            } else if (incomingByte == 84) { // T
              level = 19;
            } else if (incomingByte == 85) { // U
              level = 20;
            } else if (incomingByte == 86) { // V
              level = 21;
            } else if (incomingByte == 87) { // W 
              level = 22;
            } else if (incomingByte == 88) { // X
              level = 23;
            } else if (incomingByte == 89) { // Y
              level = 24;
            } else if (incomingByte == 90) { // Z
              level = 25;
            } else if (incomingByte == 97) { // a
              level = 26;
            } else if (incomingByte == 98) { // b
              level = 27;
            } else if (incomingByte == 99) { // c
              level = 28;
            } else if (incomingByte == 100) { // d
              level = 29;
            } else if (incomingByte == 101) { // e 
              level = 30;
            } else if (incomingByte == 102) { // f
              level = 31;
            } else if (incomingByte == 103) { // g
              level = 32;
            } else if (incomingByte == 104) { // h
              level = 33;
            } else if (incomingByte == 105) { // i
              level = 34;
            } else if (incomingByte == 106) { // j
              level = 35;
            } else if (incomingByte == 107) { // k
              level = 36;
            } else if (incomingByte == 108) { // l
              level = 37;
            } else if (incomingByte == 109) { // m
              level = 38;
            } else if (incomingByte == 110) { // n
              level = 39;
            } else if (incomingByte == 111) { // o 
              level = 40;
            } else if (incomingByte == 112) { // p
              level = 41;
            } else if (incomingByte == 113) { // q
              level = 42;
            } else if (incomingByte == 114) { // r
              level = 43;
            } else if (incomingByte == 115) { // s
              level = 44;
            } else if (incomingByte == 116) { // t
              level = 45;
            } else if (incomingByte == 117) { // u
              level = 46;
            } else if (incomingByte == 118) { // v
              level = 47;
            } else if (incomingByte == 119) { // w
              level = 48;
            } else if (incomingByte == 120) { // x
              level = 49;
            } else if (incomingByte == 121) { // y 
              level = 50;
            } else if (incomingByte == 122) { // z
              level = 51;
            }
        }
    }
  }
  t = micros() - t1;
  t = (float) t/1000;
  if (t > 5000) {silenceFeathers();}
}
