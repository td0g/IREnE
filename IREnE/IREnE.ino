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

  0.5
    2018-09-25
    Fixed bug where A & C would not stop moving in Move menu
    Fixed intervalometer timer
    Added countdown to next photo
    Fixed IREnE Library bug (atan domains)

  0.5.1
    2018-10-11
    Memory-saving changes
    Controller Reading Algorithm Improvements

  0.5.2
    2018-10-21
    Bug Fixes

  0.5.3
    2018-11-07
    Bug Fixes

  0.6
    2018-11-19
    Modifications to AccelStepper and MultiStepper libraries
    Now implement a bresenham algorithm to allow continuous, coordinated movement
    Video can be taken while in IR setup menu, as motion is continuous

  0.6.1
    2018-11-21
    IR Movie Speed user-selectable
    IR Speeds 0 - 5, where 0 is slowest and 4 is fastest constant speed, while 5 is max speed (not constant during rotation)
    XY Movie

  0.6.2
    2018-11-22
    IR Speed now in terms of seconds per revolution
    IR Acceleration in terms of time to max speed

  0.6.3
    2018-11-26
    Bug Fixes

  0.6.4
    2018-11-28
    Edits to AccelStepper
    (Multi-Stepping, reducing run() overhead)
    Migrating linear movement to movie() function
    Optimizing analogReadAll()

  0.6.5
    2018-11-30
    Bug Fixes

  0.6.6
    2018-12-01
    Changing IR Movie speed from sec/rev to mm/s
    Implement IR acceleration at max speed

  1.0
    2018-12-04
    Added gcode parser for CNC Pancake Printing!
 */
 


//Definitions

  #define FIRMWARE_VERSION "1.0"

  //#define DEBUG
  //#define DEBUG_ADC

  #define CNC_PANCAKE

  #define COORDINATED_MOVE m.runBresenhamSpeed()
  #define COORDINATED_MOVE_SETUP m.setupBresenham
  
  #define BAUD 115200
  #define A_DEFAULT 15384   //step / rad //1/8stepping
  #define B_DEFAULT 54     //step / mm
  #define C_DEFAULT 5371    //step / rad: (400step * 4) * (75/16) 1/4 stepping
  #define B_MIN_DEFAULT 250
  #define B_MAX_DEFAULT 1500
  #define MOTOR_STEP_SPEED_MAX 3000
  
  #define ADC_HISTORY_LENGTH 3
  #define ADC_HISTORY_TOLERANCE 10
  #define ADC_TIMER_INTERVAL 2
  
  #define MINIMUM_STABILIZE_TIME_DEFAULT 1500
  #define MAXIMUM_STABILIZE_TIME_DEFAULT 4000
  
  #define PROGRAM_HISTORY_EEPROM_START 128
  #define PROGRAM_HISTORY_EEPROM_SIZE 128
  
  #define INTERVALOMETER_MOVE_TIME 2  //seconds
  #define INTERVALOMETER_POST_PHOTO_DELAY 500

  #define IR_MIN_OBJ_DIST 1000                //B Units
  #define MOVIE_JERK 0.05

//Pins
  #define aStepPin 3
  #define aDirPin 4
  #define bStepPin 6
  #define bDirPin 7
  #define motorEnPin 5
  #define cStepPin 8
  #define cDirPin 9
  #define cEnPin 10
  #define shutterPin 15
  #define motorDisable PORTD |= 0b00100000
  #define motorEnable PORTD &= 0b11011111

  #define voltageInPin 18
  
  #define LCD_RS 19
  #define LCD_EN 2
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
  #include "AccelStepper.h"
  #include "MultiStepper.h"
  AccelStepper a(1, aStepPin, aDirPin);
  AccelStepper b(1, bStepPin, bDirPin);
  AccelStepper c(1, cStepPin, cDirPin);
  MultiStepper m;
  byte cFlip;
  unsigned int motorTravelSpeed = MOTOR_STEP_SPEED_MAX;
  unsigned int motorTravelAcc = 2000;

//Positioning Coefficients
  unsigned int A;   //Step / rad
  unsigned int B;   //Step / mm
  unsigned int C;   //Step / rad
  unsigned int bMin;
  unsigned int bMinMM;
  unsigned long bMax;
  unsigned int bMaxMM;

//Math Library
  #include "IREnEmath.h"
  IREnEmath IREnE;

//Current head position
  float x;            //extensionUnits
  float y;            //extensionUnits
  float theta;        //Rads
  
//Target head position
  float xT = x;       //extensionUnits
  float yT = y;       //extensionUnits
  float thetaT = 0;   //Rads
  long distT;         //extensionUnits
  
//Current object position
  float thetaO;           //Rads
  float thetaSTART;       //Rads
  float thetaEND;         //Rads
  unsigned long distO;    //Rads
  unsigned long distOMax; //Rads
  
//Target Setup
  byte targetCount;
  long targets[2][3];

//Menu Structure
  byte menuPosition;

//Movie Speed
  byte movieSpeed = 1;
  float movieAccelTime;
  long movieNextPos[3] = {0, 0, 0};
  unsigned int motorMaxSpeed[3] = {1500, 2200, 2000};
  byte pancakePrinting = 0;
  
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

  pinMode(motorEnPin, OUTPUT);
  motorDisable;
  
  Serial.begin(BAUD);
  lcd.begin(16, 2);
  lcd.clear();
  delay(100);

  lcd.setCursor(0,0);
  lcd.print(F("- IREnE Ready -"));
  lcd.setCursor(0,1);
  lcd.print(F("FW "));
  lcd.print(FIRMWARE_VERSION);

//Motor setup
  a.setPinsInverted(true, false, true);
  b.setPinsInverted(true, false, true);
  c.setPinsInverted(false, false, true); //first is dir
  m.addStepper(a);
  m.addStepper(b);
  m.addStepper(c);
  setMotorSpeed(1);
  a.setMaxFrequency(900);
  b.setMaxFrequency(1500);
  c.setMaxFrequency(800);

//Retrieve settings from EEPROM
  A = EEPROMReadint(0);
  B = EEPROMReadint(2);
  C = EEPROMReadint(4);
  bMinMM = EEPROMReadint(6);
  bMaxMM = EEPROMReadint(8);
  bMin = bMinMM * B;
  bMax = (long)bMaxMM * B;
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



void loop() { //########################################################################################################################
  getCommand(); 
  runMotor();
  analogReadAll();
  doButton();
  runUI();
}
