//Written by Tyler Gerritsen (td0g)
//2018-09-07

//Trig solver for IREnE (Inverse Radial, ExtensioN, Eggtimer) timelapse tool
//Given motors to control a pivoting and extending arm with a rotating head (on which a camera mounts),
//  this library provides the mathematics to move the camera to any desired position

//The central pivot, or PRIMARY, is located at x,y = 0,0
//The camera's position is defined by x, y, and theta
//  where x is lateral movement (right +)
//  y is longitudinal movement (forward +)
//  and theta is the camera's viewing angle (clockwise positive, viewing directly forward = 0)

//Keep in mind that degrees are never used - ONLY RADIANS
//  (theta = pi means the camera is viewing backward)

//The units are arbitrary and are set by the B coefficient
//  for instance, changing it from 108 (step / mm) to 2743 (step / inch)
//  will not disrupt the math.
//  Doing this WILL DECREASE ACCURACY, as the camera will only move to discrete positions
//  defined by the B coefficient.
//  On the other hand, using microns is not a good idea as the program will experience
//  an interger overflow.

#ifndef __IREnEmath__
#define __IREnEmath__

#include <inttypes.h>

class IREnEmath {

public:
    //Constructor
    //Accepts the unit steps per amount travelled
    //_a is the primary's steps / radian.  Take the number of steps per full revolution and divide by 2*pi.
    //_b is the extender's steps / unit.
    //_c is the secondary's steps / radian.
  IREnEmath(void);

    //For updating the A, B, C coefficients.
  void setAC(uint16_t _a,  uint16_t _c);

    //Given a and b, return x
  long abTOx(int32_t _a, uint32_t _b);

    //Given a and b, return y
  long abTOy(int32_t _a, uint32_t _b);

    //Given a and c, return theta
  float acTOtheta(int32_t _a, int32_t _c);

    //Given x and y, return a
  long xyTOa(float _x, float _y);

    //Given x and y, return b
  unsigned long xyTOb(float _x, float _y);

    //Given x, y, and theta, return c
  long xythetaTOc(float _x, float _y, float _theta);

    //Given a pair of a, b, and c coordinates, return the intersection of the lines and save as the object coordinates
  void abcPairIntersection(int32_t a0, int32_t a1, uint32_t b0, uint32_t b1, int32_t c0, int32_t c1);

    //Return the object X coordinate
  float objectX(float _x = 0);

    //Retun the object Y coordinate
  float objectY(float _y = 0);

    //Given theta and distance to a previously calculated object, return X coordinate
  float objThetaDistTOx(float _theta, float _d);
  
    //Given theta and distance to a previously calculated object, return Y coordinate
  float objThetaDistTOy(float _theta, float _d);
  
//protected:


private:
  uint16_t A;
  uint16_t C;
  float xO;
  float yO;
};

#endif
