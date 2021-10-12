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

// REPLACE WITH YOUR ESP RECEIVER'S MAC ADDRESS
uint8_t broadcastAddress1[] = {0x4C, 0x11, 0xAE, 0x77, 0xA2, 0x74};
uint8_t broadcastAddress2[] = {0x24, 0x6F, 0x28, 0xA4, 0x49, 0xB8};
uint8_t broadcastAddress3[] = {0x24, 0x6F, 0x28, 0x5F, 0x12, 0x14};
uint8_t broadcastAddress4[] = {0x24, 0x6F, 0x28, 0x5F, 0x12, 0x28};
uint8_t broadcastAddress5[] = {0x24, 0x6F, 0x28, 0xA6, 0x8B, 0x94};
uint8_t broadcastAddress6[] = {0x4C, 0x11, 0xAE, 0x76, 0x8B, 0xAC};
uint8_t broadcastAddress7[] = {0x24, 0x6F, 0x28, 0xA2, 0xDE, 0x64};
uint8_t broadcastAddress8[] = {0x24, 0x6F, 0x28, 0x1E, 0x01, 0xC4};
esp_err_t result;

typedef struct test_struct {
  char intensity;
  int y;
} test_struct;

test_struct test;

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

// Send message just to feather 1
void sendFeather1(){
    esp_err_t result = esp_now_send(broadcastAddress1, (uint8_t *) &test, sizeof(test_struct));
    Serial.write("Sent feather 1. Intensity: ");
    Serial.write(test.intensity);
}

// Send message just to feather 2
void sendFeather2(){
    esp_err_t result = esp_now_send(broadcastAddress2, (uint8_t *) &test, sizeof(test_struct));
    Serial.write("Sent feather 2. Intensity: ");
    Serial.write(test.intensity);
}

// Send message just to feather 3
void sendFeather3(){
    esp_err_t result = esp_now_send(broadcastAddress3, (uint8_t *) &test, sizeof(test_struct));
    Serial.write("Sent feather 3. Intensity: ");
    Serial.write(test.intensity);
}

// Send message just to feather 4
void sendFeather4(){
    esp_err_t result = esp_now_send(broadcastAddress4, (uint8_t *) &test, sizeof(test_struct));
    Serial.write("Sent feather 4. Intensity: ");
    Serial.write(test.intensity);
}

// Send message just to feather 5
void sendFeather5(){
    esp_err_t result = esp_now_send(broadcastAddress5, (uint8_t *) &test, sizeof(test_struct));
    Serial.write("Sent feather 5. Intensity: ");
    Serial.write(test.intensity);
}

// Send message just to feather 6
void sendFeather6(){
    esp_err_t result = esp_now_send(broadcastAddress6, (uint8_t *) &test, sizeof(test_struct));
    Serial.write("Sent feather 6. Intensity: ");
    Serial.write(test.intensity);
}

// Send message just to feather 7
void sendFeather7(){
    esp_err_t result = esp_now_send(broadcastAddress7, (uint8_t *) &test, sizeof(test_struct));
    Serial.write("Sent feather 7. Intensity: ");
    Serial.write(test.intensity);
}

