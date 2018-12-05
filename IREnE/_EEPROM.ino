void saveEEPROM(){
  EEPROMWriteint(0, A);
  EEPROMWriteint(2, B);
  EEPROMWriteint(4, C);
  EEPROMWriteint(6, bMinMM);
  EEPROMWriteint(8, bMaxMM);
  EEPROMWriteint(11, stabTimeMin);
  EEPROMWriteint(13, stabTimeMax);
  EEPROM.update(10, cFlip);
}

unsigned int EEPROMReadint(unsigned int _i){
  unsigned int _t;
  _t = EEPROM.read(_i);
  _t *= 256;
  _t += EEPROM.read(_i + 1);
  return (_t);
}

void EEPROMWriteint(unsigned int _i, unsigned int _d){
  byte t;
  t = _d;
  EEPROM.write(_i+1, t);
  t = _d >> 8;
  EEPROM.write(_i, t);
}

void EEPROMWritelong(unsigned int address, long value) {
  for (byte i = 0; i < 4; i++){
    byte _b = ((value >> (8 * i)) & 0xFF);
    EEPROM.write(address + i, _b);
  }
}

long EEPROMReadlong(int address){
  unsigned long _d;
  address += 3;
  for (byte i = 0; i < 4; i++){
    _d = _d << 8;
    _d += EEPROM.read(address - i);
  }
return _d;
}



unsigned int findIndexEEPROM(){
  byte a;
  a = EEPROM.read(PROGRAM_HISTORY_EEPROM_START);
  for (uint16_t i = PROGRAM_HISTORY_EEPROM_START + PROGRAM_HISTORY_EEPROM_SIZE; i < EEPROM.length(); i += PROGRAM_HISTORY_EEPROM_SIZE){
    a++;
    if (EEPROM.read(i) != a) {
      EEPROM.write(i, a);
      return i;
    }
    else a = EEPROM.read(i);
  }
  EEPROM.write(PROGRAM_HISTORY_EEPROM_START, a);
  return PROGRAM_HISTORY_EEPROM_START;
}


void printEEPROMhistory(){
//Find earliest record
  Serial.println("FINDING ORDER!");
  unsigned int startLoc = PROGRAM_HISTORY_EEPROM_START;
  byte v0 = EEPROM.read(startLoc);
  for (unsigned int i = PROGRAM_HISTORY_EEPROM_START + PROGRAM_HISTORY_EEPROM_SIZE; i < EEPROM.length(); i += PROGRAM_HISTORY_EEPROM_SIZE){
    if (EEPROM.read(i) != v0 + 1) {
      startLoc = i;
      break;
    }
    v0 = EEPROM.read(i);
  }
//Go through records and print
  for (unsigned int i = startLoc; i < EEPROM.length(); i += PROGRAM_HISTORY_EEPROM_SIZE) printEEPROMlocation(i);
  for (unsigned int i = PROGRAM_HISTORY_EEPROM_START; i < startLoc; i += PROGRAM_HISTORY_EEPROM_SIZE) printEEPROMlocation(i);
}

