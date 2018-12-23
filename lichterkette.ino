#include <FastLED.h>
#include <OneButton.h>
#include "PotiWert.h"
#include "ButtonWithLeds.h"

#define STRIP_LED_COUNT 51

#define DATA_PIN 9
#define BREITE_PIN A0
#define SHIFT_PIN A3

#define HUE_PIN A1
#define SATURATION_PIN A5
#define VALUE_PIN A4

#define ALIAS_PIN A2

#define BUTTON_1_LED_COUNT 3
#define BUTTON_1_PIN 12
#define BUTTON_1_LED_PIN 6
#define BUTTON_1_LED_BRIGHTNESS 1

#define BUTTON_2_LED_COUNT 6
#define BUTTON_2_PIN 11
#define BUTTON_2_LED_PIN 5
#define BUTTON_2_LED_BRIGHTNESS 1

CRGB leds[STRIP_LED_COUNT];

CFastLED strip = CFastLED();

CLEDController *ctrl_strip = 0;
CButtonWithLedsBase *myLedButton_1 = 0;
CButtonWithLedsBase *myLedButton_2 = 0;
unsigned long last_action_time = 0;
long last_elapsed_time = 0;
int timeouts[6] = { 10, 1*60, 5*60, 15*60, 30*60, 60*60 };
//int timeouts[6] = { 5, 10, 20, 10*60, 30*60, 60*60 };

void clickButton_1()
{
    myLedButton_1->pushValue();
    myLedButton_1->updateLeds();
    last_action_time = millis();
}

void clickButton_2()
{
    myLedButton_2->pushValue();
    myLedButton_2->updateLeds();
    last_action_time = millis();
}

// the setup function runs once when you press reset or power the board
void setup()
{
    Serial.begin(115200);
    pinMode(DATA_PIN, OUTPUT);
    pinMode(ALIAS_PIN, INPUT);
    pinMode(HUE_PIN, INPUT);
    pinMode(VALUE_PIN, INPUT);
    pinMode(SATURATION_PIN, INPUT);
    pinMode(BREITE_PIN, INPUT);
    pinMode(SHIFT_PIN, INPUT);

    ctrl_strip = &strip.addLeds<WS2812B, DATA_PIN>(leds, STRIP_LED_COUNT);

    myLedButton_1 = new CButtonWithLeds<BUTTON_1_LED_PIN>(BUTTON_1_PIN, BUTTON_1_LED_COUNT, BUTTON_1_LED_BRIGHTNESS, clickButton_1);
    myLedButton_2 = new CButtonWithLeds<BUTTON_2_LED_PIN>(BUTTON_2_PIN, BUTTON_2_LED_COUNT, BUTTON_2_LED_BRIGHTNESS, clickButton_2);

    FastLED.clear();
    FastLED.show();
    FastLED.setDither(DISABLE_DITHER);
    last_action_time = millis();
    last_elapsed_time = 0;
    //Serial.println("ready.");
}

void loop()
{
    myLedButton_1->tick();
    myLedButton_2->tick();

    static CPotiWert breite_poti = CPotiWert(BREITE_PIN, "'breite");
    static CPotiWert shift_poti = CPotiWert(SHIFT_PIN, "shift");
    static CPotiWert hue_poti = CPotiWert(HUE_PIN, "hue");
    static CPotiWert saturation_poti = CPotiWert(SATURATION_PIN, "saturation");
    static CPotiWert value_poti = CPotiWert(VALUE_PIN, "value");
    static CPotiWert alias_poti = CPotiWert(ALIAS_PIN, "alias");

    int breite = breite_poti.read_mapped(0, STRIP_LED_COUNT + 1);
    int shift_bereich = (STRIP_LED_COUNT - breite);
    int shift = shift_poti.read_mapped(0, shift_bereich + 1);

    int h = hue_poti.read_rgb();
    int s = saturation_poti.read_rgb();
    int v = value_poti.read_rgb();
    int a = alias_poti.read_rgb();

    int rotation_speed = myLedButton_1->getValue();
    int timeout_value = myLedButton_2->getValue();
    boolean changed = breite_poti.has_changed(true) 
        || shift_poti.has_changed(true) 
        || hue_poti.has_changed(true) 
        || saturation_poti.has_changed(true) 
        || value_poti.has_changed(true) 
        || alias_poti.has_changed(true)
        || myLedButton_1->has_changed(true)
        || myLedButton_2->has_changed(true);

    if ( changed ) {
        last_action_time = millis();
        Serial.println(
            "hue: " + String(h) +
            "\tsaturation: " + String(s) +
            "\tvalue: " + String(v) +
            "\talias: " + String(a) +
            "\tbreite: " + String(breite) +
            "\tshift: " + String(shift) +
            "\tshift_bereich: " + String(shift_bereich)
        );
    }

    long elapsed_time = (millis() - last_action_time) / 1000;
    long timeout = timeout_value > 0 ? timeouts[timeout_value - 1] : 10000000 + elapsed_time;

    if (elapsed_time >= timeout) {
        if ( last_elapsed_time != -1 ) {
            last_elapsed_time = -1;
            //Serial.println("off");

            for (int i = 0; i < STRIP_LED_COUNT; i++)
                leds[i] = CRGB::Black;
            ctrl_strip->showLeds();            
        }
    }
    else {
        long restzeit = timeout - elapsed_time;
        long restzeitgrenze = min(60, (20 * timeout) / 100); // 10 % des timeouts aber höchstens 1 minute

        if ( last_elapsed_time != elapsed_time ) {
            //Serial.println("elapsed time " + String(elapsed_time) + ", timeout in: " + String(timeout) + " Restzeit: " + String(restzeit) + " Restzeitgrenze: " + String(restzeitgrenze));
            last_elapsed_time = elapsed_time;
        }
        if ( changed || rotation_speed || restzeit <= restzeitgrenze)
        {
            int fade_faktor = 255;
            // wenn restzeitgrenze unterschritten, dann nach dunkel faden
            if ( restzeit <= restzeitgrenze ) {
                long praezise_restzeit = max(0, timeout * 1000 - (millis() - last_action_time));
                fade_faktor = map(praezise_restzeit, 0, 1000*restzeitgrenze, 0, 255);
                //Serial.println("praezise restzeit: " + String(praezise_restzeit) + " -> fade: " + String(fade_faktor) + " Restzeitgrenze: " + String(restzeitgrenze));
            }

            uint32_t timed_color_shift = millis()/(1000/(rotation_speed * rotation_speed * rotation_speed * rotation_speed)) % 255;
            // Serial.println("speed: " + String(rotation_speed) + "v: " + String(millis()/1000) + ": " + timed_color_shift);

            // Turn the first led red for 1 second
            for (int i = 0; i < STRIP_LED_COUNT; i++)
            {

                if (i >= shift && i < shift + breite)
                {
                    int ma = map(i - shift, 0, breite, 0, a * 5);
                    leds[i] = CHSV((h + ma + timed_color_shift) % 255, s, v*fade_faktor/255);
                }
                else
                {
                    leds[i] = CRGB::Black;
                }
            }
            ctrl_strip->showLeds();
        }
    }
}
