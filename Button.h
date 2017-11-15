#ifndef Buttons_h
#define Buttons_h

#include "Arduino.h"

class Button
{
  public:
    Button(int _buttonPin);
    
    void process();
    void pressHandler(void (*f)());
    void releaseHandler(void (*f)());
    void (*pressCallback)();
    void (*releaseCallback)();

    byte buttonPin;
    bool firstCheck;
    bool buttonState;
    bool lastButtonState;
    unsigned long lastCheck;
    int checkInt;

  private:
};

#endif
