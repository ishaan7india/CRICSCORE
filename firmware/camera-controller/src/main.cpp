#include <Arduino.h>
#include <WiFi.h>
#include <esp_camera.h>
#include <ESPAsyncWebServer.h>
#include "camera_config.h"

// AI-Thinker ESP32-CAM OV2640 pin map.
#define CAM_PWDN 32
#define CAM_RESET -1
#define CAM_XCLK 0
#define CAM_SIOD 26
#define CAM_SIOC 27
#define CAM_Y9 35
#define CAM_Y8 34
#define CAM_Y7 39
#define CAM_Y6 36
#define CAM_Y5 21
#define CAM_Y4 19
#define CAM_Y3 18
#define CAM_Y2 5
#define CAM_VSYNC 25
#define CAM_HREF 23
#define CAM_PCLK 22
AsyncWebServer server(80); volatile bool streaming=false; bool cameraReady=false; uint32_t lastWifiAttempt=0;
void indicator(bool on){digitalWrite(CRICSCORE_CAMERA_STATUS_LED,on?LOW:HIGH);}
bool initCamera(){camera_config_t c{};c.ledc_channel=LEDC_CHANNEL_0;c.ledc_timer=LEDC_TIMER_0;c.pin_d0=CAM_Y2;c.pin_d1=CAM_Y3;c.pin_d2=CAM_Y4;c.pin_d3=CAM_Y5;c.pin_d4=CAM_Y6;c.pin_d5=CAM_Y7;c.pin_d6=CAM_Y8;c.pin_d7=CAM_Y9;c.pin_xclk=CAM_XCLK;c.pin_pclk=CAM_PCLK;c.pin_vsync=CAM_VSYNC;c.pin_href=CAM_HREF;c.pin_sccb_sda=CAM_SIOD;c.pin_sccb_scl=CAM_SIOC;c.pin_pwdn=CAM_PWDN;c.pin_reset=CAM_RESET;c.xclk_freq_hz=20000000;c.pixel_format=PIXFORMAT_JPEG;c.frame_size=CRICSCORE_CAMERA_FRAMESIZE;c.jpeg_quality=CRICSCORE_CAMERA_JPEG_QUALITY;c.fb_count=1;c.grab_mode=CAMERA_GRAB_WHEN_EMPTY;c.fb_location=psramFound()?CAMERA_FB_IN_PSRAM:CAMERA_FB_IN_DRAM;esp_err_t err=esp_camera_init(&c);if(err!=ESP_OK){Serial.printf("Camera init failed: 0x%x\n",err);cameraReady=false;return false;}cameraReady=true;return true;}
String jsonStatus(){sensor_t*s=esp_camera_sensor_get();String resolution="unknown";if(s){framesize_t f=s->status.framesize;switch(f){case FRAMESIZE_QQVGA:resolution="160x120";break;case FRAMESIZE_QVGA:resolution="320x240";break;case FRAMESIZE_VGA:resolution="640x480";break;case FRAMESIZE_SVGA:resolution="800x600";break;case FRAMESIZE_XGA:resolution="1024x768";break;default:resolution="configured";}}return String("{\"ready\":")+(cameraReady?"true":"false")+",\"streaming\":"+(streaming?"true":"false")+",\"resolution\":\""+resolution+"\",\"format\":\"MJPEG\"}";}
void setup(){Serial.begin(115200);pinMode(CRICSCORE_CAMERA_STATUS_LED,OUTPUT);indicator(false);WiFi.mode(WIFI_STA);IPAddress ip(CRICSCORE_CAMERA_IP),gw(CRICSCORE_GATEWAY_IP),mask(CRICSCORE_SUBNET_IP);WiFi.config(ip,gw,mask);WiFi.begin(CRICSCORE_WIFI_SSID,CRICSCORE_WIFI_PASSWORD);lastWifiAttempt=millis();Serial.println("Camera stream stays OFF until explicit POST /start");
server.on("/health",HTTP_GET,[](AsyncWebServerRequest*r){AsyncWebServerResponse*res=r->beginResponse(200,"application/json",jsonStatus());res->addHeader("Access-Control-Allow-Origin","*");r->send(res);});server.on("/status",HTTP_GET,[](AsyncWebServerRequest*r){AsyncWebServerResponse*res=r->beginResponse(200,"application/json",jsonStatus());res->addHeader("Access-Control-Allow-Origin","*");r->send(res);});server.on("/start",HTTP_POST,[](AsyncWebServerRequest*r){if(!cameraReady&&!initCamera()){AsyncWebServerResponse*res=r->beginResponse(503,"application/json","{\"error\":\"Camera initialization failed\"}");res->addHeader("Access-Control-Allow-Origin","*");r->send(res);return;}streaming=true;indicator(true);AsyncWebServerResponse*res=r->beginResponse(200,"application/json",jsonStatus());res->addHeader("Access-Control-Allow-Origin","*");r->send(res);});server.on("/stop",HTTP_POST,[](AsyncWebServerRequest*r){streaming=false;indicator(false);AsyncWebServerResponse*res=r->beginResponse(200,"application/json",jsonStatus());res->addHeader("Access-Control-Allow-Origin","*");r->send(res);});server.on("/stream",HTTP_GET,[](AsyncWebServerRequest*r){if(!streaming||!cameraReady){AsyncWebServerResponse*res=r->beginResponse(409,"application/json","{\"error\":\"Stream is stopped\"}");res->addHeader("Access-Control-Allow-Origin","*");r->send(res);return;}AsyncWebServerResponse*res=r->beginChunkedResponse("multipart/x-mixed-replace; boundary=frame",[](uint8_t*buffer,size_t maxLen,size_t index)->size_t{static camera_fb_t*fb=nullptr;static size_t offset=0;static bool headerSent=false;if(!streaming){if(fb){esp_camera_fb_return(fb);fb=nullptr;}return 0;}if(!fb){fb=esp_camera_fb_get();offset=0;headerSent=false;if(!fb)return 0;}if(!headerSent){int n=snprintf((char*)buffer,maxLen,"--frame\r\nContent-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n",(unsigned)fb->len);if((size_t)n>maxLen)return 0;headerSent=true;return n;}size_t n=min(maxLen,fb->len-offset);memcpy(buffer,fb->buf+offset,n);offset+=n;if(offset>=fb->len){esp_camera_fb_return(fb);fb=nullptr;headerSent=false;}return n;});res->addHeader("Access-Control-Allow-Origin","*");r->send(res);});server.onNotFound([](AsyncWebServerRequest*r){if(r->method()==HTTP_OPTIONS){AsyncWebServerResponse*res=r->beginResponse(204);res->addHeader("Access-Control-Allow-Origin","*");res->addHeader("Access-Control-Allow-Headers","Content-Type");res->addHeader("Access-Control-Allow-Methods","GET, POST, OPTIONS");r->send(res);}else r->send(404,"application/json","{\"error\":\"Not found\"}");});server.begin();}
void loop(){if(WiFi.status()!=WL_CONNECTED&&millis()-lastWifiAttempt>10000){lastWifiAttempt=millis();WiFi.disconnect();WiFi.begin(CRICSCORE_WIFI_SSID,CRICSCORE_WIFI_PASSWORD);Serial.println("Retrying CRICSCORE Wi-Fi");}delay(20);}
