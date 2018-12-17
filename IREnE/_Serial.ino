/*
boolean getCommand() {//   SERIAL COMMUNICATIONS   ############################################################################################################################################################
  if (Serial.available()){
    char _c = Serial.read();
    if (_c == 'h')printEEPROMhistory();
    else if (_c == 'r') resetEEPROMsettings();
  }
}
*/




#ifdef CNC_PANCAKE

/*
2018-12-01
  NEW - CNC Pancake Printing Mode
  VTG

  Method of Function:
    IREnE will function same as before when using the hand control
    When a PC is connected, gcode can be sent - THIS WILL NOT AFFECT OUTSIDE FUNCTIONS
    EXCEPT:
      secondary rotator (c motor) becomes disabled in movie mode when M3/M5/M106/M107 commands are recieved

  Setup:
    Step 1 - Move the head to X=0, Y=0 and send the following command: G92 X0 Y0 ;Set Origin
    Step 2 - Move the head to X=MAX, Y=0 and send the following command: G92 Y0 ; Set Orientation


  Valid gCodes:
    G0: Rapid move
    G1: Move
    G90: Relative positioning
    G91: Absolute positioning
    G92: Set current position
    M3: Tool off
    M5: Tool on
    M114: Report current position
    M221: Set extrusion factor (step/step)
  
 */
 
  #define MAX_BUF 64 // What is the longest message Arduino can store?
  #define CHAR_BUF 128
  #define FEEDRATE_DEFAULT 600
  #define STEPS_PER_SECTION 500
  #define RETRACT_STEPS 4000
  
  char serialBuffer[MAX_BUF];
  int sofar;
  byte left;
  byte doingSomething;
  float extrusionFactor = 1;
  #define FINISHED_ACTION doingSomething = doingSomething & 0b11111101;
  
  //Pancakes
  byte posABS = 0;
  float offset[2] = {0, 0};
  
  char getSerial(byte returnChar){      //Saves incoming Serial Data to a big buffer
    static byte writePos;
    static byte readPos = CHAR_BUF - 1;
    static char charBuffer[CHAR_BUF];
  
    if (returnChar == 2){
      for (byte i = 0; i < CHAR_BUF; i++){
        charBuffer[i] = 0;
      }
      writePos = 0;
      readPos = CHAR_BUF - 1;
      while (Serial.available()) Serial.read();
      delay(100);
      while (Serial.available()) Serial.read();
      return 0;
    }
    
    if (Serial.available() && writePos != readPos){
      charBuffer[writePos] = Serial.read();
      //Serial.print(charBuffer[writePos]);
      writePos++;
      writePos %= CHAR_BUF;
    }
    if (returnChar && charBuffer[((readPos + 1) % CHAR_BUF)] != 0){
      readPos++;
      readPos %= CHAR_BUF;
      char temp = charBuffer[readPos];
      charBuffer[readPos] = 0;
      return temp;
    }
    return 0;
  }
  
  void getCommand(){
    static byte crc;
    static byte crcEnd = 0;
    static byte crcGood = 0;
    static unsigned long lineNumber = 0;
    switch (doingSomething){
      case 0:
        char c;
        c = getSerial(1);
        //Serial.print(c);
        if(c == '\n') {
          lineNumber++;
          doingSomething = 0b00000011;
        }
        else if(c != 0 && sofar < MAX_BUF) {
          serialBuffer[sofar++]=c;
          if (c == 42) crcEnd = 1;
          if (!crcEnd) crc ^= c;
        }
      break;
      case 1: case 3:
          int printedCRC;
          printedCRC = parseNumber(42,-1);
          if (crc == printedCRC|| printedCRC == -1){
            processCommand();
            doingSomething = doingSomething & 0b11111110;
            crc = 0;
            crcEnd = 0;
            for (byte i= 0; i < sofar; i++) Serial.print(serialBuffer[i]);
            Serial.println();
            Serial.println("ok");
          }
          else {
            Serial.print("N");
            Serial.print(lineNumber);
            Serial.print(" Expected Checksum ");
            Serial.println(crc);
            lineNumber--;
            getSerial(2);
          }
          sofar = 0;  //clear input buffer
          left = 0;
      break;
      default: break;
    }
  }
  
  void processCommand() {
    static float feedLast[2] = {FEEDRATE_DEFAULT,FEEDRATE_DEFAULT};
    int cmd=parseNumber('G',-1);
    switch(cmd) {
      case 0: case 1: // move in a line
        feedLast[cmd] = parseNumber('F',feedLast[cmd]);
        if (posABS) moveToPosABS(parseNumber('X', getPos(0)), parseNumber('Y', getPos(1)), feedLast[cmd]);
        else moveToPos(parseNumber('X', 0), parseNumber('Y', 0), feedLast[cmd]);
        break;
      case 4: delay(parseNumber('P',0)); FINISHED_ACTION; break; // wait a while
      case 28: break; //HOME
      case 90: posABS = 0; FINISHED_ACTION; break;
      case 91: posABS = 1; FINISHED_ACTION; break;
      case 92: setPos(parseNumber('X',0), parseNumber('Y',0)); FINISHED_ACTION; break; //setPosZero();
      default:  FINISHED_ACTION; break;
    }
    if (cmd == -1) FINISHED_ACTION;
  
    cmd=parseNumber('M',-1);
    switch(cmd) {
      case 3: case 107: if (pancakePrinting == 2) c.move(RETRACT_STEPS); while (runMotor()){}; pancakePrinting = 1;break; //Tool off / Pump off
      case 5: case 106: if (pancakePrinting == 1) c.move(-1 * RETRACT_STEPS); while (runMotor()){}; pancakePrinting = 2;break; //Tool on / Pump on
      case 114: Serial.print(F("X")); Serial.print(getPos(0)); Serial.print(F(" Y")); Serial.println(getPos(1));break;
      case 221: extrusionFactor = parseNumber('S', extrusionFactor); extrusionFactor = fabs(extrusionFactor); break;
      default: break;
    }
  }
  
  
  
  float parseNumber(char code,float val) {
    char *ptr=serialBuffer + left;  // start at the beginning of buffer
    while((long)ptr > 1 && (*ptr) && (long)ptr < (long)serialBuffer+sofar) {  // walk to the end
      if(*ptr==code) {  // if you find code on your walk,
        return atof(ptr+1);  // convert the digits that follow into a float and return it
      }
      ptr++;  // take a step from here to the letter after the next space
    }
    return val;  // end reached, nothing found, return default val.
  }
  
  
  void moveToPosABS(float _x, float _y, float _f){  //_x and _y in mm, _f in mm/min
    _x = _x + IREnE.abTOx(a.currentPosition(), b.currentPosition()) / B - offset[0];
    _y = _y + IREnE.abTOy(a.currentPosition(), b.currentPosition()) / B - offset[1];
    moveToPos(_x, _y, _f);
  }
  
  void moveToPos(float _x, float _y, float _f){ //_x and _y in mm, _f in mm/min
    //movie(_x, _y, 0, _sectionsInt);
    _x *= B;
    _y *= B;
    float dist = _x * _x + _y * _y;
    dist = sqrt(dist);
    float sections = dist / STEPS_PER_SECTION;
    unsigned int _sections = sections;
    _sections = max(_sections, 1);
    _x /= _sections;
    _y /= _sections;
    float extrude = dist * extrusionFactor / _sections;
    _f /= 60;
    float t = dist / (B * _f * _sections);
    if (pancakePrinting == 1) extrude = 0;
    //else c.setMaxSpeed(extrude / t);
    for (unsigned int i = 0; i < _sections; i++){
      float _X = IREnE.abTOx(a.currentPosition(), b.currentPosition()) - _x;
      float _Y = IREnE.abTOy(a.currentPosition(), b.currentPosition()) - _y;
      float _aDist = a.currentPosition();
      _aDist -= IREnE.xyTOa(_X, _Y);
      _aDist = fabs(_aDist);
      float _bDist = b.currentPosition();
      _bDist -= IREnE.xyTOb(_X, _Y);
      _bDist = fabs(_bDist);
        if (_aDist > _bDist) {
          a.setMaxSpeed(min(motorMaxSpeed[0], _aDist / t));
        }
        else {
          b.setMaxSpeed(min(motorMaxSpeed[1], _bDist / t));
        }
      long targ[3] = {IREnE.xyTOa(_X, _Y), IREnE.xyTOb(_X, _Y), c.currentPosition() - extrude};
      m.setupBresenham(targ);
      while (m.runBresenhamSpeed()){};
      setMotorSpeed(1);
    }
    FINISHED_ACTION;
  }
  
  float getPos(byte _a){
    if (!_a) return offset[0] - IREnE.abTOx(a.currentPosition(), b.currentPosition()) / B;
    return offset[1] - IREnE.abTOy(a.currentPosition(), b.currentPosition()) / B;
  }
  
  
  void setPos(float X, float Y){
    if (offset[1]){
      if (!Y && X != 0){
        float dx = getPos(0);
        float dy = getPos(1);
        a.setCurrentPosition(a.currentPosition() + A * atan(dy / dx));
        offset[1] = IREnE.abTOy(a.currentPosition(), b.currentPosition())/ B;
        float d;
        d = dx* dx + dy * dy;
        d = sqrt(d);
        offset[0] = d + IREnE.abTOx(a.currentPosition(), b.currentPosition()) / B;
        Serial.println(IREnE.abTOx(a.currentPosition(), b.currentPosition()));
        Serial.println(d);
        Serial.println(offset[0]);
      }
    }
    else {
      if (!X && !Y){
        a.setCurrentPosition(0);
        offset[0] = 0;
        offset[1] = b.currentPosition() / B;
      }
    }
  }

  
  

#endif
