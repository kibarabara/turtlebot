#include <Servo.h>

class Leg {
public:
  Leg(int shoulderCenter, char shoulderSign, int kneeCenter, int kneeSign)
    : shoulderCenter_(shoulderCenter)
    , shoulderSign_(shoulderSign)
    , kneeCenter_(kneeCenter)
    , kneeSign_(kneeSign)
  { }

  void attach(int shoulderPin, int kneePin) {
    shoulder_.attach(shoulderPin);
    knee_.attach(kneePin);
  }
      
  int getShoulderDelta() { return shoulderSign_ * shoulder_.read() - shoulderCenter_; }
  int getKneeDelta() { return kneeSign_ * knee_.read() - kneeCenter_; }
  
  void set(int shoulderDelta, int kneeDelta) {
    int shoulderTarget = shoulderCenter_ + shoulderSign_ * shoulderDelta;
    
    if (shoulder_.read() != shoulderTarget) {
      shoulder_.write(shoulderTarget); 
    }
    
    int kneeTarget = kneeCenter_ + kneeSign_ * kneeDelta;
    if (knee_.read() != kneeTarget) {
      knee_.write(kneeTarget);
    }
  }
  
  void setTarget(int shoulderTarget, int kneeTarget, int delay)
  {
    shoulderTarget_ = shoulderTarget;
    shoulderV_ = ((float)shoulderTarget - getShoulderDelta()) / delay;
    kneeTarget_ = kneeTarget;
    kneeV_ = ((float)kneeTarget - getKneeDelta()) / delay;
    
    targetTime_ = millis() + delay;
  }
 
  bool ease()
  {
    unsigned long curTime = millis();
    if (curTime >= targetTime_) {
      set(shoulderTarget_, kneeTarget_);
      return true; 
    }
    
    set(shoulderTarget_ - shoulderV_ * (targetTime_ - curTime), kneeTarget_ - kneeV_ * (targetTime_ - curTime));
    return false;
  }
  
private:
  Servo shoulder_;
  Servo knee_;
  const int shoulderCenter_;
  const char shoulderSign_;
  const int kneeCenter_;
  const char kneeSign_;
  
  float shoulderV_;
  float kneeV_;
  int shoulderTarget_;
  int kneeTarget_;
  unsigned long targetTime_; 
};

Leg frontRightLeg(60, +1, 90, +1);
Leg frontLeftLeg(120, -1, 90, -1);
Leg backLeftLeg(60, -1, 90, +1);
Leg backRightLeg(120, 1, 90, -1);

void easeAll()
{
  while(true) {
    delay(15);
    if (frontRightLeg.ease() && frontLeftLeg.ease() && backLeftLeg.ease() && backRightLeg.ease()) {
      break; 
    }
  }  
}


class Forward
{
public:
  Forward() : step_(0) { }
  
  void reset() { step_ = 0; }
  
  void performStep(int delay) 
  {
    frontRightLeg.setTarget(shoulderAngles_[step_], kneeAngles_[step_], delay);
    backLeftLeg.setTarget(shoulderAngles_[step_], kneeAngles_[step_], delay);
    frontLeftLeg.setTarget(-shoulderAngles_[step_], -kneeAngles_[step_], delay);
    backRightLeg.setTarget(-shoulderAngles_[step_], -kneeAngles_[step_], delay);   
  
    easeAll();
    
    step_ += 1;
    if (step_ == sizeof(shoulderAngles_)) {
      step_ = 0; 
    }   
  }
  
private:
  char step_;

  static const int shoulderAngles_[8];
  static const int kneeAngles_[8];
};

const int Forward::shoulderAngles_[] = {21, 0, -21, -30, -21, 0, 21, 30};
const int Forward::kneeAngles_[] = {-14, -20, -14, 0, 14, 20, 14, 0}; 

Forward forward;

void setup() {
  Serial.begin(38400);
  
  frontRightLeg.attach(12, 11);
  frontLeftLeg.attach(10, 9);
  backLeftLeg.attach(8, 7);
  backRightLeg.attach(6, 5);
  
  frontRightLeg.set(0, 0);
  backLeftLeg.set(0, 0);
  frontLeftLeg.set(0, 0);
  backRightLeg.set(0, 0);
}

void loop() {
  forward.performStep(100);
}
