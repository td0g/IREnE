void setMotorSpeed(byte _s){//   MOTOR CONTROLS   ############################################################################################################################################################
  if (_s){
  a.setMaxSpeed(motorTravelSpeed);
  b.setMaxSpeed(motorTravelSpeed);
  c.setMaxSpeed(motorTravelSpeed / 2);
  a.setAcceleration(motorTravelAcc);
  b.setAcceleration(motorTravelAcc * 4);
  c.setAcceleration(motorTravelAcc / 2);
  }
  else {
  a.setMaxSpeed(motorTravelSpeed / 8);
  b.setMaxSpeed(motorTravelSpeed / 4);
  c.setMaxSpeed(motorTravelSpeed / 16);
  a.setAcceleration(motorTravelAcc / 8);
  b.setAcceleration(motorTravelAcc / 4);
  c.setAcceleration(motorTravelAcc / 8);
  }
}


boolean runMotor(){
  if (b.isRunning()) bEnable;
  else {
    if (b.currentPosition() < bMin) {
      b.setCurrentPosition(bMin);
      for (byte i = 0; i < MAX_TARGETS; i++){
        if (targets[i][1]) targets[i][1] -= (bMin - b.currentPosition());
      }
    }
    bDisable;
  }
  if (b.run()){
   a.run();
   c.run();
  return true;
  }
  if (a.run()) {
    c.run();
    return true;
  }
  if (c.run()) return true;
  return false;
}



void motorStop(){
  a.stop();
  b.stop();
  c.stop();
}


void cRotate(long _c){
  long _a;
  long _b;
  int _C = C * 2;
  _C *= PI;
  if (_c < c.currentPosition()) _C *= -1;
  _a = _c - c.currentPosition();  //7700
  _b = _a - _C;
  while (abs(_b) < abs(_a)) {
    c.setCurrentPosition(c.currentPosition() + _C);
    _a = _c - c.currentPosition();
    _b = _a - _C;
  }
  c.moveTo(_c);
}
