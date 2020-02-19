/***************************************************
 Library implementing a simple ringbuffer.
 Functions like available, write and read are implemented

 Feel free to use the code as it is.

 Benjamin Völker, voelkerb@me.com
 Embedded Systems
 University of Freiburg, Institute of Informatik
 ****************************************************/

#include "ringbuffer.h"



RingBuffer::RingBuffer(uint32_t size) {
  _ring_buffer_size = size;
  _buffer = NULL;
  _resetted = true;
}


bool RingBuffer::init() {
  // You cannot init twice
  if (_buffer != NULL) return true;
  _buffer = (uint8_t*)malloc(_ring_buffer_size);

  // Return success or not
  if (_buffer != NULL) return true;
  else return false;
}

bool RingBuffer::setSize(uint32_t size) {
  _ring_buffer_size = size;
  if (_buffer != NULL) {
    free(_buffer);
  }
  _buffer = (uint8_t*)malloc(size);


  // Return success or not
  if (_buffer != NULL) return true;
  else return false;
}


uint32_t RingBuffer::available() {
  return _rwDistance();
}

uint32_t RingBuffer::availableForWrite() {
  return _wrDistance();
}

bool RingBuffer::write(uint8_t * data, uint32_t size) {
  uint32_t end = (_writePtr+size)%_ring_buffer_size;
  bool overflow = false;
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

uint32_t RingBuffer::getSize() {
  if (_buffer != NULL) return _ring_buffer_size;
  else return 0;
}
