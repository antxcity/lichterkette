#include "ButtonWithLeds.h"

CButtonWithLedsBase::CButtonWithLedsBase(
    int button_pin,
    int led_count,
    int led_brightness,
    callbackFunction onPress)
{
    _button_pin = button_pin;
    _led_count = led_count;
    _led_brightness = led_brightness;

    _button = new OneButton(_button_pin, false);
    _button->attachClick(onPress);

    _leds = new CFastLED();
    _rgb_data = new CRGB[_led_count];

    pinMode(_button_pin, INPUT);
}

void CButtonWithLedsBase::on()
{
    for (int i = 0; i < _led_count; i++)
    {
        _rgb_data[i] = CRGB::Red;
    }
    _led_controller->showLeds();
};

void CButtonWithLedsBase::off()
{
    for (int i = 0; i < _led_count; i++)
    {
        _rgb_data[i] = CRGB::Black;
    }
    _led_controller->showLeds();
};

int CButtonWithLedsBase::pushValue()
{
    _value = (++_value % (_led_count + 1)); 

    if ( _value != _last_value ) {
        _last_value = _value;
        _changed = true;
    }
    return _value;
};

void CButtonWithLedsBase::updateLeds()
{
    Serial.println("updateLeds (Button-pin " + String(_button_pin) + "): " + String(_value) + "/" + String(_led_count));
    for (int i = 0; i < _led_count; i++) {
        if ( i >= _value ) {
            _rgb_data[i] = CRGB::Black;
        }
        else {
            _rgb_data[i] = CRGB::White;
            _rgb_data[i] = _rgb_data[i].nscale8(1);
        }
  }
  //Serial.println("ok");
    _led_controller->showLeds();
};

void CButtonWithLedsBase::tick()
{
    // Serial.println("CButtonWithLedsBase::tick()");
    _button->tick();
};

boolean CButtonWithLedsBase::has_changed(boolean reset = false) 
{
    boolean c = _changed;
    if (reset) 
        reset_changed();
    return c;
}

void CButtonWithLedsBase::reset_changed()
{
    _changed = false;
}