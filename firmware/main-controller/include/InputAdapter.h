#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>

enum class ScoreCommand : uint8_t { Dot, One, Two, Three, Four, Six, Wicket, Wide, NoBall, Bye, LegBye, Undo, SwitchStrike, Menu, Select, Back };

// A module-specific D-pad/button driver translates debounced hardware presses to these commands.
// A shared dispatcher should call the same score-event handler used by touch and REST input.
class InputAdapter {
 public:
  virtual ~InputAdapter() = default;
  virtual bool begin() = 0;
  virtual bool poll(ScoreCommand &command) = 0;
};

class ScoringCommandHandler {
 public:
  virtual ~ScoringCommandHandler() = default;
  virtual bool dispatch(ScoreCommand command, String &error) = 0;
};
