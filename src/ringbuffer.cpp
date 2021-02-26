/***************************************************
 Library implementing a simple ringbuffer.
 Functions like available, write and read are implemented

 License: Creative Common V1. 

 Benjamin Völker, voelkerb@me.com
 Embedded Systems Engineer
 ****************************************************/

#include "ringbuffer.h"


RingBuffer::RingBuffer(uint32_t size) {
  _ring_buffer_size = size;
  _psram = false;
  _buffer = NULL;
  _resetted = true; 
}

#if defined(ESP32)
RingBuffer::RingBuffer(uint32_t size, bool usePSRAM) {
  _ring_buffer_size = size;
  _psram = usePSRAM;
  _buffer = NULL;
  _resetted = true; 
}
#endif

bool RingBuffer::init() {
  // You cannot init twice
  if (_buffer != NULL) return true;

  // // Allocate the memory
  // ESP_ERROR_CHECK(esp_himem_alloc(_ring_buffer_size, &_memoryHandle));
  // //Allocate a block of address range
  // ESP_ERROR_CHECK(esp_himem_alloc_map_range(ESP_HIMEM_BLKSZ, &_rangeHandle));
  //
  // for (int i = 0; i < _ring_buffer_size; i += ESP_HIMEM_BLKSZ) {
  //     uint32_t *ptr = NULL;
  //     //Map in block, write pseudo-random data, unmap block.
  //     ESP_ERROR_CHECK(esp_himem_map(_memoryHandle, _rangeHandle, i, 0, ESP_HIMEM_BLKSZ, 0, (void**)&ptr));
  //     fill_mem_seed(i ^ seed, ptr, ESP_HIMEM_BLKSZ); //
  //     ESP_ERROR_CHECK(esp_himem_unmap(rh, ptr, ESP_HIMEM_BLKSZ));
  // }
  #if defined(ESP32)
  if (_psram) {
    _buffer = (uint8_t*)ps_malloc(_ring_buffer_size);
  } 
  #endif
  if (!_psram) {
    _buffer = (uint8_t*)malloc(_ring_buffer_size);
  }

  // Return success or not
  if (_buffer != NULL) return true;
  else return false;
}


#if defined(ESP32)
bool RingBuffer::reSize(uint32_t size, bool inPSRAM) {
  _ring_buffer_size = size;
  _psram = inPSRAM;
  bool success = true;
  // If it was initialized before, reinit
  if (_buffer != NULL) {
    // free(_buffer)
    if (_psram) {
      _buffer = (uint8_t*)ps_realloc(_buffer, _ring_buffer_size);
    } else {
      _buffer = (uint8_t*)realloc(_buffer, _ring_buffer_size);
    }
    if (_buffer == NULL) success = false;
  }
  return success;
}
#else 
bool RingBuffer::reSize(uint32_t size) {
  _ring_buffer_size = size;
  bool success = true;
  // If it was initialized before, reinit
  if (_buffer != NULL) {
    _buffer = (uint8_t*)realloc(_buffer, _ring_buffer_size);
    if (_buffer == NULL) success = false;
  }
  return success
}
#endif


uint32_t RingBuffer::available() {
  return _rwDistance();
}

uint32_t RingBuffer::availableForWrite() {
  return _wrDistance();
}

bool RingBuffer::write(uint8_t * data, uint32_t size) {
  uint32_t end = (_writePtr+size)%_ring_buffer_size;
  bool overflow = false;
  // 0 means, both pointers are on the same, 
  // if we only allo _wrDistance() < size and not
  // wrDistance() <= size it should be fine
  if (_wrDistance() < size) {
    overflow = true;
    // Do not allow to write data here
    return false;
  }
  if (end > _writePtr) {
    memcpy((uint8_t*)&_buffer[_writePtr],(uint8_t*)&data[0], size);
  } else {
    memcpy((uint8_t*)&_buffer[_writePtr],(uint8_t*)&data[0], _ring_buffer_size - _writePtr);
    memcpy((uint8_t*)&_buffer[_writePtr],(uint8_t*)&data[_ring_buffer_size - _writePtr], end);
  }
  _resetted = false; 
  _writePtr = end;
  return !overflow;
}

void RingBuffer::reset() {
  _writePtr = 0;
  _readPtr = 0;
  _resetted = true; 
}

bool RingBuffer::read(uint8_t * data, uint32_t size) {
  uint32_t end = (_readPtr+size)%_ring_buffer_size;

  if (end > _readPtr) {
    memcpy((uint8_t*)&data[0], &_buffer[_readPtr], size);
  } else {
    memcpy((uint8_t*)&data[0], (uint8_t*)&_buffer[_readPtr], _ring_buffer_size - _readPtr);
    memcpy((uint8_t*)&data[(_ring_buffer_size - _readPtr)], (uint8_t*)&_buffer[0], end);
  }
  _readPtr = end;
  return true;
}

uint32_t RingBuffer::_rwDistance() {
  if (_writePtr < _readPtr) return _writePtr + (_ring_buffer_size - _readPtr);
  else return _writePtr - _readPtr;
}
uint32_t IRAM_ATTR RingBuffer::_wrDistance() {
  if (_resetted or (_readPtr - _writePtr) == 0) return _ring_buffer_size;
  if (_readPtr < _writePtr) return _readPtr + (_ring_buffer_size - _writePtr);
  else return _readPtr - _writePtr;
}

#if defined(ESP32)
bool RingBuffer::inPSRAM() {
  return _psram;
}
#endif

uint32_t RingBuffer::getSize() {
  if (_buffer != NULL) return _ring_buffer_size;
  else return 0;
}
