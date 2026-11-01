# Wiring and configurable pins

## Main controller

The main controller is an ESP32-S3 DevKitC-1 N16R8. Do not wire a 3.5-inch SPI display until its controller and touch part numbers, logic voltage, backlight requirements, and board pinout are confirmed. Different 3.5-inch modules use different controllers and may consume SPI, touch, SD, and backlight pins that overlap bootstrapping or flash/PSRAM connections.

`firmware/main-controller/include/board_config.h` contains the network defaults and explicitly leaves `CRICSCORE_TFT_*` and `CRICSCORE_INPUT_*` mappings undefined. Add a module-specific header with verified assignments after selecting the hardware. The display adapter should implement init, draw-score, draw-menu and touch/input polling behind a narrow interface; it must call the same scoring command functions as the API.

## AI-Thinker ESP32-CAM

The supplied camera firmware uses the standard AI-Thinker OV2640 pin map in `firmware/camera-controller/src/main.cpp`. It reserves GPIO 33 for the active-low rear status LED. The bright flash LED on GPIO 4 is not used as a status lamp. Power the module from a stable supply able to handle Wi-Fi and camera current peaks; avoid powering it from a weak USB-to-serial adapter's 3.3 V output.

| Function | ESP32-CAM GPIO |
|---|---:|
| Camera XCLK | 0 |
| SCCB SDA / SCL | 26 / 27 |
| Camera data D0–D7 | 5, 18, 19, 21, 36, 39, 34, 35 |
| PCLK / VSYNC / HREF | 22 / 25 / 23 |
| PWDN | 32 |
| Rear status LED | 33, active low |

The AI-Thinker GPIO map is not reusable for arbitrary ESP32-CAM boards. Verify the actual module before applying power.

## Main-device buttons

No GPIO assignments are provided for the D-pad, menu/back/select or scoring controls until the display module and enclosure are selected. Keep button inputs in a separate board configuration header, use debouncing, and prevent any selected pin from conflicting with display/touch/PSRAM/boot functions.
