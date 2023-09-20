// Author: Peter Jensen
#ifndef _SDCARD_H
#define _SDCARD_H
#include <FS.h>
#include <SD_MMC.h>
#include "debug.h"

class SdCard {
  bool         isInitialized;
  bool         hasCard;
  fs::SDMMCFS &fs;

public:
  SdCard(fs::SDMMCFS &fs) : isInitialized(false), hasCard(false), fs(fs) {}
  void begin() {
    const uint32_t maxTries = 10;
    uint32_t tryCount;
    isInitialized = true;
    //sdmmc_host_deinit();
    for (tryCount = 0; tryCount < maxTries; ++tryCount) {
      if (fs.begin("/sdcard", true)) {
        DEBUG_PRINTF("SD card mounted after %d tries\n", tryCount+1);
        break;
      }
      else {
        fs.end();
      }
    }
    if (tryCount == maxTries) {
      DEBUG_PRINTF("ERROR: Cannot initialize SD card\n");
      return;
    }
    uint8_t cardType = fs.cardType();
    if (cardType == CARD_NONE) {
      DEBUG_PRINTF("ERROR: Unknown SD card type\n");
      fs.end();
      return;
    }
    hasCard = true;
  }
  void end() {
    fs.end();
    isInitialized = false;
  }
  bool lazyInit() {
    if (!isInitialized) {
      begin();
    }
    return hasCard;
  }
  bool isDir(const char *dirName) {
    return lazyInit() && fs.exists(dirName);
  }
  void createDir(String dirName) {
    if (!lazyInit()) {
      return;
    }
    String dirNameStr = dirName + "/";
    int32_t slashPos = dirNameStr.indexOf("/", 1);
    while (slashPos != -1) {
      String parentDir = dirNameStr.substring(0, slashPos);
      if (!isDir(parentDir.c_str())) {
        DEBUG_PRINTF("Creating dir: %s\n", parentDir.c_str());
        fs.mkdir(parentDir);
      }
      slashPos = dirNameStr.indexOf("/", slashPos + 1);
    }
  }
  void createFile(String &fileName, const uint8_t *data, uint32_t len) {
    if (!lazyInit())
      return;
    const char *fileNameCStr = fileName.c_str();
    //DEBUG_STACK_INFO("createFile-entry:");
    DEBUG_PRINTF("Creating file: %s (size: %d)\n", fileNameCStr, len);
    fs::File file;
    file = fs.open(fileNameCStr, FILE_WRITE);
    file.write(data, len);
    file.close();
    //DEBUG_PRINTF("Created file: %s\n", fileNameCStr);
    //DEBUG_STACK_INFO("createFile-exit:");
  }
  void readFile(const char *fileName, uint8_t *data, uint32_t len) {
    if (!lazyInit())
      return;
    fs::File file;
    file = fs.open(fileName, FILE_READ);
    file.read(data, len);
    file.close();
  }
  void readFile(String &fileName, uint8_t *data, uint32_t len) {
    readFile(fileName.c_str(), data, len);
  }
  uint32_t fileSize(const char *fileName) {
    if (!lazyInit())
      return 0;
    uint32_t size;
    fs::File file;
    file = fs.open(fileName, FILE_READ);
    size = file.size();
    file.close();
    return size;
  }
  void createDirAndFile(String &fileName, const uint8_t *data, uint32_t len) {
    String dirName = fileName;
    uint32_t slashPos = fileName.lastIndexOf('/');
    dirName = dirName.substring(0, slashPos);
    createDir(dirName);
    createFile(fileName, data, len);
  }
  void removeFile(String &fileName) {
    fs.remove(fileName);
  }
  void removeFile(const char *fileName) {
    fs.remove(fileName);
  }
  
  using Callback = void (*)(const char *name, bool isDir, uint32_t size);
  void walkDir(const char *rootName, uint32_t level, Callback callback = nullptr, bool useFullName = true) {
    if (!lazyInit()) {
      return;
    }
    if (!callback)
      DEBUG_PRINTF("Listing Dir: %s\n", rootName);
    if (!fs.exists(rootName))
      return;
      
    File root = fs.open(rootName);
    if (!root){
      DEBUG_PRINTF("ERROR: Cannot open: %s\n", rootName);
      return;
    }
    if (!root.isDirectory()) {
      DEBUG_PRINTF("ERROR: %s is not a directory\n", rootName);
      root.close();
      return;
    }
    while (true) {
      File nextFile = root.openNextFile();
      if (!nextFile)
        break;
      String   fileName(nextFile.name());
      uint32_t fileSize    =  nextFile.size();
      bool     isDir       = nextFile.isDirectory();
      String   fullFileName;
      nextFile.close();
      if (rootName[strlen(rootName)-1] == '/')
        fullFileName = String(rootName) + fileName;
      else
        fullFileName = String(rootName) + "/" + fileName;
      const char *fullFileNameCStr = fullFileName.c_str();
      if (isDir) {
        if (callback)
          if (useFullName)
            callback(fullFileNameCStr, true, 0);
          else
            callback(fileName.c_str(), true, 0);
        else
          DEBUG_PRINTF("  DIR: %s\n", fileName);
        if (level > 1) {
          walkDir(fullFileNameCStr, level - 1, callback);
        }
      }
      else {
        if (callback)
          if (useFullName)
            callback(fullFileNameCStr, false, fileSize);
          else
            callback(fileName.c_str(), false, fileSize);
        else
          DEBUG_PRINTF("  FILE: %s (%d)\n", fileName.c_str(), fileSize);
      }
    }
    root.close();
  }
  void walkDir(String &rootName, uint32_t level, Callback callback = nullptr, bool useFullName = true) {
    walkDir(rootName.c_str(), level, callback, useFullName);
  }
};

#endif