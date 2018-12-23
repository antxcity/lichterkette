#include "Arduino.h"
#include <FastLED.h>
#include <OneButton.h>

class CButtonWithLedsBase 
{
protected:
    int _button_pin;
    int _led_count;
    int _led_brightness;    

    int _value = 0;
    int _last_value = -1;
    boolean _changed = true;

    OneButton       *_button;
    CFastLED        *_leds;
    CRGB            *_rgb_data;
    CLEDController  *_led_controller;
    
public:
    CButtonWithLedsBase(
        int button_pin, 
        int led_count, 
        int led_brightness, 
        callbackFunction onPress
    );
        
    void on();
    void off();
    int pushValue();
    int getValue() { return _value; };
    void updateLeds();
    void tick();

    boolean has_changed(boolean reset = false);
    void reset_changed();

};


template<uint8_t BUTTON_LED_DATA_PIN> 
class CButtonWithLeds : public CButtonWithLedsBase
{
public:
    CButtonWithLeds(
        int button_pin, 
        int led_count,
        int led_brightness,
        callbackFunction onPress
    );
};


template <uint8_t BUTTON_LED_DATA_PIN> 
CButtonWithLeds<BUTTON_LED_DATA_PIN>::CButtonWithLeds(
        int button_pin, 
        int led_count,
        int led_brightness,
        callbackFunction onPress) 
    : CButtonWithLedsBase(button_pin, led_count, led_brightness, onPress) 
{
    pinMode(BUTTON_LED_DATA_PIN, OUTPUT);
    _led_controller = &_leds->addLeds<PL9823, BUTTON_LED_DATA_PIN>(_rgb_data, _led_count);

    _led_controller->clearLedData();
    _led_controller->showLeds();
};
