#include <Servo.h>

class Leg {
public:
  Leg(char shoulderCenter, char shoulderDelta, char kneeDelta)
    : shoulderCenter_(shoulderCenter)
    , shoulderDelta_(shoulderDelta)
    , kneeDelta_(kneeDelta)
  { }

  void attach(int shoulderPin, int kneePin) {
    shoulder_.attach(shoulderPin);
    knee_.attach(kneePin);
  }
  
  void upForward() { set(shoulderCenter_ + shoulderDelta_, 90 + kneeDelta_); }
  void downForward() { set(shoulderCenter_ + shoulderDelta_, 90 - kneeDelta_); }
  void upBackward() { set(shoulderCenter_ - shoulderDelta_, 90 + kneeDelta_); }
  void downBackward() { set(shoulderCenter_ - shoulderDelta_, 90 - kneeDelta_); }
    
private:
  void set(int shoulderAngle, int kneeAngle) {
    if (shoulder_.read() != shoulderAngle) {
      shoulder_.write(shoulderAngle); 
    }
    if (knee_.read() != kneeAngle) {
      knee_.write(kneeAngle);
    }
  }
  
private:
  Servo shoulder_;
  Servo knee_;
  char shoulderCenter_;
  char shoulderDelta_;
  char kneeDelta_;
};

Leg frontRightLeg(60, 10, 5);
Leg frontLeftLeg(120, -10, -5);
Leg backLeftLeg(60, -10, 5);
Leg backRightLeg(120, 10, -5);

void setup() {
  Serial.begin(38400);
//  
  frontRightLeg.attach(12, 11);
  frontLeftLeg.attach(10, 9);
  backLeftLeg.attach(8, 7);
  backRightLeg.attach(6, 5);
  
  frontRightLeg.upForward();
  backLeftLeg.upForward();
  frontLeftLeg.downBackward();
  backRightLeg.downBackward();
}

void loop() {
  delay(200);
  frontRightLeg.downForward();
  backLeftLeg.downForward();
  frontLeftLeg.upBackward();
  backRightLeg.upBackward();
  
  delay(200);
  frontRightLeg.downBackward();
  backLeftLeg.downBackward();
  frontLeftLeg.upForward();
  backRightLeg.upForward();
  
  delay(200);
  frontRightLeg.upBackward();
  backLeftLeg.upBackward();
  frontLeftLeg.downForward();
  backRightLeg.downForward();
  
  delay(200);
  frontRightLeg.upForward();
  backLeftLeg.upForward();
  frontLeftLeg.downBackward();
  backRightLeg.downBackward();
}
