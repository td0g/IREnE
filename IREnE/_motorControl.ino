void setMotorSpeed(float _s){//   MOTOR CONTROLS   ############################################################################################################################################################
  if (_s > 0){
    a.setMaxSpeed(motorMaxSpeed[0] * _s);
    b.setMaxSpeed(motorMaxSpeed[1] * _s);
    c.setMaxSpeed(motorMaxSpeed[2] * _s);  //could be 3200
    a.setAcceleration(motorTravelAcc);
    b.setAcceleration(motorTravelAcc * 4);
    c.setAcceleration(8000);
  }
  else {
    a.setMaxSpeed(motorTravelSpeed / 8);
    b.setMaxSpeed(motorTravelSpeed / 4);
    c.setMaxSpeed(motorTravelSpeed / 8);
    a.setAcceleration(motorTravelAcc / 8);
    b.setAcceleration(motorTravelAcc / 4);
    c.setAcceleration(motorTravelAcc / 2);
  }
}

boolean runMotor(){
  if (!b.isRunning() && b.currentPosition() < bMin) {
    b.setCurrentPosition(bMin);
    for (byte i = 0; i < targetCount; i++)targets[i][1] += (bMin - b.currentPosition());
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


void bMove(long _b){
  b.moveTo(min(_b, bMax));
}


long cRot(long _c){
  long _a;
  long _b;
  long _C = C;
  _C *= 2;
  _C *= PI;
  if (_c < c.currentPosition()) _C *= -1;
  _a = _c - c.currentPosition();  //7700
  _b = _a - _C;
  while (abs(_b) < abs(_a)) {
    _c -= _C; //c.setCurrentPosition(c.currentPosition() + _C);
    _a = _c - c.currentPosition();
    _b = _a - _C;
  }
  return _c;
}
