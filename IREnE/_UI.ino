//Main Menu - Must start with even number
#define MENU_MOVE 0
#define MENU_SET_TARGET 1
#define MENU_SET_C_ZERO 2
#define MENU_INTERVALOMETER_RUNTIME 3
#define MENU_INTERVALOMETER_NUMSHOTS 4
#define MENU_INTERVALOMETER_EXPOSURE 5
#define MENU_RUN_ENE 6
#define MENU_RUN_IR 7
#define MENU_SETTINGS 8

//Settings Submenu - Must start with even number
#define MENU_SETTINGS_ACAL 10
#define MENU_SETTINGS_BCAL 11
#define MENU_SETTINGS_CCAL 12
#define MENU_SETTINGS_ACALMANUAL 13
#define MENU_SETTINGS_BMIN 14
#define MENU_SETTINGS_BMAX 15
#define MENU_SETTINGS_MOTORDIR 16
#define MENU_SETTINGS_TESTCAM 17
#define MENU_SETTINGS_RESET 18
#define MENU_SETTINGS_EXIT 19

//Misc menu positions
#define MENU_MOVEAB 20
#define MENU_MOVECB 21
#define MENU_MOVEXY_SPEED 22
#define MENU_MOVEXY_ACC 23
#define MENU_IR_MOVE 24
#define MENU_CAL_C_1 25
#define MENU_CAL_C_2 26

//Defining main and sub menus
#define MENU_FIRST 0
#define MENU_LAST MENU_SETTINGS + 1
#define MENU_SETTINGS_FIRST MENU_SETTINGS_ACAL
#define MENU_SETTINGS_LAST MENU_SETTINGS_EXIT + 1

//Macros
#define REFRESH_DISPLAY menuPosition |= 0b10000000

