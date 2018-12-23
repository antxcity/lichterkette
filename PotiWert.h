#include <Arduino.h>

class CPotiWert
{
  int m_pin = 0;

  int smoothWert = 0;
  int potiWertAlt = 0;
  boolean quickAktiv = false;
  int x = 0;

  int last_value = -1;
  boolean changed = true;
  String _name;

public:
  CPotiWert(int pin, String name = "unnamed") {
    m_pin = pin;
    _name = name;
  }

  int read();
  int read_mapped(int minimum, int maximum);
  int read_rgb() {
    return read_mapped(0, 255);
  };

  boolean has_changed(boolean reset = false);
  void reset_changed();
    
};