void printEEPROMlocation(unsigned int i){
      unsigned int j = i;
    for (byte k = 0; k < 100; k++){
      Serial.print(F("#,"));
    }
    Serial.println();
    Serial.println(F("Index, Type, Shots"));
    Serial.print(EEPROM.read(j));
    Serial.print(F(","));
    Serial.print(EEPROM.read(j + EEPROM_TYPE));
    Serial.print(F(","));
    Serial.println(EEPROMReadint(i + EEPROM_COUNT));
    Serial.println(F("A,B,C"));
    Serial.print(EEPROMReadint(i + EEPROM_ABC));
    Serial.print(F(", "));
    Serial.print(EEPROMReadint(i + EEPROM_ABC + 2));
    Serial.print(F(", "));
    Serial.println(EEPROMReadint(i + EEPROM_ABC + 4));

    j += EEPROM_TARGETS;
    
    float _t;
    for (byte k = 0; k < 2; k++){
      Serial.print(F("Target "));
      Serial.print(k);
      Serial.print(F(","));
      targets[k][0] = EEPROMReadlong(j);
      j += 4;
      Serial.print(targets[k][0]);
      Serial.print(F(","));
      targets[k][1] = EEPROMReadlong(j);
      j += 4;
      Serial.print(targets[k][1]);
      Serial.print(F(","));
      targets[k][2] = EEPROMReadlong(j);
      j += 4;
      Serial.println(targets[k][2]);
    }
    j = i + EEPROM_THETA;
    Serial.println(F("theta START,ThetaEND,ThetaDELTA"));
      float thetaT;
    EEPROM.get(j, thetaT);
    Serial.print(thetaT,6);
    Serial.print(F(", "));
    j += 4;
    EEPROM.get(j, _t);
    Serial.print(_t, 6);
    Serial.print(F(", "));
    j += 4;
    float thetaDelta = _t - thetaT;
    thetaDelta /= (EEPROMReadint(i + EEPROM_COUNT) - 1);
    Serial.println(thetaDelta, 6);

    Serial.print(F("DistT,"));
    distT = EEPROMReadlong(i + EEPROM_DIST);
    Serial.println(distT);

    if (!EEPROM.read(i + EEPROM_TYPE)){
      j = i + EEPROM_X;
      long xStart = EEPROMReadlong(j);
      j += 4;
      long xyEnd = EEPROMReadlong(j);
      float xDelta= xyEnd - xStart;
      xDelta /= (EEPROMReadint(i + EEPROM_COUNT) - 1);
      Serial.println(F("x START,END,D"));
      Serial.print(xStart);
      Serial.print(F(", "));
      Serial.print(xyEnd);
      Serial.print(F(", "));
      Serial.println(xDelta, 6);
      j = i + EEPROM_Y;
      long yStart = EEPROMReadlong(j);
      j += 4;
      xyEnd = EEPROMReadlong(j);
      float yDelta = xyEnd - yStart;
      yDelta /= (EEPROMReadint(i + EEPROM_COUNT) - 1);
      Serial.println(F("y START,END,DELTA"));
      Serial.print(yStart);
      Serial.print(F(", "));
      Serial.print(xyEnd);
      Serial.print(F(", "));
      Serial.println(yDelta, 6);     
      Serial.println(F("#,x,y,theta,a,b,c"));
      for (unsigned int k = 0; k < EEPROMReadint(i + EEPROM_COUNT); k++){
        Serial.print(k);
        Serial.print(F(","));
        Serial.print(xStart);
        Serial.print(F(","));
        Serial.print(yStart);
        Serial.print(F(","));
        Serial.print(thetaT, 6);
        Serial.print(F(","));
        Serial.print(IREnE.xyTOa(xStart, yStart));
        Serial.print(F(","));
        Serial.print(IREnE.xyTOb(xStart, yStart));
        Serial.print(F(","));
        Serial.println(IREnE.xythetaTOc(xStart, yStart, thetaT));
        xStart += xDelta;
        yStart += yDelta;
        thetaT += thetaDelta;
      }
    }
    

    else {
      IREnE.abcPairIntersection(targets[0][0],targets[1][0],targets[0][1],targets[1][1],targets[0][2],targets[1][2]);
      Serial.println(targets[0][0]);
      Serial.println(F("Calculated Intersection"));
      Serial.print(IREnE.objectX());
      Serial.print(F(","));
      Serial.println(IREnE.objectY());
      Serial.println(F("#,x,y,theta,a,b,c"));
      for (unsigned int k = 0; k < EEPROMReadint(i + EEPROM_COUNT); k++){
        Serial.print(k);
        Serial.print(F(","));
        Serial.print(IREnE.objThetaDistTOx(thetaT, distT));
        Serial.print(F(","));
        Serial.print(IREnE.objThetaDistTOy(thetaT, distT));
        Serial.print(F(","));
        Serial.print(thetaT, 6);
        Serial.print(F(","));
        Serial.print(IREnE.xyTOa(IREnE.objThetaDistTOx(thetaT, distT), IREnE.objThetaDistTOy(thetaT, distT)));
        Serial.print(F(","));
        Serial.print(IREnE.xyTOb(IREnE.objThetaDistTOx(thetaT, distT),IREnE.objThetaDistTOy(thetaT, distT)));
        Serial.print(F(","));
        Serial.println(IREnE.xythetaTOc(IREnE.objThetaDistTOx(thetaT, distT), IREnE.objThetaDistTOy(thetaT, distT),thetaT));
        thetaT += thetaDelta;
      }
    }
    Serial.println();
}

