/*
  Written by Tyler Gerritsen (td0g)
  vtgerritsen@gmail.com
  IREnE (Inverted Radial, ExtionsioN, Eggtimer)

  a = primary rotation, b = extension, c = secondary (camera) rotation
  x, y coordinates in plane about origin (primary pivot)
  theta = camera orientation
  alpha = object --> camera orientation (alpha 0 deg = theta 180 deg)
  
  0.1
    2018-05-26
    Kinda Functional
	
	0.2
	  2018-09-12
		EEPROM History functional and can reproduce a program

  0.3
    2018-09-19
    IR ends at current location, set start point
      set IR start point by holding buttons - no need to repeat press
      EnE will still go from Target0 to Target 1
    Change xT, yT, x, y to float for improved accuracy
      Removed B coefficient from IREnE library
    Displays countdown during shooting
    Manages total time better - minimum total time calculated, but 0 time (MAX SPEED) also usable

  0.4
    2018-09-20
    EEPROM History Print now prints events in chronological order
    Move screen indicates '*' when unable to make complete IR pass (too far from target)
    Setting bMin position can be done after targets set - their positions are updated
    TR button on Main Menu over Move will rotate camera to last target's theta
      (useful for forcing linear movement with no rotation)
 */
 


//Definitions

  #define FIRMWARE_VERSION "0.4"

  //#define DEBUG
  
  #define MAX_TARGETS 2
  #define BAUD 119200
  #define A_DEFAULT 15384   //step / rad //1/8stepping
  #define B_DEFAULT 54     //step / mm    //Change to 4 for 2Metres!!! //1/2 stepping
  #define C_DEFAULT 1194    //step / rad: (400step * 4) * (75/16) 1/4 stepping
  #define B_MIN_DEFAULT 250
  #define B_MAX_DEFAULT 1500
  #define ADC_HISTORY_LENGTH 25
  #define ADC_HISTORY_TOLERANCE 20
  #define MINIMUM_STABILIZE_TIME_DEFAULT 1500
  #define MAXIMUM_STABILIZE_TIME_DEFAULT 4000
  #define PROGRAM_HISTORY_EEPROM_START 128
  #define PROGRAM_HISTORY_EEPROM_SIZE 128
  #define INTERVALOMETER_MOVE_TIME 2  //seconds
  #define INVERTED_RADIAL_SETUP_MOVE_RADS 0.05

//Pins
  #define aStepPin 3
  #define aDirPin 4
  #define aEnPin 2
  #define bStepPin 6
  #define bDirPin 7
  #define bEnPin 5
  #define cStepPin 8
  #define cDirPin 9
  #define cEnPin 10
  #define shutterPin 15
  #define aDisable PORTD |= 0b00000100
  #define aEnable PORTD &= 0b11111011
  #define bDisable PORTD |= 0b00100000
  #define bEnable PORTD &= 0b11011111
  #define cDisable PORTB |= 0b00000100
  #define cEnable PORTB &= 0b11111011
  
  #define LCD_RS 19
  #define LCD_EN 18
  #define LCD_D4 14
  #define LCD_D5 13
  #define LCD_D6 12
  #define LCD_D7 11

  #define CONTROLLER_CHANNEL_0_PIN 16
  #define CONTROLLER_CHANNEL_1_PIN 17

  #define SHUTTER_OPEN pinMode(shutterPin, OUTPUT);
  #define SHUTTER_CLOSE pinMode(shutterPin, INPUT);

//User Input
  #define BUTTON_D buttonPressed & 0b00000001
  #define BUTTON_U buttonPressed & 0b00000010
  #define BUTTON_L buttonPressed & 0b00001000
  #define BUTTON_R buttonPressed & 0b00010000
  #define BUTTON_TL buttonPressed & 0b00100000
  #define BUTTON_TR buttonPressed & 0b00000100
  
  //22k, 39k, 75k resistors
  byte buttonPressed = 0;
  const byte c0Pin = CONTROLLER_CHANNEL_0_PIN;
  const byte c1Pin = CONTROLLER_CHANNEL_1_PIN;
  
