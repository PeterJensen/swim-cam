// Author: Peter Jensen

#include <Arduino.h>
#include <FS.h>
#include <SD_MMC.h>
#include <WiFi.h>
#include <driver/sdmmc_host.h>
#include <esp_heap_caps.h>
#include <soc/rtc_wdt.h>
#include "debug.h"
#include "camera.h"
#include "sdcard.h"
#include "web-ui-server.h"

struct Config {
  static constexpr const char *imageRootDir    = "/swims";
  static const uint32_t        startupDelay    = 10000;
  static constexpr const char *ssidAp          = "SWIM-CAM1";
  static constexpr const char *ipAddrAp        = "192.168.4.1";
  static const bool            setupAp         = true;
  static constexpr const char *ssidSta         = "<YOUR-SSID>";
  static constexpr const char *passwordSta     = "<YOUR-WIFI-PASSWORD>";
};

struct Pins {
  static const uint8_t pushButton = GPIO_NUM_13;
  static const uint8_t led        = GPIO_NUM_12;
  static const uint8_t ledFlash   = GPIO_NUM_4;
};

// ---------------------------------------------------------------------------
class Led {
private:
  bool       isOn;
  uint32_t   blinkRate;
  uint32_t   lastFlip;
  uint8_t    pin;
public:
  Led() : isOn(false), blinkRate(0) {}
  void init(uint8_t _pin) {
    lastFlip = millis();
    pin = _pin;
    pinMode(pin, OUTPUT);
    off();
  }
  void setBlinkRate(uint32_t rate) {
    blinkRate = rate;
  }
  void on() {
    digitalWrite(pin, HIGH);
    isOn = true;
  }
  void off() {
    digitalWrite(pin, LOW);
    isOn = false;
  }
  void flip() {
    if (isOn)
      off();
    else
      on();
    lastFlip = millis();
  }
  void update() {
    if (blinkRate > 0 && millis() > lastFlip + blinkRate)
      flip();
  }
};

// ---------------------------------------------------------------------------
class Button {
  void     (*onClickHandler)();
  uint32_t   bounceStart;
  uint8_t    pin;
  uint8_t    lastVal;
  bool       bouncing;
public:
  void init(uint8_t _pin) {
    lastVal    = HIGH;
    bouncing   = false;
    onClickHandler = nullptr;
    pin = _pin;
    pinMode(pin, INPUT_PULLUP);
  }
  void onClick(void (*handler)()) {
    onClickHandler = handler;
  }
  void update() {
    uint32_t now = millis();
    uint8_t  val = digitalRead(pin);
    if (!bouncing) {
      if (val != lastVal) {
        if (onClickHandler != nullptr && val == LOW) {
          onClickHandler();
        }
        lastVal = val;
        bounceStart = now;
        bouncing = true;
      }
    }
    else {
      if (now > bounceStart + 100) {
        bouncing = false;
      }
    }
  }
};

// ---------------------------------------------------------------------------
class State {
public:
  enum stateValue {takingPhotos, wifiHotspot, startup, idle};
  stateValue value;
private:
  uint32_t startTime;
public:
  State() : value(startup) {}
  void init() {
    startTime = millis();
  }
  bool updateTime() {
    if (value == startup) {
      uint32_t now = millis();
      if (now - startTime > Config::startupDelay) {
        value = takingPhotos;
        return true;
      }
    }
    return false;
  }
  bool updateClick() {
    if (value == startup) {
      value = wifiHotspot;
      return true;
    }
    else if (value == takingPhotos || value == wifiHotspot) {
      value = idle;
      return true;
    }
    return false;
  }
};

class Images {
  const uint32_t maxImagesInSubGroup = 100;

  String    rootDir;
  SdCard   &storage;
  String    groupDir;
  uint32_t  subGroupCount;
  String    subGroupDir;
  uint32_t  imagesInSubGroup;

