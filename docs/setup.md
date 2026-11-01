# Setup

## Website development

Install Node.js 20 or newer. At repository root:

```sh
npm install
npm run dev
```

Open the Vite URL shown in the terminal. In Settings enter the device base URL and local token. The computer must be connected to the CRICSCORE access point to access the physical console.

## Main ESP32-S3

Install PlatformIO Core or the VS Code PlatformIO extension. Build from `firmware/main-controller` with `pio run -e esp32s3`, then upload using the detected serial port. Build the web UI first with `npm run build`; upload the filesystem image with `pio run -e esp32s3 -t uploadfs`. Confirm the `default_16MB.csv` partition table fits the selected ESP32-S3 board revision.

On first boot the board creates `CRICSCORE` / `cricscore123`, at `192.168.4.1`, and generates a local API token stored in LittleFS. Read the one-time serial output and enter the token in the website Settings.

## Camera

Select `AI Thinker ESP32-CAM` / `esp32cam` in PlatformIO and upload `firmware/camera-controller`. The initial config statically addresses it as `192.168.4.2`. The camera joins the main controller AP. Check `http://192.168.4.2/health`; stream is stopped on boot. Use the website Livestream page and Start stream control to initialize and enable the MJPEG feed.

## Display and controls

The selected TFT and touch-controller model is not part of the project brief. Add the correct adapter, verified wiring, and board-specific pin header before display bring-up. Do not select GPIOs by copying a random 3.5-inch display pin map.

## Access token recovery

The token is stored in `/token.txt` in LittleFS. Preserve the filesystem to retain matches. If the token is lost, access the device serial output or use a documented, deliberate device reset procedure; do not erase LittleFS casually because that erases saved local match data.
