#pragma once

class Tests
{
  public:
    void servo(int pin);
    void motor(int pin);
};

#ifdef __TESTS__
  Tests tests;
#else
  extern Tests tests;
#endif