  static String formatGroupDir(uint32_t num) {
    char buf[100];
    sprintf(buf, "%04d", num);
    return String(buf);
  }
  static String formatSubGroupDir(uint32_t num) {
    return formatGroupDir(num);
  }
  static String formatImageFile(uint32_t timestamp) {
    char buf[100];
    sprintf(buf, "%08d.jpg", timestamp);
    return String(buf);
  }
  static uint32_t groupDirCount;
  static void walk(const char *name, bool isDir, uint32_t size) {
    groupDirCount++;
  }
  String makeNextGroupDir() {
    groupDirCount = 0;
    storage.walkDir(rootDir, 1, walk, false);
    String nextGroupDir = rootDir + "/" + formatGroupDir(groupDirCount);
    storage.createDir(nextGroupDir);
    return nextGroupDir;
  }
  String makeSubGroupDir() {
    String nextSubGroupDir = groupDir + "/" + formatSubGroupDir(subGroupCount);
    subGroupCount += 1;
    storage.createDir(nextSubGroupDir);
    return nextSubGroupDir;
  }
public:
  Images(String rootDir, SdCard &sdCard) : rootDir(rootDir), storage(sdCard) {}
  void init() {
    subGroupCount = 0;
    groupDir = makeNextGroupDir();
    subGroupDir = makeSubGroupDir();
    imagesInSubGroup = 0;
  }  
  void addImage(uint32_t timeStamp, uint8_t *data, uint32_t dataLen) {
    if (imagesInSubGroup >= maxImagesInSubGroup) {
      subGroupDir = makeSubGroupDir();
      imagesInSubGroup = 0;
    }
    String imageFile = subGroupDir + "/" + formatImageFile(timeStamp);
    storage.createFile(imageFile, data, dataLen);
    imagesInSubGroup += 1;
  }
};
uint32_t Images::groupDirCount;

Button      button;
State       state;
SdCard      sdCard(SD_MMC);
Led         led;
Images      images(Config::imageRootDir, sdCard);
Camera      camera;
WebUiServer server(sdCard, Config::imageRootDir);

void removeZeroSize(const char *name, bool isDir, uint32_t size) {
  if (!isDir && size == 0) {
    DEBUG_PRINTF("Removing: %s\n", name);
    sdCard.removeFile(name);
  }
}

void updateLed() {
  switch (state.value) {
    case State::startup:
      led.setBlinkRate(0);
      led.on();
      break;
    case State::takingPhotos:
      led.setBlinkRate(500);
      break;
    case State::wifiHotspot:
      led.setBlinkRate(100);
      break;
    case State::idle:
      led.setBlinkRate(0);
      led.off();
      break;
  }
}

void click() {
  DEBUG_PRINTF("Button Clicked\n");
  bool change = state.updateClick();
  if (change) {
    DEBUG_PRINTF("New state after updateClick: %d\n", state.value);
    updateLed();
  }
}

void setup() {
  DEBUG_INIT();
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  pinMode(Pins::ledFlash, OUTPUT);
  digitalWrite(Pins::ledFlash, LOW);
  led.init(Pins::led);
  led.on();
  button.init(Pins::pushButton);
  button.onClick(click);
  state.init();
}

uint32_t firstCaptureTime;
bool     hasFinishedCapturing = false;
bool     serverStarted        = false;
bool     serverStopped        = false;

void loop() {
  led.update();
  button.update();
  bool change = state.updateTime();
  if (change) {
    DEBUG_PRINTF("New state after updateTime: %d\n", state.value);
    updateLed();
  }
  switch (state.value) {
    case State::startup:
      break;
    case State::takingPhotos: {
      if (change) {
        camera.init();
        camera.dumpStatus();
        sdCard.begin();
        images.init();
        firstCaptureTime = millis();
      }
      uint8_t *buf;
      uint32_t len;
      uint32_t captureTime = millis();
      camera.capture(&buf, &len);
      images.addImage(captureTime - firstCaptureTime, buf, len);
      camera.captureRelease();
      break;
    }
    case State::wifiHotspot:
      if (!serverStarted) {
        if (Config::setupAp) {
          IPAddress ipAddr;
          ipAddr.fromString(Config::ipAddrAp);
          serverStarted = server.startAp(Config::ssidAp, ipAddr);
        }
        else
          serverStarted = server.startSta(Config::ssidSta, Config::passwordSta);
        server.startServer();
      }
      break;
    case State::idle:
      if (!hasFinishedCapturing) {
        //sdCard.walkDir(Config::imageRootDir, 3);
        sdCard.end();
        hasFinishedCapturing = true;
      }
      if (!serverStopped && serverStarted) {
        server.stopServer();
        serverStopped = true;
      }
      break;
  }
}