void runUI(){ //###########################################################################################################################################################################
  static byte _menuPosition = 1;
  if (_menuPosition != menuPosition && !runMotor()){
      menuPosition &= 0b01111111;
      lcd.clear();
      lcd.setCursor(1, 0);
      switch (menuPosition){
        case MENU_MOVE: case MENU_SET_TARGET:
        lcd.print(F("-Line<MOVE>Axis"));
        lcd.setCursor(1,1);
        lcd.print(F("Set Target "));
        lcd.print(targetCount);
        lcd.print(F("/"));
        lcd.print(2);
        break;
        case MENU_SET_C_ZERO: case MENU_INTERVALOMETER_RUNTIME:
        lcd.print(F("Set C Zero"));
        lcd.setCursor(1,1);
        lcd.print(F("Run Time "));
        lcd.print(totalTime);
        break;
        case MENU_INTERVALOMETER_NUMSHOTS: case MENU_INTERVALOMETER_EXPOSURE:
        lcd.print(F("# Shots "));
        lcd.print(numberShots);
        lcd.setCursor(1,1);
        lcd.print(F("Exp Time "));
        lcd.print(shutterTime);
        break;
        case MENU_RUN_ENE: case MENU_RUN_IR:
        lcd.print(F("Run ET/Linear"));
        lcd.setCursor(1,1);
        lcd.print(F("Run Inv Rad"));
        break;
        case MENU_SETTINGS:
        lcd.print(F("Settings"));
        break;
        case MENU_SETTINGS_ACAL: case MENU_SETTINGS_BCAL:
        lcd.print(F("A Cal "));
        lcd.print(A);
        lcd.setCursor(1,1);
        lcd.print(F("B Cal "));
        lcd.print(B);
        break;
        case MENU_SETTINGS_CCAL: case MENU_SETTINGS_ACALMANUAL:
        lcd.print(F("C Cal "));
        lcd.print(C);
        lcd.setCursor(1, 1);
        lcd.print(F("Manual A Cal"));
        break;
        case MENU_SETTINGS_BMIN: case MENU_SETTINGS_BMAX:
        lcd.print(F("Bmin "));
        lcd.print(bMinMM);
        lcd.setCursor(1, 1);
        lcd.print(F("Bmax "));
        lcd.print(bMaxMM);
        break;
       case MENU_SETTINGS_MOTORDIR: case MENU_SETTINGS_TESTCAM:
          lcd.print(F("Motor "));
          if (cFlip) lcd.print(F("Over"));
          else lcd.print(F("Under"));
          lcd.setCursor(1,1);
          lcd.print(F("Test Cam"));
        break;
        case MENU_SETTINGS_RESET: case MENU_SETTINGS_EXIT:
          lcd.print(F("Reset Defaults"));
          lcd.setCursor(1,1);
          lcd.print(F("Back"));
        break;
        case MENU_MOVEAB: case MENU_MOVECB:
          #ifdef DEBUG
            Serial.print(F("CURR POS a,b,c,x,y: "));
            Serial.print(a.currentPosition());
            Serial.print(F(", "));
            Serial.print(b.currentPosition());
            Serial.print(F(", "));
            Serial.print(c.currentPosition());
            Serial.print(F(", "));
            Serial.print(IREnE.abTOx(a.currentPosition(), b.currentPosition()));
            Serial.print(F(", "));
            Serial.println(IREnE.abTOy(a.currentPosition(), b.currentPosition()));
          #endif
          lcd.setCursor(0, 0);
          lcd.print(F("<Back"));
          lcd.setCursor(9, 0);
          lcd.print(F("A/C>"));
          lcd.setCursor(0, 1);
          lcd.print(F("<"));
          if (menuPosition == MENU_MOVEAB) lcd.print(F("A"));
          else lcd.print(F("C"));
          lcd.print(F("> vB"));
          calcCurrDistToObject();
          lcd.setCursor(6, 1);
          if (distO > distOMax) lcd.print(F("*^"));
          else lcd.print(F(" ^"));
        break;
        case MENU_MOVEXY_SPEED: case MENU_MOVEXY_ACC:
          lcd.setCursor(0, 0);
          lcd.print(F("<"));
          if (menuPosition == MENU_MOVEXY_SPEED) lcd.print(F("a"));
          else lcd.print(F("s"));
          lcd.print(F("/Exit  Change>"));
          if (menuPosition == MENU_MOVEXY_SPEED) lcd.setCursor(0, 1);
          else lcd.setCursor(10, 1);
          lcd.print(F(">"));
          lcd.setCursor(1, 1);
          lcd.print(F("s"));
          lcd.print(movieSpeed);
          lcd.setCursor(11, 1);
          lcd.print(F("a"));
          lcd.print(movieAccelTime);
          lcd.setCursor(6, 1);
          lcd.print(F("LINE"));
        break;
        case MENU_IR_MOVE: 
          lcd.setCursor(0, 0);
          lcd.print(F("s"));
          if (!movieSpeed) lcd.print("MAX");
          else lcd.print(movieSpeed);
          lcd.setCursor(5, 0);
          lcd.print(F("a"));
          lcd.print(movieAccelTime);
          lcd.print(F("  "));
          lcd.setCursor(11, 0);
          lcd.print(F("STRT>"));
          lcd.setCursor(0, 1);
          lcd.print(F("< MOVE >"));
        break;
        case MENU_CAL_C_1: case MENU_CAL_C_2:
        lcd.setCursor(0, 0);
        if (menuPosition == MENU_CAL_C_1) lcd.print(F(" Pick "));
        else lcd.print(F("Rot 18"));
        lcd.print(F("0deg/Next >"));
        lcd.setCursor(0, 1);
        lcd.print(F("< Move A >"));
        break;
    }
    if (menuPosition < MENU_SETTINGS_LAST){
      byte _t = menuPosition & 1;
      lcd.setCursor(0, 1 - _t);
      lcd.print(F(" "));
      lcd.setCursor(0, _t);
      lcd.print(F(">"));
    }
  _menuPosition = menuPosition;
  }
}


