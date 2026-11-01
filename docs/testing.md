# Validation and test status

## Deterministic engine checks

`website/src/engine.test.ts` covers legal over completion, illegal wide/no-ball delivery counting, bye/leg-bye counting, odd-run and end-of-over strike rotation, batter/bowler boundary totals, run-out wicket credit, undo replay, restart replay, event editing, invalid event rejection, and innings completion.

Run from repository root:

```sh
npm run test:engine
npm run build
```

## Hardware checks still required

- PlatformIO compile/upload on ESP32-S3 and AI-Thinker ESP32-CAM.
- LittleFS mount, power-loss recovery and long-match storage endurance on the selected flash/partition layout.
- Journal reconciliation after a forced power loss during each scoring, undo, and edit write sequence.
- API/device end-to-end behavior on the AP, including access token and multiple concurrent clients.
- OV2640 initialization, stream stability, `/stop` behavior and active-low rear LED on physical hardware.
- Native display/touch bring-up and actual button debounce after selecting modules and pinouts.
- Calibration of event semantics against the competition's playing conditions and operator workflow.

No physical ESP32, camera, TFT or touch module is available in this workspace, so hardware validation is not claimed. A successful TypeScript check does not substitute for firmware compilation or official-match acceptance testing.