//EEPROM
  #include <EEPROM.h>
  #define EEPROM_TYPE 1
  #define EEPROM_COUNT 2
  #define EEPROM_ABC 4
  #define EEPROM_THETA 10
  #define EEPROM_X 18
  #define EEPROM_Y 26
  #define EEPROM_DIST 34
  #define EEPROM_TARGETS 72

//Display
  #include <LiquidCrystal.h>
  LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
  #define LCD_CLEAR_LINE lcd.print("        ");

//Motor
  #include <AccelStepper.h>
  AccelStepper a(1, aStepPin, aDirPin);
  AccelStepper b(1, bStepPin, bDirPin);
  AccelStepper c(1, cStepPin, cDirPin);
  byte cFlip;
  unsigned int motorTravelSpeed = 2000;
  unsigned int motorTravelAcc = 2000;

//Positioning Coefficients
  unsigned int A;   //Step / rad
  unsigned int B;   //Step / mm
  unsigned int C;   //Step / rad
  unsigned int bMin;
  unsigned long bMax;

//Math Library
  #include "IREnEmath.h"
  IREnEmath IREnE(A,C);

//Current head position
  float x;
  float y;
  float theta;
  
//Target head position
  float xT = x;
  float yT = y;
  float thetaT = 0;  //0.01 deg
  long distT;
  
//Current object position
  float thetaO;
  float thetaSTART;
  float thetaEND;
  unsigned long distO;
  unsigned long distOMax;
  
//Target Setup
  byte targetCount;
  long targets[MAX_TARGETS][3];

//Serial
  String COMstring;
  long COMvalue;
  const char terminator = '/';

//Menu Structure
  byte menuPosition;

//Intervalometer
  unsigned int totalTime;
  unsigned int shutterTime = 1;
  unsigned int numberShots = 2;
  unsigned int stabTimeMax;
  unsigned int stabTimeMin;

void setup() {
  digitalWrite(shutterPin, 0);
  SHUTTER_CLOSE;
  pinMode(c0Pin, INPUT);    //470 nF cap to gnd
  pinMode(c1Pin, INPUT);    //470 nF cap to gnd

  pinMode(aEnPin, OUTPUT);
  pinMode(bEnPin, OUTPUT);
  pinMode(cEnPin, OUTPUT);
  aDisable;
  bDisable;
  cDisable;
  
  Serial.begin(BAUD);
  Serial.println(F("IREnE Ready"));

  lcd.begin(16, 2);
  lcd.setCursor(0,0);
  lcd.print(F("IREnE Ready"));
  lcd.setCursor(0,1);
  lcd.print(F("FW "));
  lcd.print(FIRMWARE_VERSION);

//Motor setup
  a.setPinsInverted(true, false, true);
  b.setPinsInverted(true, false, true);
  c.setPinsInverted(false, false, true); //first is dir
  setMotorSpeed(1);

//Retrieve settings from EEPROM
  A = EEPROMReadint(0);
  B = EEPROMReadint(2);
  C = EEPROMReadint(4);
  bMin = EEPROMReadint(6);
  bMax = EEPROMReadint(8);
  bMin *= B;
  bMax *= B;
  bMax *= 10;
  stabTimeMin = EEPROMReadint(11);
  stabTimeMax = EEPROMReadint(13);
  cFlip = EEPROM.read(10);
  if (cFlip) c.setPinsInverted(true, false, true); //first param is dir

//Normalize remote control
  while (millis() < 500) analogReadAll();
  
//Other setup
  b.setCurrentPosition(bMin);
  IREnE.setAC(A, C);
}



void loop() {
  if (getCommand()) parseCommand(); 
  runMotor();
  analogReadAll();
  doButton();
  runUI();
}
