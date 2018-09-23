
boolean getCommand() {//   SERIAL COMMUNICATIONS   ############################################################################################################################################################
  static byte i = 0;
  static char stringChar[20];
  boolean newCommand = false;
  COMstring = "";
  COMvalue = 0;
  if (Serial.available()) {
    char COMinput = Serial.read();
    if (COMinput == terminator) newCommand = true;
    else if (COMinput > 47 || COMinput == 45) {
      stringChar[i] = COMinput;
      i++;
    }
    if (newCommand) {
      byte neg = 0;
      COMvalue = 0;
      for (byte j = 0; j < i; j++) {
        if (!COMvalue && stringChar[j] == '-') neg = 1;
        else if (stringChar[j] > 57) COMstring += String(stringChar[j]);
        else COMvalue = COMvalue * 10 + stringChar[j] - '0';
        stringChar[j] = 0;
      }
      i = 0;
      if (neg) COMvalue *= -1;
    }
  }
  return newCommand;
}


void parseCommand(){
  if (COMstring == "x") xT = COMvalue;
  else if (COMstring == "y") yT = COMvalue;
  else if (COMstring == "t") thetaT = COMvalue;
  else if (COMstring == "a") {
    A = COMvalue;
    saveEEPROM();
  }
  else if (COMstring == "b") {
    B = COMvalue;
    saveEEPROM();
  }
  else if (COMstring == "c") {
    C = COMvalue;
    saveEEPROM();
  }
  else if (COMstring == "bmin") {
    bMin = COMvalue;
    saveEEPROM();
  }
  else if (COMstring == "bmax") {
    bMax = COMvalue;
    saveEEPROM();
  }
  else if (COMstring == "h"){
    printEEPROMhistory();
  }
  else if (COMstring == "e"){
    for (unsigned int i = PROGRAM_HISTORY_EEPROM_START; i < EEPROM.length(); i++){
      EEPROM.write(i, 0);
    }
    Serial.println(F("DONE"));
  }
  else if (COMstring == "s"){
    unsigned int recIndex = findIndexEEPROM();
    Serial.print(F("Index:"));
    Serial.println(EEPROM.read(recIndex));
    Serial.println(F("Enter #shots,distance,A0,B0,C0,A1,B1,C1"));
    EEPROM.update(recIndex + EEPROM_TYPE, 1);
    EEPROMWriteint(recIndex + EEPROM_ABC, A);
    EEPROMWriteint(recIndex + EEPROM_ABC + 2, B);
    EEPROMWriteint(recIndex + EEPROM_ABC + 4, C);
    EEPROM.put(recIndex + EEPROM_THETA, float(-2));
    EEPROM.put(recIndex + EEPROM_THETA + 4, float (2));
    while (!getCommand()){};
    EEPROMWriteint(recIndex + EEPROM_COUNT, COMvalue);
    Serial.println(COMvalue);
    while (!getCommand()){};
    EEPROMWritelong(recIndex + EEPROM_DIST, COMvalue);
    Serial.println(COMvalue);
    recIndex += EEPROM_TARGETS;
    for (byte i = 0; i < 3; i++){
      while (!getCommand()){};
      EEPROMWritelong(recIndex,COMvalue);
      Serial.println(COMvalue);
      recIndex += 4;
    }
    for (byte i = 0; i < 3; i++){
      while (!getCommand()){};
      EEPROMWritelong(recIndex,COMvalue);
      Serial.println(COMvalue);
      recIndex += 4;
    }
    Serial.println(F("DONE"));
  }
}

