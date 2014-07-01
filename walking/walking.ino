#include <Servo.h>
#include "SonarSR04.h"

#define SONAR_TRIG 2
#define SONAR_ECHO 3

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

SonarSR04 sonar;

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

// Equal interval sin table:
const float CIRCLE[] = {0, 0.71, 1.0, 0.71, 0, -0.71, -1.0, -0.71};

int wrapAround(int i)
{
  if (i >= sizeof(CIRCLE)) {
      return i - sizeof(CIRCLE);
  }
  return i;
}

struct LegAngles {
  int shoulder;
  int knee;
};

class Movement
{
public:
  Movement(const LegAngles* legsAmplitudes)
    : step_(0), legsAmplitudes_(legsAmplitudes)
  { }

  void reset() { step_ = 0; }

  void performStep(int delay)
  {
    for (int iLeg = 0; iLeg < 4; ++iLeg) {
      legs[iLeg].setTarget(CIRCLE[step_]*legsAmplitudes_[iLeg].shoulder, CIRCLE[wrapAround(step_ + sizeof(CIRCLE)/4)]*legsAmplitudes_[iLeg].knee, delay);
    }

    easeAll();

    step_ = wrapAround(step_ + 1);
  }

private:
  char step_;
  const LegAngles* legsAmplitudes_;
};

LegAngles FORWARD_AMPLITUDES[4] = {{30, 20}, {-30, -20}, {30, 20}, {-30, -20}};
Movement forward(FORWARD_AMPLITUDES);

LegAngles TURN_AMPLITUDES[4] = {{30, 20}, {30, -20}, {-30, 20}, {-30, -20}};
Movement turn(TURN_AMPLITUDES);

void setup() {
  Serial.begin(38400);

  frontRightLeg.attach(12, 11);
  frontLeftLeg.attach(10, 9);
  backLeftLeg.attach(8, 7);
  backRightLeg.attach(6, 5);
  
  sonar.attach(SONAR_TRIG, SONAR_ECHO, 500);


  for (int i = 0; i < 4; ++i) {
    legs[i].set(0, 0);
  }
}

void loop() {
  if (sonar.ping()>100) {
    forward.performStep(100);
  } else {
    turn.performStep(100);
  }  
}
