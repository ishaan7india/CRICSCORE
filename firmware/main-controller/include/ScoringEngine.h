#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>

// Pure event-replay boundary: persistent records are authoritative, snapshots are derived.
class ScoringEngine {
 public:
  static bool validate(JsonObjectConst e, String &error);
  static void apply(JsonObjectConst e, JsonObject state);
  static bool rebuild(JsonArrayConst events, JsonObject state, String &error);
};
