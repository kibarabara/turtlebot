#include <Servo.h>

// ------------------------- Legs -----------------------------

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

// Counterclockwise from front right leg.
// Position is approximately neutral: shoulder is along the radius, knee joints are horizontal.

Leg legs[4] = {Leg(60, +1, 90, +1), Leg(120, -1, 90, -1), Leg(60, -1, 90, +1), Leg(120, 1, 90, -1)};
Leg& frontRightLeg = legs[0];
Leg& frontLeftLeg = legs[1];
Leg& backLeftLeg = legs[2];
Leg& backRightLeg = legs[3];

void easeAll()
{
  while(true) {
    delay(15);
    if (legs[0].ease() && legs[1].ease() && legs[2].ease() && legs[3].ease()) {
      break; 
    }
  }  
}

// ------------------------- Movement ----------------------------

// Approximate ellipse : shoulderAngle = A \sin \phi; knee angle = B \cos \phi.
const int shoulderAngles[] = {0, 21, 30, 21, 0, -21, -30, -21};
const int kneeAngles[] = {20, 14, 0, -14, -20, -14, 0, 14}; 

class Movement
{
public:
  Movement(const char* shoulderPattern) : step_(0), shoulderPattern_(shoulderPattern) { }
  
  void reset() { step_ = 0; }
  
  void performStep(int delay) 
  {
    static const char kneePattern[] = {1, -1, 1, -1};
    
    for (int iLeg = 0; iLeg < 4; ++iLeg) {
      legs[iLeg].setTarget(shoulderAngles[step_]*shoulderPattern_[iLeg], kneeAngles[step_]*kneePattern[iLeg], delay);
    }
    
    easeAll();
    
    step_ += 1;
    if (step_ == sizeof(shoulderAngles)) {
      step_ = 0; 
    }   
  }
  
private:
  char step_;
  const char* shoulderPattern_;
};

char forwardPattern[4] = {1, -1, 1, -1};
Movement forward(forwardPattern);

char turnPattern[4] = {1, 1, -1, -1};
Movement turn(turnPattern);

void setup() {
  Serial.begin(38400);
  
  frontRightLeg.attach(12, 11);
  frontLeftLeg.attach(10, 9);
  backLeftLeg.attach(8, 7);
  backRightLeg.attach(6, 5);
  
  for (int i = 0; i < 4; ++i) {
    legs[i].set(0, 0);
  }
}

void loop() {
  forward.performStep(100);
}
