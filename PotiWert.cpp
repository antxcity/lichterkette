#include "PotiWert.h"

int CPotiWert::read()
{
    int potiWert = analogRead(m_pin);
    smoothWert = 0.7 * smoothWert + 0.3 * analogRead(m_pin);
    int differenz = abs(potiWert - potiWertAlt);
    int quickSmooth;

    if (differenz <= 10 && quickAktiv == false)
    {
        quickSmooth = 0.7 * smoothWert + 0.3 * analogRead(m_pin);
    }
    else if (differenz <= 10 && quickAktiv == true)
    {
        quickSmooth = analogRead(m_pin);
        x++;
    }
    else
    {
        quickSmooth = analogRead(m_pin);
        quickAktiv = true;
    };

    if (x >= 3)
    {
        x = 0;
        quickAktiv = false;
    };

    potiWertAlt = potiWert;

    return quickSmooth;
}

boolean CPotiWert::has_changed(boolean reset = false) 
{
    boolean c = changed;
    if (reset) 
        reset_changed();
    return c;
}

void CPotiWert::reset_changed()
{
    changed = false;
}

int CPotiWert::read_mapped(int minimum, int maximum)
{
    int v = map(read(), 0, 1021, minimum, maximum);
    if ( v != last_value ) {
        /*
        Serial.println(
            "poti value changed: '" + _name + "': " + String(last_value) + " -> " + String(v)
        );
        */
        last_value = v;
        changed = true;
    }
    return v;
}