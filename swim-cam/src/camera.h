// Author: Peter Jensen

#ifndef _CAMERA_H
#define _CAMERA_H
#include <esp_camera.h>
#include "debug.h"

#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

#define LED_GPIO_NUM      4

static camera_config_t cameraConfig = {
  .pin_pwdn = PWDN_GPIO_NUM,
  .pin_reset = RESET_GPIO_NUM,
  .pin_xclk = XCLK_GPIO_NUM,
  .pin_sccb_sda = SIOD_GPIO_NUM,
  .pin_sccb_scl = SIOC_GPIO_NUM,
  .pin_d7 = Y9_GPIO_NUM,
  .pin_d6 = Y8_GPIO_NUM,
  .pin_d5 = Y7_GPIO_NUM,
  .pin_d4 = Y6_GPIO_NUM,
  .pin_d3 = Y5_GPIO_NUM,
  .pin_d2 = Y4_GPIO_NUM,
  .pin_d1 = Y3_GPIO_NUM,
  .pin_d0 = Y2_GPIO_NUM,
  .pin_vsync = VSYNC_GPIO_NUM,
  .pin_href = HREF_GPIO_NUM,
  .pin_pclk = PCLK_GPIO_NUM,
  .xclk_freq_hz = 20000000,
  .ledc_timer = LEDC_TIMER_0,
  .ledc_channel = LEDC_CHANNEL_0,
  .pixel_format = PIXFORMAT_JPEG,
  .frame_size = FRAMESIZE_UXGA,
  .jpeg_quality = 10,
  .fb_count = 2,
  .fb_location = CAMERA_FB_IN_PSRAM,
  .grab_mode = CAMERA_GRAB_LATEST,
  .sccb_i2c_port = -1 // Not used because pin_sccb_sda != -1
};

class Camera {
  camera_fb_t *fb = nullptr;
public:
  bool init() {
    esp_err_t err = esp_camera_init(&cameraConfig);
    if (err != ESP_OK) {
      DEBUG_PRINTF("Camera: Init failed with error 0x%x\n", err);
      return false;
    }
    sensor_t *s = esp_camera_sensor_get();
    s->set_aec2(s, 1);
    pinMode(LED_GPIO_NUM, OUTPUT);
    digitalWrite(LED_GPIO_NUM, LOW);

    // warm up the camera
    uint8_t  *buf;
    uint32_t  len;
    for (uint32_t i = 0; i < 5; i++) {
      capture(&buf, &len);
      captureRelease();
    }
    delay(1000);
    for (uint32_t i = 0; i < 5; i++) {
      capture(&buf, &len);
      captureRelease();
    }
    return true;
  }
  bool capture(uint8_t **buf, uint32_t *len) {
    if (fb != nullptr) {
      DEBUG_PRINTF("Camera: Last capture not released\n");
      return false;
    }
    fb  = esp_camera_fb_get();
    if (!fb) {
      DEBUG_PRINTF("Camera: Capture fail\n");
      return false;
    }
    *buf = fb->buf;
    *len = fb->len;
    return true;
  }

  void captureRelease() {
    esp_camera_fb_return(fb);
    fb = nullptr;
  }

  void dumpStatus() {
    sensor_t        *s = esp_camera_sensor_get();
    camera_status_t *status = &(s->status);
    DEBUG_PRINTF("xclk:           %u\n", s->xclk_freq_hz/1000000);
    DEBUG_PRINTF("pixformat:      %u\n", s->pixformat);
    DEBUG_PRINTF("framesize:      %u\n", status->framesize);
    DEBUG_PRINTF("quality:        %u\n", status->quality);
    DEBUG_PRINTF("brightness:     %d\n", status->brightness);
    DEBUG_PRINTF("contrast:       %d\n", status->contrast);
    DEBUG_PRINTF("saturation:     %d\n", status->saturation);
    DEBUG_PRINTF("sharpness:      %d\n", status->sharpness);
    DEBUG_PRINTF("special_effect: %u\n", status->special_effect);
    DEBUG_PRINTF("wb_mode:        %u\n", status->wb_mode);
    DEBUG_PRINTF("awb:            %u\n", status->awb);
    DEBUG_PRINTF("awb_gain:       %u\n", status->awb_gain);
    DEBUG_PRINTF("aec:            %u\n", status->aec);
    DEBUG_PRINTF("aec2:           %u\n", status->aec2);
    DEBUG_PRINTF("aec_value:      %u\n", status->aec_value);
    DEBUG_PRINTF("agc:            %u\n", status->agc);
    DEBUG_PRINTF("agc_gain:       %u\n", status->agc_gain);
    DEBUG_PRINTF("gainceiling:    %u\n", status->gainceiling);
    DEBUG_PRINTF("bpc:            %u\n", status->bpc);
    DEBUG_PRINTF("wpc:            %u\n", status->wpc);
    DEBUG_PRINTF("raw_gma:        %u\n", status->raw_gma);
    DEBUG_PRINTF("lenc:           %u\n", status->lenc);
    DEBUG_PRINTF("hmirror:        %u\n", status->hmirror);
    DEBUG_PRINTF("vflip:          %u\n", status->vflip);
    DEBUG_PRINTF("dcw:            %u\n", status->dcw);
    DEBUG_PRINTF("colorbar:       %u\n", status->colorbar);
  }
};

#endif