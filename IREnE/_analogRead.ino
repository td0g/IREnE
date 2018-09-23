void analogReadAll(){  //############################################################################################################################################################
  static byte adcPin;
  static byte adcNumber;
  static const byte adcPinCount = 2;
  static const byte adcPins[adcPinCount] = {c0Pin - 14, c1Pin - 14};
  static const byte adcHistSize = ADC_HISTORY_LENGTH;
  static int adcHist [2][adcHistSize];
  static byte i;
  
  if (bit_is_clear(ADCSRA, ADSC)){
    adcHist[adcNumber][i] = ADC;
    unsigned int sum;
    for (byte j = 0; j < adcHistSize; j++){
      sum += adcHist[adcNumber][j];
    }
    sum /= adcHistSize;
    byte match;
    match = 0;
    for (byte j = 0; j < adcHistSize; j++){
      if (abs(adcHist[adcNumber][j] - sum) < ADC_HISTORY_TOLERANCE){
        match++;
      }
    }
    if (match == adcHistSize || ADC > 950) {
      byte temp;   
      //675 tR, 514 Up, 371 Down, 272 UpDown, 312 is DowntR, 408 UptR, 239 UpDowntR
      //tR, Up, Down
      //675 tL, 514 Right, 371 Left, 272 RightLeft, 312 is LefttL, 408 RighttL, 239 RightLefttL
      //tL, R, L
      if (sum < 255)temp = 0b00000111;        //239 is UDtR           
      else if (sum < 292) temp = 0b00000011;  //272 is UD
      else if (sum < 340) temp = 0b00000101;  //312 is DtR
      else if (sum < 385) temp = 0b00000001;  //371 is D
      else if (sum < 460) temp = 0b00000110;  //408 is UtR
      else if (sum < 600) temp = 0b00000010;  //514 is U
      else if (sum < 800) temp = 0b00000100;  //675 is tR
      else temp = 0b00000000;                 //1023 is none 
      if (!adcNumber) buttonPressed = (buttonPressed & 0b00111000) | temp;
      else {
        buttonPressed = (buttonPressed & 0b00000111) | (temp << 3 & 0b00111000);
      }
    }
    if (adcNumber){
      i++;
      i %= adcHistSize;
    }
    adcNumber++;
    adcNumber %= 2;
    adcPin = adcPins[adcNumber];
    ADMUX =   bit (REFS0) | (adcPin & 31);  // AVcc  
    ADCSRB = ((adcPin & 0b00100000) >> 2) + 1;
    bitSet(ADCSRA, ADSC);
  }
}
