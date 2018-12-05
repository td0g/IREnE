// MultiStepper.cpp
//
// Copyright (C) 2015 Mike McCauley
// $Id: MultiStepper.cpp,v 1.2 2015/10/04 05:16:38 mikem Exp $

#include "MultiStepper.h"
#include "AccelStepper.h"

MultiStepper::MultiStepper()
    : _num_steppers(0)
{
}

//VTG 2018-11-17
void MultiStepper::setupBresenham(long absolute[]){
  //Serial.println("########### BRESENHAM");
_steppers[_bresenhamMotor]->d();            //1
  /*Serial.println("SetBres");
  for (byte i = 0; i < 3; i++){
  Serial.print(_steppers[i]->currentPosition());
  Serial.print(" -> ");
  Serial.println(absolute[i]);
  }*/
  _Dbresenham = _steppers[0]->currentPosition() - absolute[0];
  _Dbresenham = abs(_Dbresenham);  //This could be done in the following loop, but saves 8 us by doing it here
  _bresenhamMotor = 0;
  _D[0] = _Dbresenham;
  _steppers[_bresenhamMotor]->d();            //2
  //Serial.println("D:");
  //Serial.println(_D[0]);
  for (byte i = 1; i < _num_steppers; i++){
    _D[i] = _steppers[i]->currentPosition() - absolute[i];
    _D[i] = abs(_D[i]);
    //Serial.println(_D[i]);
    if (_D[i] > _Dbresenham) {
      _bresenhamMotor = i;
      _Dbresenham = _D[i];
    }
  }
  _steppers[_bresenhamMotor]->d();            //3
  if (!_Dbresenham) return;
  //Serial.print("tgt=");
  //Serial.println(_steppers[_bresenhamMotor]->targetPosition());
  _A = _steppers[_bresenhamMotor]->_acceleration;
  _S = _steppers[_bresenhamMotor]->maxSpeed();
  for (byte i = 0; i < _num_steppers; i++){
    _steppers[i]->setCurrentPosition(_steppers[i]->currentPosition());  //Resets _n
    _steppers[i]->moveTo(absolute[i]);  //Problem child, increments _n by one (ONE is ok, TWO is not)
  
    if (i != _bresenhamMotor){
      if (absolute[i] > _steppers[i]->currentPosition()) _steppers[i]->_direction = 1;
      else _steppers[i]->_direction = 0;
      unsigned long Aa = _steppers[i]->_acceleration;
      unsigned long Sa = _steppers[i]->maxSpeed();
      if (Aa < _A && _D[i]){
        Aa *= _Dbresenham;
        if (Aa < _A * _D[i] || !_A) _A = Aa / _D[i];
      }
      if (Sa < _S && _D[i]){
        Sa *= _Dbresenham;
        if (Sa < _S * _D[i] || !_S) _S = Sa / _D[i];
      }
      _D[i] *= 2;       //Going forward ALL values will be already doubled
      _slopeErr[i] = _D[i] - _Dbresenham;
    }
  }
  _steppers[_bresenhamMotor]->d();          //4
  _Dbresenham *= 2; //Going forward ALL dBresenham values will be already doubled
  _a = _steppers[_bresenhamMotor]->_acceleration;
  _s = _steppers[_bresenhamMotor]->maxSpeed();
  _steppers[_bresenhamMotor]->d();          //5
  _steppers[_bresenhamMotor]->setAcceleration(_A);
  _steppers[_bresenhamMotor]->d();        //6
  _steppers[_bresenhamMotor]->setMaxSpeed(_S);
  _steppers[_bresenhamMotor]->d();
//  Serial.print("BM=");
//  Serial.print(_bresenhamMotor);
//  Serial.print(" at ");
//  Serial.println(_S);
}

boolean MultiStepper::runBresenham(){
  return mainBresenham(1);
}

boolean MultiStepper::runBresenhamSpeed(){
  return mainBresenham(0);
}

//VTG 2018-11-17
boolean MultiStepper::mainBresenham(uint8_t _acc){
  byte _done = 0;
  long d = _steppers[_bresenhamMotor]->distanceToGo();
  if (_acc) _done = _steppers[_bresenhamMotor]->run();
  else {
    _steppers[_bresenhamMotor]->setSpeed(_S);
    _steppers[_bresenhamMotor]->runSpeedToPosition();
    if (_steppers[_bresenhamMotor]->distanceToGo()) _done = 1;
    else _steppers[_bresenhamMotor]->setSpeed(0);
  }
  d -= _steppers[_bresenhamMotor]->distanceToGo();
  d = abs(d);
  while (d){ //Step was taken
    d--;
    for (byte i = 0; i < _num_steppers; i++){
      if (i != _bresenhamMotor){
        _slopeErr[i] += _D[i];
        if (_slopeErr[i] > 0){
          //Serial.print(" ");
          //Serial.println(_steppers[i]->_direction);
          if (_steppers[i]->_direction) _steppers[i]->_currentPos++;
          else _steppers[i]->_currentPos--;
          _steppers[i]->step(1);    //Need to include a dummy parameter??
          _slopeErr[i] -= _Dbresenham;
        }
      }
      if (!_done) {
        _steppers[i]->d();
        
      }
    }
  }
  if (_done) return true;
  endBresenham();
  _Dbresenham = 0;
  return false;
}

void MultiStepper::endBresenham(){
  _steppers[_bresenhamMotor] -> setAcceleration(_a);
  _steppers[_bresenhamMotor] -> setMaxSpeed(_s);
}

//VTG 2018-11-17
void MultiStepper::stopBresenham(){
  _steppers[_bresenhamMotor]->stop();
}


boolean MultiStepper::addStepper(AccelStepper& stepper)
{
    if (_num_steppers >= MULTISTEPPER_MAX_STEPPERS)
	return false; // No room for more
    _steppers[_num_steppers++] = &stepper;
    return true;
}

void MultiStepper::moveTo(long absolute[])
{
    // First find the stepper that will take the longest time to move
    float longestTime = 0.0;

    uint8_t i;
    for (i = 0; i < _num_steppers; i++)
    {
	long thisDistance = absolute[i] - _steppers[i]->currentPosition();
	float thisTime = abs(thisDistance) / _steppers[i]->maxSpeed();

	if (thisTime > longestTime)
	    longestTime = thisTime;
    }

    if (longestTime > 0.0)
    {
	// Now work out a new max speed for each stepper so they will all 
	// arrived at the same time of longestTime
	for (i = 0; i < _num_steppers; i++)
	{
	    long thisDistance = absolute[i] - _steppers[i]->currentPosition();
	    float thisSpeed = thisDistance / longestTime;
	    _steppers[i]->moveTo(absolute[i]); // New target position (resets speed)
	    _steppers[i]->setSpeed(thisSpeed); // New speed
	}
    }
}

// Returns true if any motor is still running to the target position.
boolean MultiStepper::run()
{
    uint8_t i;
    boolean ret = false;
    for (i = 0; i < _num_steppers; i++)
    {
	if ( _steppers[i]->distanceToGo() != 0)
	{
	    _steppers[i]->runSpeed();
	    ret = true;
	}
    }
    return ret;
}

// Blocks until all steppers reach their target position and are stopped
void    MultiStepper::runSpeedToPosition()
{ 
    while (run())
	;
}

