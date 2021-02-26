/***************************************************
 Library implementing a simple ringbuffer.
 Functions like available, write and read are implemented
 
 License: Creative Common V1. 

 Benjamin Völker, voelkerb@me.com
 Embedded Systems Engineer
 ****************************************************/

#ifndef RING_BUFFER_h
#define RING_BUFFER_h

#if (ARDUINO >= 100)
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class RingBuffer {
  public:
    RingBuffer(uint32_t size);
#if defined(ESP32)
    RingBuffer(uint32_t size, bool usePSRAM);
#endif
    bool init();
#if defined(ESP32) or defined(ESP8266) 
    bool IRAM_ATTR write(uint8_t * data, uint32_t size); // Returns false if overflow occurs
#else
    bool write(uint8_t * data, uint32_t size); // Returns false if overflow occurs
#endif
    bool read(uint8_t * data, uint32_t size);
    void reset();
#if defined(ESP32)
    bool inPSRAM();
    bool reSize(uint32_t size, bool inPSRAM=false);
#else
    bool retSize(uint32_t size);
#endif
    uint32_t getSize();
    uint32_t available();
    uint32_t availableForWrite();


  private:
    uint32_t _rwDistance();
#if defined(ESP32) or defined(ESP8266) 
    uint32_t IRAM_ATTR _wrDistance();
#else
    uint32_t IRAM_ATTR _wrDistance();
#endif
    // Size of the buffer
    uint32_t _ring_buffer_size;
    uint32_t _readPtr;
    uint32_t _writePtr;
    bool _resetted;
    // Buffer pointer
    uint8_t* _buffer;
    // esp_himem_handle_t _memoryHandle;
    // esp_himem_rangehandle_t _rangeHandle;
    // If ps ram should be used for the buffer
    bool _psram;
};

#endif
