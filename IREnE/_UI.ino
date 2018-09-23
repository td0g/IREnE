void runUI(){ //###########################################################################################################################################################################
  static byte _menuPosition = 1;
  if (_menuPosition != menuPosition){
      lcd.clear();
      switch (menuPosition){
        case 0: case 1:
        lcd.setCursor(1, 0);
        lcd.print(F("Move"));
        lcd.setCursor(1,1);
        lcd.print(F("Set Target "));
        lcd.print(targetCount);
        lcd.print("/");
        lcd.print(MAX_TARGETS);
        break;
        case 2: case 3:
        lcd.setCursor(1, 0);
        lcd.print(F("Set C Zero"));
        lcd.setCursor(1,1);
        lcd.print(F("Run Time "));
        lcd.print(totalTime);
        break;
        case 4: case 5:
        lcd.setCursor(1, 0);
        lcd.print(F("# Shots "));
        lcd.print(numberShots);
        lcd.setCursor(1,1);
        lcd.print(F("Exp Time "));
        lcd.print(shutterTime);
        break;
        case 6: case 7:
        lcd.setCursor(1, 0);
        lcd.print(F("Run ET/Linear"));
        lcd.setCursor(1,1);
        lcd.print(F("Run Inv Rad"));
        break;
        case 8:
        lcd.setCursor(1, 0);
        lcd.print(F("Settings"));
        break;
        case 16:case 17:
        lcd.setCursor(1, 0);
        lcd.print(F("A Cal "));
        lcd.print(A);
        lcd.setCursor(1,1);
        lcd.print(F("B Cal "));
        lcd.print(B);
        break;
        case 18: case 19:
        lcd.setCursor(1, 0);
        lcd.print("C Cal ");
        lcd.print(C);
        lcd.setCursor(1, 1);
        lcd.print("Manual A Cal");
        break;
        case 20: case 21:
        lcd.setCursor(1,0);
        lcd.print("Bmin ");
        lcd.print(bMin / B);
        lcd.setCursor(1, 1);
        lcd.print("Bmax ");
        lcd.print(bMax / B);
        break;
       case 22: case 23:
        lcd.setCursor(1, 0);
        lcd.print("Motor ");
        if (cFlip) lcd.print("Under");
        else lcd.print("Over ");
        lcd.setCursor(1,1);
        lcd.print("Test Cam");
        break;
        case 24: case 25:
        lcd.setCursor(1, 0);
        lcd.print("Reset Defaults");
        lcd.setCursor(1,1);
        lcd.print("Back");
        break;
        case 128:
        lcd.setCursor(0, 0);
        lcd.print("<Back      C>");
        lcd.setCursor(0, 1);
        lcd.print("<A> vB ^");
        break;
        case 129:
        lcd.setCursor(0, 0);
        lcd.print("<Back      A>");
        lcd.setCursor(0, 1);
        lcd.print("<C> vB ^");
        break;
        case 140:
        lcd.setCursor(0, 0);
        lcd.print("<Cancel     Go!>");
        lcd.setCursor(0, 1);
        lcd.print("< Set End Pos >");
        break;
        case 150:
        lcd.setCursor(0, 0);
        lcd.print("Pick 0deg/Next >");
        lcd.setCursor(0, 1);
        lcd.print("< Move A >");
        break;
        case 151:
        lcd.setCursor(0, 0);
        lcd.print("Rot 180deg/Next>");
        lcd.setCursor(0, 1);
        lcd.print("< Move A >");
        break;
    }
    if (menuPosition < 32){
      if (menuPosition % 2)lcd.setCursor(0, 0);
      else lcd.setCursor(0, 1);
      lcd.print(" ");
      if (menuPosition % 2)lcd.setCursor(0, 1);
      else lcd.setCursor(0, 0);
      lcd.print(">");
    }
  }
  _menuPosition = menuPosition;
}


