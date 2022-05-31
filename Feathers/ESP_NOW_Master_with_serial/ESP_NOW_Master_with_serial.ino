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
int count;
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
esp_err_t result1;
esp_err_t result2;
esp_err_t result3;
esp_err_t result4;
esp_err_t result5;
esp_err_t result6;
esp_err_t result7;
esp_err_t result8;

typedef struct test_struct {
  char intensity;
} test_struct;

test_struct f1;
test_struct f2;
test_struct f3;
test_struct f4;
test_struct f5;
test_struct f6;
test_struct f7;
test_struct f8;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  char macStr[18];
  //Serial.print("Packet to: ");
  // Copies the sender mac address to a string
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  //Serial.print(macStr);
  //Serial.print(" send status:\t");
  //Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// Send message to all feathers
void sendFeathers(){
    // Feather 1
    result1 = esp_now_send(broadcastAddress1, (uint8_t *) &f1, sizeof(test_struct));
    if (result1 == 0) {
      Serial.print("\n1");
    }
    else {
      while (result1 != 0) {
        result1 = esp_now_send(broadcastAddress1, (uint8_t *) &f1, sizeof(test_struct));
      }
      Serial.print("\n1");
    }

    // Feather 2
    result2 = esp_now_send(broadcastAddress2, (uint8_t *) &f2, sizeof(test_struct));
    if (result2 == 0) {
      Serial.print("\n2");
    }
    else {
      while (result2 != 0) {
        result2 = esp_now_send(broadcastAddress2, (uint8_t *) &f2, sizeof(test_struct));
      }
      Serial.print("\n2");
    }

    // Feather 3
    result3 = esp_now_send(broadcastAddress3, (uint8_t *) &f3, sizeof(test_struct));
    if (result3 == 0) {
      Serial.print("\n3");
    }
    else {
      while (result3 != 0) {
        result3 = esp_now_send(broadcastAddress3, (uint8_t *) &f3, sizeof(test_struct));
      }
      Serial.print("\n3");
    }

    // Feather 4
    result4 = esp_now_send(broadcastAddress4, (uint8_t *) &f4, sizeof(test_struct));
    if (result4 == 0) {
      Serial.print("\n4");
    }
    else {
      while (result4 != 0) {
        result4 = esp_now_send(broadcastAddress4, (uint8_t *) &f4, sizeof(test_struct));
      }
      Serial.print("\n4");
    }

    // Feather 5
    result5 = esp_now_send(broadcastAddress5, (uint8_t *) &f5, sizeof(test_struct));
    if (result5 == 0) {
      Serial.print("\n5");
    }
    else {
      while (result5 != 0) {
        result5 = esp_now_send(broadcastAddress5, (uint8_t *) &f5, sizeof(test_struct));
      }
      Serial.print("\n5");
    }

    // Feather 6
    result6 = esp_now_send(broadcastAddress6, (uint8_t *) &f6, sizeof(test_struct));
    if (result6 == 0) {
      Serial.print("\n6");
    }
    else {
      while (result6 != 0) {
        result6 = esp_now_send(broadcastAddress6, (uint8_t *) &f6, sizeof(test_struct));
      }
      Serial.print("\n6");
    }

    // Feather 7
    result7 = esp_now_send(broadcastAddress7, (uint8_t *) &f7, sizeof(test_struct));
    if (result7 == 0) {
      Serial.print("\n7");
    }
    else {
      while (result7 != 0) {
        result7 = esp_now_send(broadcastAddress7, (uint8_t *) &f7, sizeof(test_struct));
      }
      Serial.print("\n7");
    }

    // Feather 8
    result8 = esp_now_send(broadcastAddress8, (uint8_t *) &f8, sizeof(test_struct));
    if (result8 == 0) {
      Serial.print("\n8");
    }
    else {
      while (result8 != 0) {
        result8 = esp_now_send(broadcastAddress8, (uint8_t *) &f8, sizeof(test_struct));
      }
      Serial.print("\n8");
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
 
void setup() {
  Serial.begin(57600);
 
  WiFi.mode(WIFI_STA);
 
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  esp_now_register_send_cb(OnDataSent);
   
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
