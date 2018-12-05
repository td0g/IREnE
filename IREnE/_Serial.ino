
boolean getCommand() {//   SERIAL COMMUNICATIONS   ############################################################################################################################################################
  if (Serial.available()){
    char _c = Serial.read();
    if (_c == 'h')printEEPROMhistory();
    else if (_c == 'r') resetEEPROMsettings();
  }
}

