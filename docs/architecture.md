# Architecture

## Data flow

```text
Buttons / touchscreen ─┐
Companion website ──────┼──> ESP32-S3 scoring state ──> LittleFS match database
                        │               │
                        │               └──> Wi-Fi AP + REST API + static website
ESP32-CAM <─────────────┘ (separate local node; stream is independent of scoring)
```

The durable record is a unique match ID and unique event IDs. Scoring records append to `/events.jsonl`, the match snapshot is atomically replaced with a backup, and the firmware replays any journal operations missing from the snapshot at boot. The browser cache is a last-known read-only view, never a scoring authority. Sync status stays local/unsynced until a cloud backend is configured.

## Hardware boundaries

- Main controller: ESP32-S3 DevKitC-1 N16R8, Arduino framework, PlatformIO.
- Camera: separate AI-Thinker ESP32-CAM with OV2640, statically addressed at `192.168.4.2` initially.
- Display: interface and pin map remain modular. The selected TFT and touch-controller part numbers are required before assigning any pins or claiming display validation.
- Physical buttons should call the same scoring command boundary as touchscreen and HTTP actions. Button pins are not assigned until a board-specific input module is selected.

## Scoring model

The TypeScript engine defines event semantics and deterministic replay used by the web application and its tests. The firmware has a C++ scoring boundary in `ScoringEngine.*`. Every accepted event carries the innings number, batter(s), bowler, event kind, runs, optional dismissal, timestamp and generated ID. Legal-ball accounting excludes wides and no-balls; byes and leg-byes count. Batter runs, team extras and bowler-conceded runs are separate fields.

For exact competition rules, a match operator must choose the applicable playing conditions. The first release defaults to conventional limited-overs rules and does not model retired-out, penalty runs, five-ball over variants, DLS, super overs or timed-out dismissals.

## Website modes

- Development: `npm run dev`, Vite serves the app from the computer. Configure device URL and token in Settings.
- Device production: `npm run build` copies Vite assets to `firmware/main-controller/data/`; upload LittleFS and firmware.
- Offline view: last loaded match JSON remains in browser localStorage and is visibly marked cached/offline. Scoring buttons are disabled while device connectivity is unavailable.

## Camera privacy

The camera only initializes when its explicit `/start` control is called, and only then does `/stream` return MJPEG. `/stop` immediately ends the stream and changes the camera's status LED. No video is recorded or relayed to a cloud service by this project. Local-network devices can view the feed unless the LAN is otherwise segmented.
