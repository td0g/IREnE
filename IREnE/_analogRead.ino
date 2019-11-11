void analogReadAll(){  //############################################################################################################################################################
  static byte _voltageTimer;
  static byte adcPin;
  static byte adcNumber;
  static const byte adcPinCount = 3;
  static const byte adcPins[adcPinCount] = {c0Pin - 14, c1Pin - 14, voltageInPin - 14};
  static const byte adcHistSize = ADC_HISTORY_LENGTH;
  static int adcHist [2][adcHistSize];
  static byte i;
  static unsigned long _timer;
  static const byte _adcLookup[2][8] = {{200, 150, 114, 96, 85, 73, 64, 0},   //167 tR, 127 Up, 92 Down, 68 UpDown, 77 is DowntR, 101 UptR, 59 UpDowntR
                                                {0, 4, 2, 6, 1, 5, 3, 7}};    //675 tL, 514 Right, 371 Left, 272 RightLeft, 312 is LefttL, 408 RighttL, 239 RightLefttL
  
  if (bit_is_clear(ADCSRA, ADSC) && millis() > _timer){                 //New reading ready
    _timer += ADC_TIMER_INTERVAL;                                       //Increment Timer
    if (adcNumber < 2){
      adcHist[adcNumber][i] = ADC;                                        //Put sample into history
      unsigned long sum;
      for (byte j = 0; j < adcHistSize; j++) sum += adcHist[adcNumber][j];  //Add up all the data in history
      sum /= adcHistSize;                                                 //Divide the sum by the history size to get average
      byte match = 0;
      for (byte j = 0; j < adcHistSize; j++){                             //Count the sample points that are within tolerance
        int _t = adcHist[adcNumber][j] - sum;
        if (abs(_t) < ADC_HISTORY_TOLERANCE)match++;
      }
      #ifdef DEBUG_ADC 
        Serial.print(ADC / 4); Serial.print(F("  ")); Serial.print(match); Serial.print(F("  ")); if (adcNumber) Serial.println();
      #endif
      if (match == adcHistSize || ADC > 950) {                            //It is within tolerance - change the button pressed value
        byte _adcIndex;
        for (_adcIndex = 0; sum < ((int)_adcLookup[0][_adcIndex] << 2); _adcIndex++){}; //temp = _adcLookup[1][i];
        if (!adcNumber) buttonPressed = (buttonPressed & 0b00111000) | _adcLookup[1][_adcIndex];
        else buttonPressed = (buttonPressed & 0b00000111) | (_adcLookup[1][_adcIndex] << 3 & 0b00111000);
      }
      if (adcNumber){                                                     //Change the history slot position
        i++;
        i %= adcHistSize;
      }
    }
    else voltIn = (voltIn + ADC) >> 1;
    _voltageTimer++;
    adcNumber++; //Change the ADC Pin and begin a new reading
    adcNumber &= 1; 
    if (!_voltageTimer) adcNumber = 2;
    adcPin = adcPins[adcNumber];
    ADMUX =   bit (REFS0) | (adcPins[adcNumber] & 31);  // AVcc  
    ADCSRB = ((adcPin & 0b00100000) >> 2) + 1;
    bitSet(ADCSRA, ADSC);
  }
}

float inputVoltage(){
  float _v;
  _v = voltIn;
  _v /= VOLTAGE_DIVISOR;
  return _v;
}
