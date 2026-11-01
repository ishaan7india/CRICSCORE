# Local REST API

The ESP32-S3 serves HTTP on `192.168.4.1`. JSON write requests require `X-CRICSCORE-TOKEN`; GET status and match reads are available to the local AP. On first boot, copy the token from the serial console into the website Settings. Responses use `application/json` and include `{ "error": "..." }` for failures.

| Method | Route | Purpose |
|---|---|---|
| GET | `/api/status` | Device and storage health |
| GET | `/api/matches` | Saved match list |
| GET | `/api/match/current` | Current match |
| GET | `/api/match/{id}` | Match by ID |
| POST | `/api/match/create` | Create a match with teams, squads, overs and toss |
| POST | `/api/match/start` | Start current match with `{striker, nonStriker, bowler}` IDs |
| POST | `/api/match/next-innings` | Start the chase with opening batter and bowler IDs |
| POST | `/api/match/event` | Submit one score event |
| POST | `/api/match/edit` | Replace an event by its zero-based index and replay the match |
| POST | `/api/match/undo` | Undo last score event |
| POST | `/api/match/control` | Pause, resume, switch strike, change bowler, or finish (`{action, ...}`) |
| GET | `/api/match/export` | Export current match JSON |
| POST | `/api/match/import` | Import a match JSON document |

Example score request:

```http
POST /api/match/event
X-CRICSCORE-TOKEN: <token>
Content-Type: application/json

{"innings":1,"kind":"runs","runs":4,"batterId":"a1","nonStrikerId":"a2","bowlerId":"b1"}
```

Kinds: `runs`, `wicket`, `wide`, `no_ball`, `bye`, `leg_bye`. Batter run values are 0, 1, 2, 3, 4 or 6; bye/leg-bye and wide/no-ball running extras accept whole numbers 0–6. A wicket includes a dismissal name. A wide may dismiss by run out or stumped; a no-ball may dismiss by run out or obstructing the field. Run-outs are not credited to the bowler.

Camera controller routes are independent of the scoring API: `GET /health`, `GET /status`, `POST /start`, `POST /stop`, and `GET /stream`.

The browser polls the device every eight seconds instead of using server-sent updates. Scoring event IDs are de-duplicated by the device. The append-only `/events.jsonl` journal records scoring and edit/undo control entries, while the match database is atomically replaced with a previous-file recovery copy.

Example edit request: `{"index": 4, "event": {"id":"same-event-id","innings":1,"kind":"runs","runs":2,"batterId":"a1","nonStrikerId":"a2","bowlerId":"b1"}}`. Event indexes refer to the full chronological event list for that match.
