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
  Serial.println("Use 's' and 'd' to go one degree to the left or right.");
  Serial.println("Use 'a' and 'f' to go 10 degrees to the left or right.");
  Serial.println("Use 'x' to exit.");

  while (Serial.read() != -1) ;

  int deg = 0;

  while (true) {
    char ch = Serial.read();

    if (ch == 'x') break;
    if (ch == 'a') {
      deg -= 10;
      if (deg < 0) deg = 0;
      servo.write(deg);
      Serial.printf("\r%d   ", deg);
    }
    else if (ch == 's') {
      deg--;
      if (deg < 0) deg = 0;
      servo.write(deg);
      Serial.printf("\r%d   ", deg);
    }
    else if (ch == 'd') {
      deg++;
      if (deg >= 180) deg = 180;
      servo.write(deg);
      Serial.printf("\r%d   ", deg);
    }
    else if (ch == 'f') {
      deg += 10;
      if (deg >= 180) deg = 180;      
      servo.write(deg);
      Serial.printf("\r%d   ", deg);
    }
  }

  
  pinMode(pin, INPUT);
}

void 
Tests::motor_set(int pin, int level)
{
  int pulseStart, timer;
  
  //Write all motor pins high
  digitalWrite(pin, HIGH);
  pulseStart = micros();

  //Write each motor pin low as correct pulse length is reached
  while (1) { //keep going until final (3rd) pulse is finished, then done
    timer = micros();
    if (level <= (timer - pulseStart)) {
      digitalWrite(pin, LOW);
      break;
    }
  }
}

void 
Tests::motor(int pin, int freq)
{
  Serial.printf("Motor Test using pin #%d at frequency %d Hz.\n", pin, freq);
  Serial.println("Use 's' and 'd' to get -/+ one uSec change.");
  Serial.println("Use 'a' and 'f' to get -/+ ten uSec change.");
  Serial.println("Use 'x' to exit.");

  while (Serial.read() != -1) ;

  int level = 125;

  pinMode(pin, OUTPUT);
  delay(10);

  digitalWrite(pin, LOW);
  delay(10);

  Serial.printf("\r%d   ", level);

  while (true) {
    unsigned long current_time;

    current_time = micros();      

    if (Serial.available() > 0) {
      char ch = Serial.read();

      if (ch == 'x') break;
      if (ch == 'a') {
        level -= 10;
        if (level < 125) level = 125;
        Serial.printf("\r%d   ", level);
      }
      else if (ch == 's') {
        level--;
        if (level < 125) level = 125;
        Serial.printf("\r%d   ", level);
      }
      else if (ch == 'd') {
        level++;
        if (level > 250) level = 250;
        Serial.printf("\r%d   ", level);
      }
      else if (ch == 'f') {
        level += 10;
        if (level > 250) level = 250;      
        Serial.printf("\r%d   ", level);
      }
    }

    motor_set(pin, level);

    float invFreq = 1.0 / freq * 1000000.0;
    unsigned long checker = micros();
    
    //Sit in loop until appropriate time has passed
    while (invFreq > (checker - current_time)) {
      checker = micros();
    }
  }

  motor_set(pin, 120);
}

void 
Tests::motor_calibration(int pin)
{
  Serial.printf("Motor Calibration using pin #%d.\n", pin);
  Serial.println("Use 's' to get low value (125 uSec).");
  Serial.println("Use 'd' to get high value (250 uSec).");
  Serial.println("Use 'x' to exit.");

  while (Serial.read() != -1) ;

  int level = 250;

  pinMode(pin, OUTPUT);
  delay(100);  
  digitalWrite(pin, LOW);  
  delay(100);

  Serial.printf("\r%d   ", level);

  while (true) {
    unsigned long current_time;

    current_time = micros();      

    if (Serial.available() > 0) {
      char ch = Serial.read();

      if (ch == 'x') break;
      else if (ch == 's') {
        level = 125;
        Serial.printf("\r%d   ", level);
      }
      else if (ch == 'd') {
        level = 250;
        Serial.printf("\r%d   ", level);
      }
    }

    motor_set(pin, level);

    float invFreq = 1.0 / 2000 * 1000000.0;
    unsigned long checker = micros();
    
    //Sit in loop until appropriate time has passed
    while (invFreq > (checker - current_time)) {
      checker = micros();
    }
  }

  motor_set(pin, 120);
}