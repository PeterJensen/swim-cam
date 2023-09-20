// Author: Peter Jensen
#ifndef _WEB_UI_SERVER_H
#define _WEB_UI_SERVER_H

#include <Wifi.h>
#include <esp_http_server.h>
#include "swim-cam-html.h"
#include "debug.h"
#include "sdcard.h"
#include "cstring.h"

class SdCardInfo {
  SdCard &_sdCard;
private:
  static bool isGroupName(const char *name) {
    return true;
  }
public:
  SdCardInfo(SdCard &sdCard) : _sdCard(sdCard) {}
  static uint32_t groupCount;
  static void groupCountCb(const char *name, bool isDir, uint32_t size) {
    if (isDir && isGroupName(name)) {
      groupCount += 1;
    }
  }
  uint32_t numGroupsInDir(const char *rootName) {
    groupCount = 0;
    _sdCard.walkDir(rootName, 1, groupCountCb, false);
    return groupCount;
  }
  static char *groupName(char *buf, uint32_t groupNum) {
    sprintf(buf, "%04u", groupNum);
    return buf;
  }
  static char *imageName(char *buf, uint32_t timestamp) {
    sprintf(buf, "%08u.jpg", timestamp);
    return buf;
  }
  static CString *imagesJsonCstr;
  static void imagesJsonCb(const char *name, bool isDir, uint32_t size) {
    if (!isDir) {
      uint32_t num = atoi(name);
      if (imagesJsonCstr->len() > 1)
        *imagesJsonCstr = *imagesJsonCstr + ",";
      *imagesJsonCstr = *imagesJsonCstr + num;
    }
  }
  void imagesJson(CString &imageDir, CString &json) {
    json = "[";
    imagesJsonCstr = &json;
    _sdCard.walkDir(imageDir.cStr(), 1, imagesJsonCb, false);
    json = json + "]";
  }
  SdCard &sdCard() {
    return _sdCard;
  }
};
uint32_t SdCardInfo::groupCount;
CString *SdCardInfo::imagesJsonCstr;

class WebUiServer {
private:
  SdCardInfo             sdCardInfo;
  const char            *imageRootDir;
  static httpd_handle_t  serverHttpd;
  static WebUiServer    *self;

  #define maxFilePathLen 50
  #define maxJsonLen     1024
  #define maxQueryLen    50
  static char            filePathBuf[maxFilePathLen];
  static char            jsonBuf[maxJsonLen];
  static char            queryBuf[maxQueryLen];

  static bool getQuery(httpd_req_t *req, CString &query) {
    uint32_t len = httpd_req_get_url_query_len(req);
    if (len > 0) {
      if (httpd_req_get_url_query_str(req, query.cStr(), query.maxLen()) != ESP_OK)
        return false;
      query.setLen(len);
      return true;
    }
    return false;
  }
  static uint32_t getQueryVar(const char *query, const char *key) {
    char intStr[16];
    if (httpd_query_key_value(query, key, intStr, sizeof(intStr)) != ESP_OK){
      return 0;
    }
    return (uint32_t)atoi(intStr);
  }
  static esp_err_t indexHandler(httpd_req_t *req) {
    DEBUG_PRINTF("indexHandler called\n");
    httpd_resp_set_type(req, "text/html");
    return httpd_resp_send(req, swimCamHtml, strlen(swimCamHtml));
  }
  static esp_err_t groupsHandler(httpd_req_t *req) {
    DEBUG_PRINTF("groupsHandler called\n");
    uint32_t numGroups = self->sdCardInfo.numGroupsInDir(self->imageRootDir);
    DEBUG_PRINTF("numGroups: %d\n", numGroups);
    CString json(jsonBuf, maxJsonLen);
    CString groupDir(filePathBuf, maxFilePathLen);
    groupDir = self->imageRootDir;
    groupDir = groupDir + "/";
    uint32_t prefixLength = groupDir.len();
    json = "[";
    for (uint32_t g = 0; g < numGroups; g++) {
      char groupNameBuf[5];
      groupDir.setLen(prefixLength);
      groupDir = groupDir + SdCardInfo::groupName(groupNameBuf, g);
      uint32_t nsg = self->sdCardInfo.numGroupsInDir(groupDir.cStr());
      if (g > 0)
        json = json + ",";
      json = json + nsg;
    }
    json = json + "]";
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    return httpd_resp_send(req, json.cStr(), json.len());
  }
  static esp_err_t imagesHandler(httpd_req_t *req) {
    DEBUG_PRINTF("imagesHandler called\n");
    CString json(jsonBuf, maxJsonLen);
    CString query(queryBuf, maxQueryLen);
    getQuery(req, query);
    DEBUG_PRINTF("Query: %s\n", query.cStr());
    const char *queryCstr = query.cStr();
    uint32_t groupId = getQueryVar(queryCstr, "group");
    uint32_t subGroupId = getQueryVar(queryCstr, "subgroup");
    CString imgDir(filePathBuf, maxFilePathLen);
    char groupNameBuf[5];
    imgDir = self->imageRootDir;
    imgDir = imgDir + "/" + SdCardInfo::groupName(groupNameBuf, groupId);
    imgDir = imgDir + "/" + SdCardInfo::groupName(groupNameBuf, subGroupId);
    DEBUG_PRINTF("imgDir: %s\n", imgDir.cStr());
    self->sdCardInfo.imagesJson(imgDir, json);
    DEBUG_PRINTF("Images json: %s\n", json.cStr());
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    return httpd_resp_send(req, json.cStr(), json.len());
  }
  static esp_err_t imageHandler(httpd_req_t *req) {
    DEBUG_PRINTF("imageHandler called\n");
    CString query(queryBuf, maxQueryLen);
    getQuery(req, query);
    DEBUG_PRINTF("Query: %s\n", query.cStr());
    const char *queryCstr = query.cStr();
    uint32_t groupId = getQueryVar(queryCstr, "group");
    uint32_t subGroupId = getQueryVar(queryCstr, "subgroup");
    uint32_t timestamp  = getQueryVar(queryCstr, "timestamp");
    CString imgPath(filePathBuf, maxFilePathLen);
    char nameBuf[16];
    imgPath = self->imageRootDir;
    imgPath = imgPath + "/" + SdCardInfo::groupName(nameBuf, groupId);
    imgPath = imgPath + "/" + SdCardInfo::groupName(nameBuf, subGroupId);
    imgPath = imgPath + "/" + SdCardInfo::imageName(nameBuf, timestamp);
    DEBUG_PRINTF("imgPath: %s\n", imgPath.cStr());
    SdCard &sdCard = self->sdCardInfo.sdCard();
    uint32_t size = sdCard.fileSize(imgPath.cStr());
    uint8_t *imgBuf = (uint8_t *)malloc(size + 8);
    sdCard.readFile(imgPath.cStr(), imgBuf, size);
    httpd_resp_set_type(req, "image/jpeg");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    esp_err_t err = httpd_resp_send(req, (const char *)imgBuf, size);
    free(imgBuf);
    return err;
  }

public:
  WebUiServer(SdCard &sdCard, const char *imageRootDir) : sdCardInfo(sdCard), imageRootDir(imageRootDir) {
    self = this;
  }

