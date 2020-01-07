#pragma once

class Terminal;

class Keyboard {
  Terminal& terminal;
  uint8_t modifiers = 0;
  uint8_t ignoreBytes = 0;

public:
  Keyboard(Terminal&);

  void handleIrq();
};
