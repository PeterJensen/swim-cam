// Author: Peter Jensen
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <cstdlib>

class CString {
  uint32_t _len;
  char    *_buf;
  uint32_t _maxLen;
  bool     _internalBuf;
public:
  CString(char *buf, uint32_t maxLen) : _len(0), _buf(buf), _maxLen(maxLen-1), _internalBuf(false) {}
  CString(uint32_t maxLen) : _len(0), _maxLen(maxLen-1), _internalBuf(true) {
    _buf = (char *)malloc(maxLen);
  }
  ~CString() {
    if (_internalBuf)
      free(_buf);
  }
  CString &operator=(const char *cStr) {
    _len = strlen(cStr);
    if (_len > _maxLen)
      _len = _maxLen;
    memcpy(_buf, cStr, _len);
    return *this;
  }
  CString &operator=(CString &rhs) {
    if (this == &rhs)
      return *this;
    if (rhs._len <= _maxLen) {
      memcpy(_buf, rhs._buf, rhs._len);
      _len = rhs._len;
    }
    return *this;
  }
  CString &operator+(const char *cStr) {
    uint32_t sl = strlen(cStr);
    uint32_t nl = _len + sl;
    uint32_t cl = sl;
    if (nl > _maxLen)
      cl -= nl - _maxLen;
    memcpy(_buf + _len, cStr, cl);
    _len += cl;
    return *this;
  }
  CString &operator+(uint32_t u) {
    char buf[20];
    sprintf(buf, "%u", u);
    return this->operator+(buf);
  }
  uint32_t len() {
    return _len;
  }
  void setLen(uint32_t len) {
    _len = len;
  }
  uint32_t maxLen() {
    return _maxLen;
  }
  char *cStr() {
    _buf[_len] = '\0';
    return _buf;
  }
};
