void runProgram(byte _type){//   THE INTERVALOMETER PROGRAM   ############################################################################################################################################################
  unsigned long _totalTime = (long)totalTime * 1000;
  unsigned long _shutterTime = (long)shutterTime * 1000;
  unsigned int _numberShots = numberShots - 1;

//Declare variables used in loops
  long xDelta;
  long yDelta;
  float thetaDelta;
  unsigned int recIndex = findIndexEEPROM();
  EEPROM.update(recIndex + EEPROM_TYPE, _type);
  EEPROMWriteint(recIndex + EEPROM_ABC, A);
  EEPROMWriteint(recIndex + EEPROM_ABC + 2, B);
  EEPROMWriteint(recIndex + EEPROM_ABC + 4, C);
  EEPROMWriteint(recIndex + EEPROM_COUNT, numberShots);
  for (byte i = 0; i < 2; i++){
    EEPROMWritelong(recIndex + EEPROM_TARGETS + i * 12, targets[i][0]);
    EEPROMWritelong(recIndex + EEPROM_TARGETS + 4 + i * 12, targets[i][1]);
    EEPROMWritelong(recIndex + EEPROM_TARGETS + 8 + i * 12, targets[i][2]);
  }
  calcCurrDistToObject();
  thetaSTART = thetaO;
  thetaDelta = thetaEND - thetaSTART;
  thetaDelta /= numberShots;
  if (!_type) {
    thetaEND = IREnE.acTOtheta(targets[1][0],targets[1][2]);
    thetaSTART = IREnE.acTOtheta(targets[0][0],targets[0][2]);
    thetaDelta = thetaEND - thetaSTART;
    thetaDelta /= numberShots;
    
  //Tell the users that we are gonna do this
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Moving to start");
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
    //Move to beginning
    a.moveTo(targets[0][0]);
    b.moveTo(targets[0][1]);
    c.moveTo(cRot(targets[0][2]));
    while (runMotor()){
      analogReadAll();
      if (BUTTON_TL) {
        menuPosition = 6;
        motorStop();
        setMotorSpeed(1);
      }
    }  
  }
  #ifdef DEBUG
    else {
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
    }
  #endif

  EEPROM.put(recIndex + EEPROM_THETA, thetaSTART);
  EEPROM.put(recIndex + EEPROM_THETA +  4, thetaEND);
  EEPROMWritelong(recIndex + EEPROM_X, IREnE.abTOx(targets[0][0],targets[0][1]));
  EEPROMWritelong(recIndex + EEPROM_X +  4, IREnE.abTOx(targets[1][0],targets[1][1]));
  EEPROMWritelong(recIndex + EEPROM_Y, IREnE.abTOy(targets[0][0],targets[0][1]));
  EEPROMWritelong(recIndex + EEPROM_Y +  4, IREnE.abTOy(targets[1][0],targets[1][1]));
  EEPROMWritelong(recIndex + EEPROM_DIST, distT);
//Slow down motors
  setMotorSpeed(0);
/*
 Display:

 XXXXing TTTTTT s
 ####/$$$$  Time TO Next
 */
  
//Start showing useful stuff on LCD
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(F("Shoting        s"));
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print(F("1   / "));
  lcd.print(numberShots);
  
//Get timer set up
  unsigned long _startTime = millis();
  unsigned int _countDown = 0;
  unsigned long _userPhotoInterval = (_totalTime - _shutterTime) / _numberShots;
  unsigned long _userPhotoTimer = millis();
  
//Take the first photo
  lcd.setCursor(0,0);
  lcd.print(F("Shoting"));
  SHUTTER_OPEN;
  delay(_shutterTime);
  SHUTTER_CLOSE;
//Get the current position (FOR ExtensioN/Eggtimer)
  if (!_type) {
    calcCurrentxy();
    xT = x;
    yT = y;
    thetaT = theta; 
  } 

//Start the cycle - MOVE, STABILIZE/DWELL, SHOOT
  while (_numberShots){
    _numberShots--;
    thetaT += thetaDelta;
    xT += xDelta;
    yT += yDelta;
    
    lcd.setCursor(0,1);
    lcd.print(numberShots - _numberShots);
    lcd.setCursor(0,0);
    lcd.print(F(" Mov"));
    if (_type) {
      moveToObjThetaDist();
      moveToxytheta();
    }
    else moveToxytheta();
    while (m.runBresenham()){
      analogReadAll();
      if (BUTTON_TL) motorStop();
    }

//STABILIZE
    lcd.setCursor(0,0);
    lcd.print(F("Wait"));
    if (totalTime)_userPhotoTimer += _userPhotoInterval;
    else {
      _userPhotoTimer = 0;
      lcd.setCursor(8, 0);
      lcd.print(F("RAPID"));
    }
    unsigned long _timer = max(_userPhotoTimer, millis() + calcStabilizeTime());
    while (millis() < _timer){
      unsigned long _countDownNow = _totalTime / 1000 - (millis() - _startTime) / 1000;
      if (_countDownNow != _countDown){
        _countDown = _countDownNow;
        if (totalTime) {
          lcd.setCursor(8, 0);
          lcd.print(_countDown);
          lcd.print(F(" "));
        }
        lcd.setCursor(11,1);
        lcd.print((_timer - millis()) / 1000);
        lcd.print(F(" "));
      }
      analogReadAll();
      if (BUTTON_TL) {_timer = millis() - _shutterTime; _numberShots = 0;}
    }

//OPEN SHUTTER
    lcd.setCursor(0,0);
    lcd.print(F("Shot"));
    SHUTTER_OPEN;
    
//WAIT WHILE SHUTTER IS OPEN
    while (millis() < _timer + _shutterTime){
      unsigned long _countDownNow = _totalTime / 1000 - (millis() - _startTime) / 1000;
      if (_countDownNow != _countDown){
        _countDown = _countDownNow;
        if (totalTime) {
          lcd.setCursor(8, 0);
          lcd.print(_countDown);
          lcd.print(F(" "));
        }
        lcd.setCursor(11,1);
        lcd.print((_timer + _shutterTime - millis()) / 1000);
        lcd.print(F(" "));
      }
      analogReadAll();
      if (BUTTON_TL) {_timer = millis() - _shutterTime; numberShots = 0;}
    }
    SHUTTER_CLOSE;
    delay(INTERVALOMETER_POST_PHOTO_DELAY);
  }
  setMotorSpeed(1);
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
