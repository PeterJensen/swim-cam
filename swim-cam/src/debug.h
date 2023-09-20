// Author: Peter Jensen
#ifndef _DEBUG_H
#define _DEBUG_H

#define DEBUG_INIT() { \
  Serial.begin(115200); \
  Serial.setDebugOutput(true);}
#define DEBUG_PRINTF(...) Serial.printf(__VA_ARGS__)
#define HEAP_CHECK(msg) {\
  DEBUG_PRINTF("Heap integrity: %s - ", msg);      \
  if (heap_caps_check_integrity(MALLOC_CAP_DEFAULT, true)) \
    DEBUG_PRINTF("OK\n");                    \
  else                                                     \
    DEBUG_PRINTF("NOT OK\n"); }
#define HEAP_INFO(msg) {\
  DEBUG_PRINTF("Heap info: %s\n", msg); \
  heap_caps_print_heap_info(MALLOC_CAP_DEFAULT); }

#endif