#define __TESTS__
#include "tests.h"

#include "Arduino.h"
#include <PWMServo.h>

void 
Tests::servo(int pin)
{
  PWMServo servo;

  servo.attach(pin, 1000, 2000); //pin, min PWM value, max PWM value

  if (servo.attached() == 0) {
    Serial.printf("Unable to attach to servo on pin %d\n", pin);
    return;
  }

  Serial.println("Servo Test.");
  Serial.println("Use 's' and 'd' to go one degree on left or right.");
  Serial.println("Use 'a' and 'f' to go 10 degrees on left or right.");
  Serial.println("Use 'x' to exit.");

  while (Serial.read() != -1) ;

  int deg = 0;

  while (true) {
    char ch = Serial.read();

    if (ch == 'x') break;
    if (ch == 'a') {
      deg -= 10;
      if (deg < 0) deg = 180 + deg;
      servo.write(deg);
      Serial.printf("\r%d   ", deg);
    }
    else if (ch == 's') {
      deg--;
      if (deg < 0) deg = 180 + deg;
      servo.write(deg);
      Serial.printf("\r%d   ", deg);
    }
    else if (ch == 'd') {
      deg++;
      if (deg >= 180) deg = deg - 180;
      servo.write(deg);
      Serial.printf("\r%d   ", deg);
    }
    else if (ch == 'f') {
      deg += 10;
      if (deg >= 180) deg = deg - 180;      
      servo.write(deg);
      Serial.printf("\r%d   ", deg);
    }
  }

  //servo.detach();
}

void 
Tests::motor(int pin)
{

}
