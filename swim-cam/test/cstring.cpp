#include <cstring.h>

int main() {
  char buf1[20];
  char buf2[20];
  CString cs1(buf1, 20);
  CString cs2(buf2, 20);
  cs1 = "Hello\n";
  printf("%s", cs1.cStr());
  cs1 = "Hello World!\n";
  printf("%s\n", cs1.cStr());
  cs1 = "He";
  cs1 = cs1 + "ll" + "o";
  printf("%s\n", cs1.cStr());
  cs1 = cs1 + " World!";
  printf("%s\n", cs1.cStr());
  cs2 = cs1;
  printf("%s\n", cs2.cStr());
  cs1 = "Prefix-";
  uint32_t prefixLen = cs1.length();
  for (uint32_t i = 0; i < 10; i++) {
    char spbuf[10];
    sprintf(spbuf, "%04u", i);
    cs1.setLength(prefixLen);
    cs1 = cs1 + spbuf;
    printf("%s\n", cs1.cStr());
  }
  CString cs3(1024);
  cs3 = "123456";
  cs3 = cs3 + " Yoyo";
  printf("%s\n", cs3.cStr());
}