# ringbuffer
Simple ringbuffer implementation.
Allows to store arbitrary data types in a ringbuffer. Uses stream syntax of available, read and write.

```C++
#include "ringbuffer.h"

const uint32_t BUF_SIZE = 10*1024; // 10kB
RingBuffer ringBuffer(BUF_SIZE);

void setup()
  Serial.printf("Trying to initialize %lu bytes\n", BUF_SIZE);
  // Init the ringbuffer
  bool success = ringBuffer.init();
  if (success) {
    Serial.printf("Ringbuffer initialized with size: %lu bytes\n", ringBuffer.getSize());

    // Writing the ringbuffer (will return false if full)
    bool success = false;
    for (int i = 10; i >= 0; i--) {
      success = ringBuffer.write((uint8_t*)&i, sizeof(i));
      if (!success) break;
    }
    
    Serial.println("Reading:");
    // Reading everything
    int j = 0;
    while (ringBuffer.available()) {
      ringBuffer.read((uint8_t*)&j, sizeof(j));
      Serial.println(j);
    }
  } else {
    Serial.println("Ringbuffer init failed");
  }
}
```


Output
```bash
Trying to initialize 10240 bytes
Ringbuffer initialized with size: 10240 bytes
Reading:
10
9
8
7
6
5
4
3
2
1
0
```
