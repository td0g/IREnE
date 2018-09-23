#include "IREnEmath.h"
#include <Arduino.h>

#define DEBUG


IREnEmath::IREnEmath(uint16_t _a, uint16_t _c){
  A = _a;
  C = _c;
}

void IREnEmath::setAC(uint16_t _a, uint16_t _c){
  A = _a;
  C = _c;
}

long IREnEmath::abTOx(int32_t _a, uint32_t _b){
  float temp = _a;
  temp /= A;
  temp = sin(temp);
  temp *= _b;
  //temp /= B;
  #ifdef DEBUG
    Serial.println(F("  X FROM AB"));
    Serial.print(_a);
    Serial.print(F(", "));
    Serial.print(_b);
    Serial.print(F(" --> "));
    Serial.println(temp);
    Serial.println(F("  COMPUTED"));
    Serial.println();
  #endif
  return long(temp);
}

long IREnEmath::abTOy(int32_t _a, uint32_t _b){
  float temp = _a;
  temp /= A;
  temp = cos(temp);
  temp *= _b; 
  //temp /= B;
  #ifdef DEBUG
    Serial.println(F("  Y FROM AB"));
    Serial.print(_a);
    Serial.print(F(", "));
    Serial.print(_b);
    Serial.print(F(" --> "));
    Serial.println(temp);
    Serial.println(F("  COMPUTED"));
    Serial.println();
  #endif
  return long(temp);
}

float IREnEmath::acTOtheta(int32_t _a, int32_t _c){
  float temp = _a;
  temp /= A;
  float temp2 = _c;
  temp2 /= C;
  temp2 = PI - temp2;
  temp -= temp2;
  if (abs(temp - PI * 2) < abs(temp)) temp -= PI * 2;
  else if (abs(temp + PI * 2) < abs(temp)) temp += PI * 2;

  #ifdef DEBUG
    Serial.println(F("  THETA FROM A,C"));
    Serial.print(_a);
    Serial.print(F(", "));
    Serial.print(_c);
    Serial.print(F(" --> "));
    Serial.println(temp, 6);
    Serial.println(F("  COMPUTED"));
    Serial.println();
  #endif
  return temp;
}


  long IREnEmath::xyTOa(float _x, float _y){
  float tf = _x;
  tf /= _y;
  tf = atan(tf);
  tf *= A;
  return long(tf);
  }
  
  unsigned long IREnEmath::xyTOb(float _x, float _y){
    if (abs(_x) < 32700 && abs(_y) < 32700){
      long t0 = _x;
      t0 *= _x;
      long t1 = _y;
      t1 *= _y;
      t0 += t1;
      t0 = sqrt(t0);
      //t0 *= B;
      return t0;
    }
    else {
      float t0 = _x;
      t0 *= _x;
      float t1 = _y;
      t1 *= _y;
      t0 += t1;
      t0 = sqrt(t0);
      //t0 *= B;
      return t0;
    }
  }

  long IREnEmath::xythetaTOc(float _x, float _y, float _theta){
  float tf = _x;
  tf /= _y;
  tf = atan(tf);
  tf = _theta - tf;
  tf = PI + tf;
  long _d = C;
  _d *= tf;
  return _d;
  }


void IREnEmath::abcPairIntersection(long a0, long a1, uint32_t b0, uint32_t b1, long c0, long c1){
  //https://stackoverflow.com/questions/40330407/formula-to-find-intersection-point-of-two-lines
  //In this case, we are measuring wrong angle with acTOtheta...
  //So swap X, Y coordinates in formula
  long x0 = abTOx(a0, b0);
  long x1 = abTOx(a1, b1);
  long y0 = abTOy(a0, b0);
  long y1 = abTOy(a1, b1);
  float slope0 = tan(acTOtheta(a0, c0));
  float slope1 = tan(acTOtheta(a1, c1));
  
  #ifdef DEBUG
    Serial.println(F("  COMPUTING POINT FROM XY PAIR"));
    Serial.println(F("XY Pairs:"));
    Serial.print(x0);
    Serial.print(" ");
    Serial.println(y0);
    Serial.print(x1);
    Serial.print(" ");
    Serial.println(y1);
    Serial.println(F("Inverse Slopes:"));
    Serial.println(slope0, 8);
    Serial.println(slope1, 8);
  #endif
  
  float temp;
  if (isnan(slope0)){
    yO = y0;
    temp = yO - y1;
    temp *= slope1;
    xO = x1 + temp;
  }
  else if (isnan(slope1)){
    yO = y1;
    temp = yO - y0;
    temp *= slope0;
    xO = x0 + temp;
  }
  else {
    temp = slope0;
    temp *= y0;
    float temp2 = slope1;
    temp2 *= y1;
    yO = (x1 - x0 + temp - temp2);
    temp = (slope0 - slope1);
    yO /= temp;
    temp = yO - y0;
    temp *= slope0;
    xO = x0 + temp;
  }
  
  #ifdef DEBUG
    Serial.print(F("xO:"));
    Serial.print(xO);
    Serial.print(F(" yO:"));
    Serial.println(yO);
    Serial.println(F("  POINT COMPUTED"));
    Serial.println();
  #endif
}

float IREnEmath::objectX(float _x){
  if (_x) xO = _x;
  return xO;
}

float IREnEmath::objectY( float _y){
  if (_y) yO = _y;
  return yO;
}


float IREnEmath::objThetaDistTOx(float _theta, float _d){
  _theta = sin(_theta);
  _d *= _theta;
  return objectX() - _d;
}

float IREnEmath::objThetaDistTOy(float _theta, float _d){
  _theta = cos(_theta);
  _d *= _theta;
  return objectY() - _d;
}
