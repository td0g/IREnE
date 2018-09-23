void runProgram(byte _type){//   THE INTERVALOMETER PROGRAM   ############################################################################################################################################################
//Declare variables used in loops
  long xDelta;
  long yDelta;
  unsigned int recIndex = findIndexEEPROM();
  EEPROM.update(recIndex + EEPROM_TYPE, _type);
  EEPROMWriteint(recIndex + EEPROM_ABC, A);
  EEPROMWriteint(recIndex + EEPROM_ABC + 2, B);
  EEPROMWriteint(recIndex + EEPROM_ABC + 4, C);
  EEPROMWriteint(recIndex + EEPROM_COUNT, numberShots);
  for (byte i = 0; i < MAX_TARGETS; i++){
    EEPROMWritelong(recIndex + EEPROM_TARGETS + i * 12, targets[i][0]);
    EEPROMWritelong(recIndex + EEPROM_TARGETS + 4 + i * 12, targets[i][1]);
    EEPROMWritelong(recIndex + EEPROM_TARGETS + 8 + i * 12, targets[i][2]);
  }
//Decrement numberShots by one - easier maths :)
  numberShots --;
  shutterTime *= 1000;
//Tell the users that we are gonna do this
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Moving to start");
//Calc thetaDelta
  if (!_type){
    thetaEND = IREnE.acTOtheta(targets[1][0],targets[1][2]);
    thetaSTART = IREnE.acTOtheta(targets[0][0],targets[0][2]);
  }
  float thetaDelta = thetaEND - thetaSTART;

  if (!_type) {  
    xDelta = IREnE.abTOx(targets[1][0],targets[1][1]) - IREnE.abTOx(targets[0][0],targets[0][1]);
    yDelta = IREnE.abTOy(targets[1][0],targets[1][1]) - IREnE.abTOy(targets[0][0],targets[0][1]);
    xDelta /= numberShots;
    yDelta /= numberShots;
    #ifdef DEBUG
      Serial.println(F("  RUNNING EnE"));
      Serial.print(F("xDelta:"));
      Serial.println(xDelta);
      Serial.print(F("yDelta"));
      Serial.println(yDelta);
      Serial.print(F("thetaDelta:"));
      Serial.println(thetaDelta);
      Serial.print(F("Current a:"));
      Serial.print(a.currentPosition());
      Serial.print(F(" b:"));
      Serial.print(b.currentPosition());
      Serial.print(F(" c:"));
      Serial.println(c.currentPosition());
      Serial.println();
    #endif
    lcd.setCursor(0,1);
    lcd.print("tD:");
    lcd.print(thetaDelta);
    //Move to beginning
    a.moveTo(targets[0][0]);
    b.moveTo(targets[0][1]);
    cRotate(targets[0][2]);
    while (runMotor()){
      analogReadAll();
      if (BUTTON_TL) {
        numberShots++;
        shutterTime /= 1000;
        menuPosition = 6;
        motorStop();
        setMotorSpeed(1);
      }
    }  
  }
  else {
    #ifdef DEBUG
      Serial.println(F("  RUNNING IR"));
      Serial.print(F("theta START, END, D:"));
      Serial.print(thetaSTART);
      Serial.print(F(", "));
      Serial.print(thetaEND);
      Serial.print(F(", "));
      Serial.println(thetaDelta);
      Serial.print(F("Current a:"));
      Serial.print(a.currentPosition());
      Serial.print(F(" b:"));
      Serial.print(b.currentPosition());
      Serial.print(F(" c:"));
      Serial.println(c.currentPosition());
      Serial.println();
    #endif
  }

  EEPROM.put(recIndex + EEPROM_THETA, thetaSTART);
  EEPROM.put(recIndex + EEPROM_THETA +  4, thetaEND);
  EEPROMWritelong(recIndex + EEPROM_X, IREnE.abTOx(targets[0][0],targets[0][1]));
  EEPROMWritelong(recIndex + EEPROM_X +  4, IREnE.abTOx(targets[1][0],targets[1][1]));
  EEPROMWritelong(recIndex + EEPROM_Y, IREnE.abTOy(targets[0][0],targets[0][1]));
  EEPROMWritelong(recIndex + EEPROM_Y +  4, IREnE.abTOy(targets[1][0],targets[1][1]));
  EEPROMWritelong(recIndex + EEPROM_DIST, distT);
  thetaDelta /= numberShots;
//Slow down motors
  setMotorSpeed(0);
//Get timer set up
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(F("Waiting        s"));
  delay(calcStabilizeTime());
  unsigned long _startTime = millis();
  unsigned int _countDown = 0;
  unsigned long _timer = millis();
  unsigned long _timerInt = totalTime;
  _timerInt /= numberShots;
  _timer += _timerInt;
//Start showing useful stuff on LCD
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print(F("1   / "));
  lcd.print(numberShots + 1);
  lcd.setCursor(0,0);
  lcd.print(F("Shot"));
//Take the first photo
  SHUTTER_OPEN;
  delay(shutterTime);
  SHUTTER_CLOSE;
//Get the current position (FOR ExtensioN/Eggtimer)
  if (!_type) {
    calcCurrentxy();
    xT = x;
    yT = y;
    thetaT = theta; 
  } 
//Start the cycle - MOVE, STABILIZE/DWELL, SHOOT
  for (unsigned int i = 0; i < numberShots; i++){
    thetaT += thetaDelta;
    xT += xDelta;
    yT += yDelta;
    
    if (_type) moveToObjThetaDist();
    else moveToxytheta();
    
    lcd.setCursor(0,0);
    lcd.print(F(" Mov"));
    while (runMotor()){
      analogReadAll();
      if (BUTTON_TL) {
        motorStop();
        setMotorSpeed(1);
        shutterTime /= 1000;
        numberShots++;
      }
    }
    lcd.setCursor(0,0);
    lcd.print(F("Wait"));
    lcd.setCursor(0,1);
    lcd.print(i + 2);

    unsigned long _nextTime = millis() + calcStabilizeTime();
    _timer += _timerInt;
    lcd.setCursor(0,1);
    lcd.print(i+2);
    while (millis() < max(_nextTime, _timerInt)){
      unsigned long _countDownNow = millis();
      _countDownNow -= _startTime;
      _countDownNow /= 1000;
      _countDownNow = totalTime - _countDownNow;
      if (_countDownNow != _countDown){
        _countDown = _countDownNow;
        lcd.setCursor(8, 0);
        lcd.print(_countDown);
        lcd.print(" ");
      }
      analogReadAll();
      if (BUTTON_TL) {
        numberShots += 1;
        shutterTime /= 1000;
        setMotorSpeed(1);
        motorStop();
        _nextTime = millis();
        _timerInt = millis();
      }
    }
    lcd.setCursor(0,0);
    lcd.print(F("Shot"));
    SHUTTER_OPEN;
    delay(shutterTime);
    SHUTTER_CLOSE;
    delay(500);
    _timer += _timerInt;
  }
    
  setMotorSpeed(1);
  numberShots++;
  shutterTime /= 1000;
  menuPosition = 0;
}

unsigned int calcStabilizeTime(){
  unsigned int _t = stabTimeMax;
  _t -= stabTimeMin;
  unsigned long _b = b.currentPosition();
  _b -= bMin;
  unsigned long _b1 = bMax;
  _b1 -= bMin;
  float _d;
  _d = _b / _b1;
  _t *= _d;
  _t += stabTimeMin;
  return _t;
}