// Send message just to feather 8
void sendFeather8(){
    esp_err_t result = esp_now_send(broadcastAddress8, (uint8_t *) &test, sizeof(test_struct));
    Serial.write("Sent feather 8. Intensity: ");
    Serial.write(test.intensity);
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
  esp_now_peer_info_t peerInfo;
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
  test.y = random(0,20);

  delay(4); // 30 fps/8 feathers = loop should run every ~4 ms
  while (Serial.available() > 0) { // if any data available
    char incomingByte = Serial.read(); // read byte0
    if (incomingByte == 49) { // Feather 1
      sendFeather1();
    } else if (incomingByte == 50) { // Feather 2
      sendFeather2();
    } else if (incomingByte == 51) { // Feather 3
      sendFeather3();
    } else if (incomingByte == 52) { // Feather 4
      sendFeather4();
    } else if (incomingByte == 53) { // Feather 5
      sendFeather5();
    } else if (incomingByte == 54) { // Feather 6
      sendFeather6();
    } else if (incomingByte == 55) { // Feather 7
      sendFeather7();
    } else if (incomingByte == 56) { // Feather 8
      sendFeather8();
    } else { // Intensity of vibration -- 5 levels of offset
        if (incomingByte == 35) { // #
          test.intensity = test.intensity;
        } else if (incomingByte == 36) { // $
          test.intensity = test.intensity + 52;
        } else if (incomingByte == 37) { // %
          test.intensity = test.intensity + 104;
        } else if (incomingByte == 38) { // &
          test.intensity = test.intensity + 156;
        } else if (incomingByte == 39) { // '
          test.intensity = test.intensity + 208;
        } else { // Intensity of vibration -- 52 levels
            if (incomingByte == 65) { // A 
              test.intensity = 0;
            } else if (incomingByte == 66) { // B
              test.intensity = 1;
            } else if (incomingByte == 67) { // C
              test.intensity = 2;
            } else if (incomingByte == 68) { // D
              test.intensity = 3;
            } else if (incomingByte == 69) { // E
              test.intensity = 4;
            } else if (incomingByte == 70) { // F
              test.intensity = 5;
            } else if (incomingByte == 71) { // G
              test.intensity = 6;
            } else if (incomingByte == 72) { // H
              test.intensity = 7;
            } else if (incomingByte == 73) { // I
              test.intensity = 8;
            } else if (incomingByte == 74) { // J
              test.intensity = 9;
            } else if (incomingByte == 75) { // K
              test.intensity = 10;
            } else if (incomingByte == 76) { // L
              test.intensity = 11;
            } else if (incomingByte == 77) { // M 
              test.intensity = 12;
            } else if (incomingByte == 78) { // N
              test.intensity = 13;
            } else if (incomingByte == 79) { // O
              test.intensity = 14;
            } else if (incomingByte == 80) { // P
              test.intensity = 15;
            } else if (incomingByte == 81) { // Q 
              test.intensity = 16;
            } else if (incomingByte == 82) { // R
              test.intensity = 17;
            } else if (incomingByte == 83) { // S
              test.intensity = 18;
            } else if (incomingByte == 84) { // T
              test.intensity = 19;
            } else if (incomingByte == 85) { // U
              test.intensity = 20;
            } else if (incomingByte == 86) { // V
              test.intensity = 21;
            } else if (incomingByte == 87) { // W 
              test.intensity = 22;
            } else if (incomingByte == 88) { // X
              test.intensity = 23;
            } else if (incomingByte == 89) { // Y
              test.intensity = 24;
            } else if (incomingByte == 90) { // Z
              test.intensity = 25;
            } else if (incomingByte == 97) { // a
              test.intensity = 26;
            } else if (incomingByte == 98) { // b
              test.intensity = 27;
            } else if (incomingByte == 99) { // c
              test.intensity = 28;
            } else if (incomingByte == 100) { // d
              test.intensity = 29;
            } else if (incomingByte == 101) { // e 
              test.intensity = 30;
            } else if (incomingByte == 102) { // f
              test.intensity = 31;
            } else if (incomingByte == 103) { // g
              test.intensity = 32;
            } else if (incomingByte == 104) { // h
              test.intensity = 33;
            } else if (incomingByte == 105) { // i
              test.intensity = 34;
            } else if (incomingByte == 106) { // j
              test.intensity = 35;
            } else if (incomingByte == 107) { // k
              test.intensity = 36;
            } else if (incomingByte == 108) { // l
              test.intensity = 37;
            } else if (incomingByte == 109) { // m
              test.intensity = 38;
            } else if (incomingByte == 110) { // n
              test.intensity = 39;
            } else if (incomingByte == 111) { // o 
              test.intensity = 40;
            } else if (incomingByte == 112) { // p
              test.intensity = 41;
            } else if (incomingByte == 113) { // q
              test.intensity = 42;
            } else if (incomingByte == 114) { // r
              test.intensity = 43;
            } else if (incomingByte == 115) { // s
              test.intensity = 44;
            } else if (incomingByte == 116) { // t
              test.intensity = 45;
            } else if (incomingByte == 117) { // u
              test.intensity = 46;
            } else if (incomingByte == 118) { // v
              test.intensity = 47;
            } else if (incomingByte == 119) { // w
              test.intensity = 48;
            } else if (incomingByte == 120) { // x
              test.intensity = 49;
            } else if (incomingByte == 121) { // y 
              test.intensity = 50;
            } else if (incomingByte == 122) { // z
              test.intensity = 51;
            }
        }
    }
    //Serial.write(incomingByte); // send it back
  }
}
