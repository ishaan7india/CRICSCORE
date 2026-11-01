#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>

// Implement for the selected TFT/touch modules only after their controllers are known.
// Keeping this interface free of GPIO definitions prevents accidental board conflicts.
class DisplayAdapter {
 public:
  virtual ~DisplayAdapter() = default;
  virtual bool begin() = 0;
  virtual void showMatch(JsonObjectConst match) = 0;
  virtual void showMessage(const String &title, const String &message) = 0;
  virtual bool readTouch(int &x, int &y) = 0;
};
