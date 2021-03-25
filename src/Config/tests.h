#pragma once

class Tests
{
  public:
    void servo(int pin);
    void motor(int pin, int freq);
    void motor_calibration(int pin);
  private:
    void motor_set(int pin, int level);
};

#ifdef __TESTS__
  Tests tests;
#else
  extern Tests tests;
#endif
