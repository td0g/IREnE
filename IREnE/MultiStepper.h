// MultiStepper.h

#ifndef MultiStepper_h
#define MultiStepper_h

#include <stdlib.h>
#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#include <wiring.h>
#endif

#define MULTISTEPPER_MAX_STEPPERS 3 //VTG reduced to 4, as each instance uses at least 8 bytes of RAM

class AccelStepper;

/////////////////////////////////////////////////////////////////////
/// \class MultiStepper MultiStepper.h <MultiStepper.h>
/// \brief Operate multiple AccelSteppers in a co-ordinated fashion
///
/// This class can manage multiple AccelSteppers (up to MULTISTEPPER_MAX_STEPPERS = 10), 
/// and cause them all to move
/// to selected positions at such a (constant) speed that they all arrive at their
/// target position at the same time. This can be used to support devices with multiple steppers
/// on say multiple axes to cause linear diagonal motion. Suitable for use with X-Y plotters, flatbeds,
/// 3D printers etc
/// to get linear straight line movement between arbitrary 2d (or 3d or ...) positions.
///
/// Caution: only constant speed stepper motion is supported: acceleration and deceleration is not supported
/// All the steppers managed by MultiStepper will step at a constant speed to their
/// target (albeit perhaps different speeds for each stepper).
class MultiStepper
{
public:
    /// Constructor
    MultiStepper();

    //VTG 2018-11-17

    //This implementation of the Bresenham Algorithm tries to minimize changes to AccelStepper and MultiStepper libraries
    //Some variables have been moved to public (see below) and MultiStepper MAX Steppers has been adjusted to reduce RAM footprint
    //While several implementations methods have been considered, the author chose to base it in the MultiStepper library
    //It will hijack one of the AccelStepper instances and adjust the MaxSpeed and Acceleration parameters
    //Each time it detects a step has been taken in that instance, the algorithm will determine if other motors need to be stepped as well
    //Computation overhead is minimal compared to running multiple AccelStepper instances simultaneously

    //Following are some alternative implementation methods considered by the author
    
    //Another way to implement the algorithm is to create a dummy AccelStepper instance
    //That way, the MaxSpeed and Acceleration paramters don't need to be overwritten in any of the real AccelStepper instances
    //However, the memory overhead of another AccelStepper instance may be too great for a low-power controller

    //A brand new library could be written with the Bresenham algorithm in mind
    //However, the AccelStepper and MultiStepper libraries already provide lots of functionality in a light-weight profile

    //Following are the changes made to the AccelStepper and MultiStepper libraries
    
    //Accelstepper: Moved step(), _direction, _currentPos, _acceleration, and _maxSpeed to public
    //Multistepper: changed MAX_STEPPERS to 4

    //FUTURE CONSIDERATIONS: If AccelStepper can increment/decrement _currentPosition in the .step() function, then we don't need access to _direction and _currentPosition
    //                       It's a shame that _accleration and _maxSpeed need to be altered.  Is there some way to temporarily alter these with automatic revert when AccelStepper.run() is called?
        
    //Basic operation of Bresenham functions:  It is expected that runBresenham will be run until motor has stopped.
    //Otherwise the Bresenham driver motor will have altered parameters and this won't be corrected
    
    //When a new set of target positions have been set, setup the variables to run in a straight line
    //Acceleration will be implemented and no max speeds / accelerations will be violated
    void setupBresenham(long absolute[]);

    //Operates similar to AccelStepper.run(), acceleration will be implemented and movement will be straight-line
    //Return true if motors are still moving, false if they have reached destination
    boolean runBresenham();

    //Operates similar to AccelStepper.runSpeed(), no acceleration, movement will be straight-line
    //Return true if motors are still moving, false if they have reached destination
    boolean runBresenhamSpeed();

    //Operates similar to AccelStepper.stop(), acceleration is implemented
    void stopBresenham();

    //Reverts all motor settings (_acceleration and _maxSpeed) to original
    void endBresenham();

    //END VTG 2018-11-17

    

    /// Add a stepper to the set of managed steppers
    /// There is an upper limit of MULTISTEPPER_MAX_STEPPERS = 10 to the number of steppers that can be managed
    /// \param[in] stepper Reference to a stepper to add to the managed list
    /// \return true if successful. false if the number of managed steppers would exceed MULTISTEPPER_MAX_STEPPERS
    boolean addStepper(AccelStepper& stepper);

    /// Set the target positions of all managed steppers 
    /// according to a coordinate array.
    /// New speeds will be computed for each stepper so they will all arrive at their 
    /// respective targets at very close to the same time.
    /// \param[in] absolute An array of desired absolute stepper positions. absolute[0] will be used to set
    /// the absolute position of the first stepper added by addStepper() etc. The array must be at least as long as 
    /// the number of steppers that have been added by addStepper, else results are undefined.
    void moveTo(long absolute[]);
    
    /// Calls runSpeed() on all the managed steppers
    /// that have not acheived their target position.
    /// \return true if any stepper is still in the process of running to its target position.
    boolean run();

    /// Runs all managed steppers until they acheived their target position.
    /// Blocks until all that position is acheived. If you dont
    /// want blocking consider using run() instead.
    void    runSpeedToPosition();
    
private:
    /// Array of pointers to the steppers we are controlling.
    /// Fills from 0 onwards
    AccelStepper* _steppers[MULTISTEPPER_MAX_STEPPERS];

    /// Number of steppers we are controlling and the number
    /// of steppers in _steppers[]
    uint8_t       _num_steppers;

    //VTG 2018-11-19
    
    //Main bresenham algorithm (makes call to a single AccelStepper.run() or AccelStepper.runAtSpeed()
    //Called by runBresenham() and runBresenhamAtSpeed()
    boolean mainBresenham(uint8_t _acc);
    
    float _A;               //Bresenham Motor Acceleration
    float _S;               //Bresenham Motor Max Speed
    int32_t _Dbresenham;   //Bresenham Motor Absolute Distance To Go
    float _a;               //Original Motor Acceleration
    float _s;               //Original Motor Max Speed
    int32_t _D[MULTISTEPPER_MAX_STEPPERS];       //Each Individual Motor Absolute Distance To Go
    uint8_t _bresenhamMotor;                      //Bresenham Motor Number
    int32_t _slopeErr[MULTISTEPPER_MAX_STEPPERS]; //Each Individual Motor Bresenham Slope Error
    //End VTG 
};

/// @example MultiStepper.pde
/// Use MultiStepper class to manage multiple steppers and make them all move to 
/// the same position at the same time for linear 2d (or 3d) motion.

#endif
