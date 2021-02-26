/***************************************************
 Example file for using the ringbuffer library.
 
 License: Creative Common V1. 

 Benjamin Voelker, voelkerb@me.com
 Embedded Systems Engineer
 ****************************************************/

#include "ringbuffer.h"
// NOTE: If you use ESP32, make sure PSRAM is set to "Enabled" under Tools

#if defined(ESP32)
// This will use PSRAM of the ESP32
// const uint32_t BUF_SIZE = uint32_t(3.5*1024*1024); // 3.5MB
uint32_t BUF_SIZE = uint32_t(3*1024*1024); // 3.5MB
RingBuffer ringBuffer(BUF_SIZE, true);
#else
#if defined(ESP8266)
// This will not use flash memory of the ESP8266 
// It rather uses dynamic memory (SRAM)
const uint32_t BUF_SIZE = uint32_t(50*1024); // 50kB
#else
const uint32_t BUF_SIZE = 10*1024; // 10kB
#endif
RingBuffer ringBuffer(BUF_SIZE);
#endif


void setup() {
  Serial.begin(115200);
  delay(2000);

#if defined(ESP32)
  if (!psramFound()) {
    Serial.println("No PSRAM found, using regular RAM");
    BUF_SIZE = uint32_t(50*1024);
    ringBuffer.reSize(BUF_SIZE, false);  
  }
#endif

  Serial.printf("Trying to initialize %lu bytes\n", BUF_SIZE);
  // Init the ringbuffer
  bool success = ringBuffer.init();
  if (success) {
    Serial.printf("Ringbuffer initialized with size: %lu bytes\n", ringBuffer.getSize());

    volatile uint32_t i = 100;
    Serial.println("Writing:");
    while (ringBuffer.availableForWrite() >= sizeof(i)) {
      ringBuffer.write((uint8_t*) &i, sizeof(i));
      i--;
      Serial.println(i);
      if (i == 0) break;
    }
    uint32_t j = 0;
    Serial.println("Reading:");
    while (ringBuffer.available()) {
      ringBuffer.read((uint8_t*) &j, sizeof(j));
      Serial.println(j);
    }
  } else {
    Serial.println("Ringbuffer init failed");
  }

  Serial.println("Program done");
}

void loop() {
  yield();
}