void doButton(){  //#######################################################################################################################################################################
  static byte _buttonPressed;

//Move cursor
  if (_buttonPressed != buttonPressed){
    if (menuPosition < MENU_SETTINGS_LAST){
        switch (buttonPressed & 0b00000011){
          case 0b00000010:
            if (!menuPosition) menuPosition = MENU_LAST;
            else if (menuPosition == MENU_SETTINGS_FIRST) menuPosition = MENU_SETTINGS_LAST;
            menuPosition--;
          break;
          case 0b00000001:
            menuPosition++;
            if (menuPosition == MENU_LAST) menuPosition = MENU_FIRST;
            else if (menuPosition == MENU_SETTINGS_LAST) menuPosition = MENU_SETTINGS_FIRST;
          break;
        }
      }
      
//Other Stuff
      switch (menuPosition & 0b01111111){
      case MENU_MOVE:
        if (BUTTON_R) {menuPosition = MENU_MOVEAB; motorEnable;}
        else if (BUTTON_TR && targetCount) c.moveTo(cRot(IREnE.xythetaTOc(IREnE.abTOx(a.currentPosition(), b.currentPosition()), IREnE.abTOy(a.currentPosition(), b.currentPosition()), IREnE.acTOtheta(targets[targetCount-1][0],targets[targetCount-1][2]))));
        else if (BUTTON_L) {
          menuPosition = MENU_MOVEXY_SPEED;
          movieAccelTime = 0.25;
          movieSpeed = max(movieSpeed, 1);
          motorEnable;
        }
      break;
      case MENU_SET_TARGET:
          if (BUTTON_R){
            if (targetCount < 2){
              #ifdef DEBUG
                Serial.println(F("TARG SET"));
              #endif
              menuPosition = MENU_MOVE;
              targets[targetCount][0] = a.currentPosition();
              targets[targetCount][1] = b.currentPosition();
              targets[targetCount][2] = c.currentPosition();
              targetCount++;
              REFRESH_DISPLAY;
              calcPointFromTargetLines();
            }
          }
        else if (BUTTON_L){
          targetCount = 0;
          REFRESH_DISPLAY;
          for (byte i = 0; i < 3; i++){
            targets[0][i] = 0;
            targets[1][i] = 0;
          }
        }
      break;
      case MENU_SET_C_ZERO: 
        if (BUTTON_R){    
          for (byte i = 0; i < targetCount; i++)targets[i][2] -= c.currentPosition();
          c.setCurrentPosition(0);
          menuPosition = MENU_MOVE;
        } 
      break;
      case MENU_INTERVALOMETER_RUNTIME:
        unsigned int _minTotalTime;
        _minTotalTime = minTotalTime();
        if (totalTime == minTotalTime()) _minTotalTime = 0;
        if (alterValue(&totalTime, 250)){
          if (totalTime < minTotalTime() && totalTime) totalTime = _minTotalTime;
        }
        REFRESH_DISPLAY;
      break;
      case MENU_INTERVALOMETER_NUMSHOTS:
        if (alterValue(&numberShots, 25)){
          numberShots = max(2, numberShots);
          REFRESH_DISPLAY;
          if (totalTime) totalTime = max(totalTime, minTotalTime());
        }
      break;
      case MENU_INTERVALOMETER_EXPOSURE:
        if (alterValue(&shutterTime, 60)){
          if (!shutterTime) shutterTime = 1;
          REFRESH_DISPLAY;
          if (totalTime) totalTime = max(totalTime, minTotalTime());
        }
      break;
      case MENU_RUN_ENE: if (BUTTON_R && targetCount == 2) runProgram(0); break; //EnE
      case MENU_RUN_IR: //IR
        if (BUTTON_R){
          calcCurrDistToObject(); //Do this so we get distO
          if (targetCount && distO > IR_MIN_OBJ_DIST) {
            movieSpeed = 0;
            movieAccelTime = 0.25;
            distT = distO;
            thetaT = thetaO;
            thetaEND = thetaO;
            menuPosition = MENU_IR_MOVE;
            moveToObjThetaDist();
            moveToxytheta();
            while (buttonPressed) analogReadAll();
          }
        }
      break;
      case MENU_SETTINGS: if (BUTTON_R) menuPosition = 16;  break;
      case MENU_SETTINGS_ACAL: 
        if (alterValue(&A, 25)) {
          A = max(A, 1); 
          REFRESH_DISPLAY;
        }
      break;
      case MENU_SETTINGS_BCAL: 
        if (alterValue(&B, 25)){
          B = max(B, 1); 
          REFRESH_DISPLAY;
        }
      break;
      case MENU_SETTINGS_CCAL: 
        if (alterValue(&C, 25)){
          C = max(C, 1); 
          REFRESH_DISPLAY;      
        }
      break; 
      case MENU_SETTINGS_ACALMANUAL: if (BUTTON_R) menuPosition = MENU_CAL_C_1; break;
      case MENU_SETTINGS_BMIN: if (alterValue(&bMinMM, 25)) REFRESH_DISPLAY; break;
      case MENU_SETTINGS_BMAX: if (alterValue(&bMaxMM, 25)) REFRESH_DISPLAY; break;
      case MENU_SETTINGS_MOTORDIR:
        if (BUTTON_R){
          cFlip = 1 - cFlip;
          c.setPinsInverted(cFlip, false, true);
          REFRESH_DISPLAY;
        }
      break;
      case MENU_SETTINGS_TESTCAM:
        if (BUTTON_R){
          SHUTTER_OPEN;
          lcd.clear();
          delay(1000);
          SHUTTER_CLOSE;
          REFRESH_DISPLAY;
        }
      break;
      case MENU_SETTINGS_RESET:
        if (BUTTON_R){
          resetEEPROMsettings();
          IREnE.setAC(A, C);
          menuPosition = MENU_SETTINGS_EXIT;
        }
      break;
      case MENU_SETTINGS_EXIT: if (BUTTON_R){saveEEPROM(); bMin = bMinMM * B; bMax = (long) bMaxMM * B; menuPosition = 8;} break;
      case MENU_MOVEAB: case MENU_MOVECB:
        REFRESH_DISPLAY;
        if ((menuPosition & 0b01111111) == MENU_MOVEAB){
          if (BUTTON_L) a.moveTo(-1000000000);
          else if (BUTTON_R) a.moveTo(1000000000);
          else {
            a.stop();
            if (BUTTON_TR) menuPosition = MENU_MOVECB;
          }
        }
        else {
          if (BUTTON_L) c.moveTo(-1000000000);
          else if (BUTTON_R) c.moveTo(1000000000);
          else {
            c.stop();
             if (BUTTON_TR) menuPosition = MENU_MOVEAB;
          }
        }
        if (BUTTON_D) b.moveTo(-1000000000);
        else if (BUTTON_U) b.moveTo(bMax);
        else b.stop();
        if (buttonPressed == 0b00100000) {
          calcCurrentxy();
          menuPosition = MENU_MOVE;
          motorStop();
        }
      break;
      case MENU_MOVEXY_SPEED: case MENU_MOVEXY_ACC:
        if (BUTTON_TL) {
          menuPosition = MENU_MOVEXY_SPEED + MENU_MOVEXY_ACC - menuPosition;
          unsigned long _t = millis() + 1000;
          while (buttonPressed && millis() < _t) analogReadAll();
          if (millis() >= _t) menuPosition = MENU_MOVE;
        }
        else if (BUTTON_TR){
          if (menuPosition == MENU_MOVEXY_ACC){
            if (movieAccelTime >= 8) movieAccelTime = 0.25;
            else movieAccelTime *= 2;
          }
          else if (movieSpeed >= 128? movieSpeed = 1 : movieSpeed *= 2);
          REFRESH_DISPLAY;
        }
        else {
          calcCurrDistToObject();
          thetaT = theta;
          float xInc;
          xInc = B * cos(theta);
          float yInc;
          yInc = B * sin(theta);
          if ((BUTTON_L) || (BUTTON_D)){
            xInc *= -1;
            yInc *= -1;
          }
          if ((BUTTON_L) || (BUTTON_R)) movie(xInc, yInc, 0, 0);
          else if ((BUTTON_U) || (BUTTON_D)) movie(yInc, xInc, 0, 0);
        }

      break;
      //Inv Rad
      case MENU_IR_MOVE:
        if ((BUTTON_L) || (BUTTON_R)){
          //d = theta * r, theta = d / r
          calcCurrDistToObject();
          float thetaD = B;   //1 mm
          thetaD /= distO;     //theta = 1mm / distO
          if (BUTTON_R) thetaD *= -1;
          movie(0, 0, thetaD, 0);
        }
        else if (BUTTON_TR) {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print(F("HOLD"));
          unsigned long _t = millis();
          while (BUTTON_TR && millis() < _t + 1000) analogReadAll();
          if (millis() > _t + 999) runProgram(1);
          REFRESH_DISPLAY;
        }
        
        else if (BUTTON_TL) menuPosition = MENU_RUN_IR;
        else if (BUTTON_U) {
          if (movieSpeed >= 128) movieSpeed = 0;
          else if (!movieSpeed) movieSpeed = 1;
          else movieSpeed *= 2;
          REFRESH_DISPLAY;
        }
        else if (BUTTON_D){
          if (movieAccelTime >= 8) movieAccelTime = 0.25;
          else movieAccelTime *= 2;
          REFRESH_DISPLAY;
        }
      break;
      case MENU_CAL_C_1: case MENU_CAL_C_2:
        if (BUTTON_TR){
          if (menuPosition == MENU_CAL_C_1) {
            a.setCurrentPosition(0);
            menuPosition = MENU_CAL_C_2;
          }
          else if (a.currentPosition()){
            menuPosition = MENU_SETTINGS_EXIT;
            long _A;
            _A = a.currentPosition();
            _A = abs(_A);
            _A /= PI;
            A = _A;
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
  unsigned int _totalTime;
  _totalTime = MAXIMUM_STABILIZE_TIME_DEFAULT;
  _totalTime /= 1000;
  _totalTime += INTERVALOMETER_MOVE_TIME + shutterTime;
  _totalTime *= (numberShots - 1); 
  _totalTime += shutterTime;
  return _totalTime;
}

boolean alterValue(unsigned int *_n, byte _bigChange){
  if (BUTTON_L) {
    if (*_n) *_n -= 1;
  }
  else if (BUTTON_TL) *_n -= min(_bigChange, *_n);
  else if (BUTTON_R) *_n += 1;
  else if (BUTTON_TR) *_n += _bigChange;
  else return false;
  return true;
}


void resetEEPROMsettings(){
  A = A_DEFAULT;
  B = B_DEFAULT;
  C = C_DEFAULT;
  bMinMM = B_MIN_DEFAULT;
  bMaxMM = B_MAX_DEFAULT;
  stabTimeMin = MINIMUM_STABILIZE_TIME_DEFAULT;
  stabTimeMax = MAXIMUM_STABILIZE_TIME_DEFAULT;
  saveEEPROM();
}

float minRotateTime(){
  float _m1 = sqrt(IREnE.objectX() * IREnE.objectX() + IREnE.objectY() * IREnE.objectY());
  _m1 -= distO;
  _m1 *= motorTravelSpeed;
  _m1 /= A;
  _m1 = min(_m1, motorTravelSpeed);
  _m1 /= distO; //rad/s
  float _m2 = motorTravelSpeed;
  _m2 /= C;
  //Serial.print("crotmax:");
  //Serial.println(_m2);
  _m1 = min(_m1, _m2);
  _m1 /= PI;
  _m1 /= 2; //rot / s
  _m1 = 1 / _m1; //s / rot
  return _m1;
}