  static bool startAp(const char *ssid, IPAddress ipAddr) {
    DEBUG_PRINTF("Starting WiFi Access Point\n");
    IPAddress gateway(192, 168, 4, 0);
    IPAddress subnet(255, 255, 255, 0);
    WiFi.mode(WIFI_AP);
    bool apOk = WiFi.softAP(ssid);
    if (!apOk) {
      DEBUG_PRINTF("ERROR: Cannot setup softAP\n");
      return false;
    }
    //bool apConfigOk = WiFi.softAPConfig(ipAddr, gateway, subnet);
    //if (!apConfigOk) {
    //  DEBUG_PRINTF("ERROR: Cannot configure softAP\n");
    //  return false;
    //}
    return true;
  }
  static bool startSta(const char *ssid, const char *password) {
    DEBUG_PRINTF("Starting WiFi Station\n");
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      DEBUG_PRINTF(".");
      delay(1000);
    }
    DEBUG_PRINTF("\nIP address: %s\n", WiFi.localIP().toString().c_str());
    return true;
  }
  bool startServer() {
    DEBUG_PRINTF("Starting Web server\n");
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_uri_t indexUri = {
      .uri = "/",
      .method = HTTP_GET,
      .handler = indexHandler,
      .user_ctx = NULL
    };
    httpd_uri_t groupsUri = {
      .uri = "/groups",
      .method = HTTP_GET,
      .handler = groupsHandler,
      .user_ctx = NULL
    };
    httpd_uri_t imagesUri = {
      .uri = "/images",
      .method = HTTP_GET,
      .handler = imagesHandler,
      .user_ctx = NULL
    };
    httpd_uri_t imageUri = {
      .uri = "/image",
      .method = HTTP_GET,
      .handler = imageHandler,
      .user_ctx = NULL
    };
    if (httpd_start(&serverHttpd, &config) != ESP_OK) {
      DEBUG_PRINTF("ERROR: Cannot start server\n");
      return false;
    }
    httpd_register_uri_handler(serverHttpd, &indexUri);
    httpd_register_uri_handler(serverHttpd, &groupsUri);
    httpd_register_uri_handler(serverHttpd, &imagesUri);
    httpd_register_uri_handler(serverHttpd, &imageUri);
    DEBUG_PRINTF("Web server started successfully\n");
    return true;
  }
  bool stopServer() {
    if (httpd_stop(serverHttpd) != ESP_OK) {
      DEBUG_PRINTF("ERROR: Cannot stop server\n");
      return false;
    }
    return true;
  }

};

httpd_handle_t  WebUiServer::serverHttpd = nullptr;
WebUiServer    *WebUiServer::self = nullptr;
char            WebUiServer::filePathBuf[maxFilePathLen];
char            WebUiServer::jsonBuf[maxJsonLen];
char            WebUiServer::queryBuf[maxQueryLen];

#endif