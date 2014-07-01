#ifndef SonarSR04_h
#define SonarSR04_h

#include <Arduino.h>

class SonarSR04 {
public:
  void attach(byte trigPin, byte echoPin, int maxDistance){
    trigPin_ = trigPin;
    echoPin_ = echoPin;
    maxDistance_ = maxDistance;
    pinMode(trigPin_, OUTPUT); // Trig
    digitalWrite(trigPin_, LOW);
    pinMode(echoPin_, INPUT);  // Echo
  }
  int ping(){
    unsigned long duration, timeout;
    int distance;

    digitalWrite(trigPin_, LOW);
    digitalWrite(trigPin_, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin_, LOW);

    timeout = maxDistance_*5.8;
    duration = pulseIn(echoPin_, HIGH, timeout);

    // converting to mm
    distance = duration / 5.8;

    if (distance > maxDistance_ || duration==0) distance = maxDistance_;

    return distance;
  }
private:
  byte trigPin_;
  byte echoPin_;
  int maxDistance_;
};

#endif