void doButton(){  //#######################################################################################################################################################################
  static byte _buttonPressed;
  static long aStart;
  static byte _distCurrent;
  if (menuPosition == 140 && !runMotor()){
    if (BUTTON_L){
      thetaT += INVERTED_RADIAL_SETUP_MOVE_RADS;
      moveToObjThetaDist();
      if (b.targetPosition() < bMin) {
        motorStop();
        thetaT -= INVERTED_RADIAL_SETUP_MOVE_RADS;
      }
    }
    else if (BUTTON_R){
      thetaT -= INVERTED_RADIAL_SETUP_MOVE_RADS;
      moveToObjThetaDist();
      if (b.targetPosition() < bMin) {
        motorStop();
        thetaT += INVERTED_RADIAL_SETUP_MOVE_RADS;
      }
    }
  }
  else if (menuPosition == 128 || menuPosition == 129){
    if (runMotor()) _distCurrent = 0;
    else if (!_distCurrent){
      _distCurrent = 1;
      calcCurrDistToObject();
      Serial.println(distO);
      Serial.println(distOMax);
        lcd.setCursor(6, 1);
      if (distO > distOMax) lcd.print(F("*"));
      else lcd.print(F(" "));
    }
  }
  if (_buttonPressed != buttonPressed){
    if (menuPosition < 9){
        switch (buttonPressed & 0b00000011){
          case 0b00000010:
            if (menuPosition) menuPosition--;
            else menuPosition = 8;
          break;
          case 0b00000001:
            menuPosition++;
            menuPosition %= 9;
          break;
        }
      }
    else if (menuPosition < 26){
        switch (buttonPressed & 0b00000011){
          case 0b00000010:
            if (menuPosition > 16) menuPosition--;
            else menuPosition = 25;
          break;
          case 0b00000001:
            if (menuPosition < 25) menuPosition++;
            else menuPosition = 16;
          break;
        }
      }
      switch (menuPosition){
      case 0:
        if (BUTTON_R) {menuPosition = 128; aEnable; cEnable;}
        else if (BUTTON_TR && targetCount) cRotate(IREnE.xythetaTOc(IREnE.abTOx(a.currentPosition(), b.currentPosition()), IREnE.abTOy(a.currentPosition(), b.currentPosition()), IREnE.acTOtheta(targets[targetCount-1][0],targets[targetCount-1][2])));
      break;
      case 1:
          if (BUTTON_R){
            if (targetCount < MAX_TARGETS){
            menuPosition = 0;
              targets[targetCount][0] = a.currentPosition();
              targets[targetCount][1] = b.currentPosition();
              targets[targetCount][2] = c.currentPosition();
              targetCount++;
              lcd.setCursor(13,1);
              lcd.print(targetCount);
              calcPointFromTargetLines();
            }
          }
        else if (BUTTON_L){
          lcd.setCursor(12,1);
          targetCount = 0;
          lcd.print(targetCount);
          for (byte i = 0; i < MAX_TARGETS; i++){
            targets[i][0] = 0;
            targets[i][1] = 0;
            targets[i][2] = 0;
          }
        }
      break;
      case 2:
        if (BUTTON_R){
          a.setCurrentPosition(0);
          c.setCurrentPosition(0);
          menuPosition = 0;
        }
      break;
      case 3:
        if (BUTTON_R){
          if (!totalTime) totalTime = minTotalTime();
          else totalTime += 10;
        }
        else if (BUTTON_L){
          if (totalTime = minTotalTime()) totalTime = 0;
          totalTime -= min(totalTime, 10);
        }
        else if (BUTTON_TR){
          if (!totalTime) totalTime = minTotalTime();
          else totalTime += 600;
        }
        else if (BUTTON_TL){
          if (totalTime = minTotalTime()) totalTime = 0;
          totalTime -= min(600, totalTime);
        }
        if (totalTime) totalTime = max(totalTime, minTotalTime());
        lcd.setCursor(10,1);
        lcd.print(totalTime);
        LCD_CLEAR_LINE;
      break;
      case 4:
        if (BUTTON_R)numberShots ++;
        else if (BUTTON_L && numberShots){
          numberShots--;
          numberShots = max(2, numberShots);
        }
        else if (BUTTON_TR) numberShots += 25;
        else if (BUTTON_TL) numberShots -= min(25, numberShots);
        lcd.setCursor(9,0);
        lcd.print(numberShots);
        LCD_CLEAR_LINE;
        if (totalTime) totalTime = max(totalTime, minTotalTime());
      break;
      case 5:
        if (BUTTON_R) shutterTime++;
        else if (BUTTON_L && shutterTime) shutterTime--;
        else if (BUTTON_TR) shutterTime += 60;
        else if (BUTTON_TL) shutterTime -= min(60, shutterTime);
        lcd.setCursor(10,1);
        lcd.print(shutterTime);
        LCD_CLEAR_LINE;
        if (totalTime) totalTime = max(totalTime, minTotalTime());
      break;
      case 6:  //EnE
        if (BUTTON_R && targetCount == 2) runProgram(0);
      break;
      case 7: //IR
        if (BUTTON_R){
          if (targetCount) {
            calcCurrDistToObject();
            distT = distO;
            thetaT = thetaO;
            thetaEND = thetaT;
            menuPosition = 140;
            moveToObjThetaDist();
          }
        }
      break;
      case 8: if (BUTTON_R) menuPosition = 16;  break;
      case 16:
        if (BUTTON_L) {
          A--;
        lcd.setCursor(7, 0);
          lcd.print(A);
        }
        else if (BUTTON_R) {
         A++;
        lcd.setCursor(7, 0);
          lcd.print(A);
        }
      break;
      case 17:
        if (BUTTON_L) {
          B--;
        lcd.setCursor(7, 1);
          lcd.print(B);
        }
        else if (BUTTON_R) {
          B++;
        lcd.setCursor(7, 1);
          lcd.print(B);
        }
      break;
      case 18:
        if (BUTTON_L) {
          C--;
        lcd.setCursor(7, 0);
          lcd.print(C);
        }
        else if (BUTTON_R) {
          C++;
        lcd.setCursor(7, 0);
          lcd.print(C);
        }
      break;      
      case 19: if (BUTTON_R) menuPosition = 150; break;
      case 20:
        if (BUTTON_L) bMin = ((bMin / B) - 1) * B;
        else if (BUTTON_TL) bMin = ((bMin / B) - 25) * B;
        else if (BUTTON_R) bMin = ((bMin / B) + 1) * B;
        else if (BUTTON_R) bMin = ((bMin / B) + 25) * B;
        lcd.setCursor(6, 1);
        lcd.print(bMin / B);
      break;
      case 21:
        if (BUTTON_L) bMax = ((bMax / B) - 1) * B;
        else if (BUTTON_TL) bMax = ((bMax / B) - 25) * B;
        else if (BUTTON_R) bMax = ((bMax / B) + 1) * B;
        else if (BUTTON_TR) bMax = ((bMax / B) + 25) * B;
        lcd.setCursor(6, 1);
        lcd.print(bMax / B);
      break;
      case 22:
        if (BUTTON_R){
          lcd.setCursor(7,0);
          if (cFlip) {
            cFlip = 0;
            lcd.print("Over ");
            c.setPinsInverted(false, false, true); //first is dir
          }
          else {
            cFlip = 1;
            lcd.print("Under");
            c.setPinsInverted(true, false, true); //first is dir
          }
        }
      break;
      case 23:
        if (BUTTON_R){
          SHUTTER_OPEN;
          lcd.setCursor(0, 1);
          lcd.print(F(" "));
          Serial.println(F("SHUTTER"));
          delay(1000);
          SHUTTER_CLOSE;
          lcd.setCursor(0, 1);
          lcd.print(F(">"));
        }
      break;
      case 24:
        if (BUTTON_R){
          A = A_DEFAULT;
          B = B_DEFAULT;
          C = C_DEFAULT;
          bMin = B_MIN_DEFAULT * B;
          bMax = B_MAX_DEFAULT * B;
          stabTimeMin = MINIMUM_STABILIZE_TIME_DEFAULT;
          stabTimeMax = MAXIMUM_STABILIZE_TIME_DEFAULT;
          saveEEPROM();
          IREnE.setAC(A, C);
          menuPosition = 25;
        }
      break;
      case 25: if (BUTTON_R){ saveEEPROM();  menuPosition = 8; } break;
      case 128: case 129:
      if (_buttonPressed != buttonPressed){
        if (menuPosition == 128){
          if (BUTTON_TR) menuPosition = 129;
          if (BUTTON_L){
            a.moveTo(-1000000000);
          }
          else if (BUTTON_R){
            a.moveTo(1000000000);
        
          }
          else a.stop();
        }
        else {
          if (BUTTON_TR) menuPosition = 128;
          if (BUTTON_L) c.moveTo(-1000000000);
          else if (BUTTON_R) c.moveTo(1000000000);
          else c.stop();
        }
        if (BUTTON_D) b.moveTo(-1000000000);
        else if (BUTTON_U) b.moveTo(bMax);
        else b.stop();
        if (buttonPressed == 0b00100000) {
          Serial.print(F("a,b,c "));
          Serial.print(a.currentPosition());
          Serial.print(" ");
          Serial.print(b.currentPosition());
          Serial.print(" ");
          Serial.println(c.currentPosition());
          calcCurrentxy();
          Serial.print(F("x,y,theta "));
          Serial.print(x);
          Serial.print(" ");
          Serial.print(y);
          Serial.print(" ");
          Serial.println(theta);
          menuPosition = 0;
          motorStop();
        }
      }
      break;

      
      //Inv Rad
      case 140:
        if (!a.run() && !b.run() && !c.run()){
          if (BUTTON_TR) {
            thetaSTART = thetaT;
            Serial.println(F("  BEGINNING IR"));
            runProgram(1);
          }
          else if (BUTTON_TL) menuPosition = 7;
        }
      break;
      case 150: case 151:
        if (BUTTON_TR){
          if (menuPosition == 150) {
            aStart = a.currentPosition();
            menuPosition = 151;
          }
          else if (a.currentPosition() != aStart){
            menuPosition = 19;
            aStart -= a.currentPosition();
            aStart = abs(aStart);
            aStart /= PI;
            A = aStart;
          }
        }
        if (BUTTON_L) a.moveTo(-1000000000);
        else if (BUTTON_R)a.moveTo(1000000000);
        else a.stop();
      break;
    }
  _buttonPressed = buttonPressed;
  }
}

unsigned int minTotalTime(){
  unsigned int _totalTime = MAXIMUM_STABILIZE_TIME_DEFAULT / 1000;
  _totalTime += INTERVALOMETER_MOVE_TIME;
  _totalTime += shutterTime;
  _totalTime *= (numberShots - 1); 
  _totalTime += shutterTime;
  return _totalTime;
}
