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

unsigned long t = micros();
unsigned long t1;
const int order = 10;
unsigned int val[order];
float sum = 0;
float avg;
int i;

//Structure example to receive data
//Must match the sender structure
typedef struct test_struct {
  int intensity;
} test_struct;

//Create a struct_message called myData
test_struct myData;

//callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  sigmaDeltaWrite(0, myData.intensity);
  
  t1 = micros() - t;
  val[i] = (float) t1/1000;
  sum += val[i++];
  i = i%order;
  
  avg = (float)sum/order;
  sum -= val[i];
  Serial.print(avg);
  Serial.print("\n");
  
  sigmaDeltaWrite(0, myData.intensity);
  Serial.print("\ntime: ");
  t = micros();
}
 
void setup() {
  sigmaDeltaSetup(0, 312500);
  sigmaDeltaAttachPin(A1, 0);
  sigmaDeltaWrite(0, 0);
  
  //Initialize Serial Monitor
  Serial.begin(57600);
  
  //Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  pinMode(LED_BUILTIN, OUTPUT);
  
  //Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  for (i = 0; i < order - 1; i++) {
    val[i] = 0;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);
}
 
void loop() {
  // put your main code here, to run repeatedly:
  //sigmaDeltaWrite(0, 255);
  //delay(500);
  //sigmaDeltaWrite(0, 0);
  //delay(500);
}
