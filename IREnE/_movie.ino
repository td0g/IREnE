void movie(float _x, float _y, float objThetaD, unsigned int _sections){
  byte _calcNextPoint;
  float nextSpeed = 0;
  unsigned long _lastTime = millis() - (movieAccelTime * MOVIE_JERK * 1000);//Start at 5% max speed
  float _accelRate = 0;
  movieNextPos[0] = a.currentPosition();
  movieNextPos[1] = b.currentPosition();
  movieNextPos[2] = c.currentPosition();
  if (_sections > 1){
    _x /= _sections;
    _y /= _sections;
    objThetaD /= _sections;
  }
  _sections++;
  while (buttonPressed || _accelRate > MOVIE_JERK || _sections){
    if (_sections > 0) _sections--;
    _calcNextPoint = 1;
    while (COORDINATED_MOVE || _calcNextPoint) {
      COORDINATED_MOVE;
      analogReadAll();
      if (_calcNextPoint){         //Let's compute the next target ahead of time
        _calcNextPoint = 0;
        COORDINATED_MOVE;
          if (!buttonPressed) _accelRate = _accelRate - (millis() - _lastTime) / (1000 * movieAccelTime);
          else {
            _accelRate = _accelRate + (millis() - _lastTime) / (1000 * movieAccelTime);
            _accelRate = min(_accelRate, 1);
          }
          _lastTime = millis();
          COORDINATED_MOVE;
 //Now we have _accelRate.
 
        if (objThetaD){ //IR
          thetaT = thetaT + (objThetaD *  _accelRate);
          COORDINATED_MOVE;
          moveToObjThetaDist();
        }
        else {
          xT += _x * _accelRate;
          yT += _y * _accelRate;
        }
        COORDINATED_MOVE;
  //Now we have a target physical position
      
        long _a = movieNextPos[0];
        long _b = movieNextPos[1];
        movieNextPos[0] = IREnE.xyTOa(xT, yT);  //About 224 micros
        COORDINATED_MOVE;
        movieNextPos[1] = min(bMax, IREnE.xyTOb(xT, yT));  //About 76 micros
        COORDINATED_MOVE;
        movieNextPos[2] = IREnE.xythetaTOc(xT, yT, thetaT);   //About 260 micros
        COORDINATED_MOVE;
  //Now we have motor target positions
        if (movieSpeed){
          _a -= movieNextPos[0];
          _b -= movieNextPos[1];
          _a = abs(_a);
          _b = abs(_b);
          if (_a > _b) nextSpeed = _a;
          else nextSpeed = _b * -1;
          nextSpeed *= movieSpeed;
          COORDINATED_MOVE;
        }
      }
    }
    
#ifdef CNC_PANCAKE
    if (!pancakePrinting) movieNextPos[2] = cRot(movieNextPos[2]);
    else if (pancakePrinting == 2) {
      //ADVANCE c motor
    }
#else
    movieNextPos[2] = cRot(movieNextPos[2]);
#endif

    if (!movieSpeed) setMotorSpeed(_accelRate);
    else{
      if (nextSpeed > 0) {
        nextSpeed = min(nextSpeed, motorMaxSpeed[0]);
        a.setMaxSpeed(nextSpeed);
        b.setMaxSpeed(motorMaxSpeed[1]);
      }
      else {
        nextSpeed *= -1;
        nextSpeed = min(nextSpeed, motorMaxSpeed[1]);
        a.setMaxSpeed(motorMaxSpeed[0]);
        b.setMaxSpeed(nextSpeed); //Cause setMaxSpeed alread checks for negative values
      }
    }
    COORDINATED_MOVE_SETUP(movieNextPos);
    nextSpeed = 0;
  }
  //Cleanup on exit
  setMotorSpeed(1);
  motorStop();
  calcCurrentxy();
  xT = x;
  yT = y;
  thetaT = theta;
}
