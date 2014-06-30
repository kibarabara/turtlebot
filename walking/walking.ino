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
    
private:
  Servo shoulder_;
  Servo knee_;
  int shoulderCenter_;
  char shoulderSign_;
  int kneeCenter_;
  char kneeSign_;
};

Leg frontRightLeg(60, +1, 90, +1);
Leg frontLeftLeg(120, -1, 90, -1);
Leg backLeftLeg(60, -1, 90, +1);
Leg backRightLeg(120, 1, 90, -1);

class Easing {
public:
  Easing(Leg& leg)
    : leg_(leg), shoulderV_(0), kneeV_(0), shoulderTarget_(0), kneeTarget_(0), targetTime_(0)
  { }
  
  void setTarget(int shoulderTarget, int kneeTarget, int delay)
  {
    shoulderTarget_ = shoulderTarget;
    shoulderV_ = ((float)shoulderTarget - leg_.getShoulderDelta()) / delay;
    kneeTarget_ = kneeTarget;
    kneeV_ = ((float)kneeTarget - leg_.getKneeDelta()) / delay;
    
    targetTime_ = millis() + delay;
  }
 
  bool ease()
  {
    unsigned long curTime = millis();
    if (curTime >= targetTime_) {
      leg_.set(shoulderTarget_, kneeTarget_);
      return true; 
    }
    
    leg_.set(shoulderTarget_ - shoulderV_ * (targetTime_ - curTime), kneeTarget_ - kneeV_ * (targetTime_ - curTime));
    return false;
  } 
    
private:
  Leg& leg_;
  float shoulderV_;
  float kneeV_;
  int shoulderTarget_;
  int kneeTarget_;
  unsigned long targetTime_; 
};

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
//
//int frShoulderAngles[] = {30, -30, -30, 30};
//int frKneeAngles[] = {-20, -20, 20, 20};

int shoulderAmpl = 30;
int kneeAmpl = 20;
float shoulderPhase = 0.0;
float kneePhase = PI / 2;

  Easing fr(frontRightLeg);  
  Easing bl(backLeftLeg);
  Easing fl(frontLeftLeg);
  Easing br(backRightLeg);
  
void loop() {
  fr.setTarget(shoulderAmpl * sin(shoulderPhase), kneeAmpl * sin(kneePhase), 100);
  bl.setTarget(shoulderAmpl * sin(shoulderPhase), kneeAmpl * sin(kneePhase), 100);
  fl.setTarget(-shoulderAmpl * sin(shoulderPhase), -kneeAmpl * sin(kneePhase), 100);
  br.setTarget(-shoulderAmpl * sin(shoulderPhase), -kneeAmpl * sin(kneePhase), 100);   
  while(true) {
    delay(15);
    bool allEased = fl.ease() && br.ease() && fr.ease() && bl.ease();
    if (allEased) {
      break; 
    }
  }
    
  shoulderPhase += PI / 6;
  kneePhase += PI / 6;
}
