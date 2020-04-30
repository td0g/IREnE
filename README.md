# IREnE
Advanced camera timelapse slider

By Tyler Gerritsen
vtgerritsen@gmail.com

For description see https://gerritsendesign.wordpress.com/2018/09/23/camera-slider-2-0-irene-diy/

## UI Manual:

Menu can be navigated using UP / DOWN buttons

### Main Menu:

  Move: RIGHT to select and manually move camera, T_RIGHT to revert to last target's theta
  
  Set Target: RIGHT to set a target, LEFT to reset all targets
  
  Set C Zero: RIGHT to set current C position as zero (Camera facing straight back at rotator)
  
  Total Time: LEFT / RIGHT / T_LEFT / T_RIGHT to increase / decrease total timelapse time
  ```
  If Total Time = 0, photos will be taken at max speed.
    
  The minimum total time (if NOT 0) is calculated by the controller.  A shorter amount of time cannot be selected.
    
  This is done to guarantee that the photos are equichronilogical (same time between photos).
  ```
  Number Shots: LEFT / RIGHT / T_LEFT / T_RIGHT to increase / decrease number of photos in timelapse
  
  Shutter Time: LEFT / RIGHT / T_LEFT / T_RIGHT to increase / decrease shutter open time
  
  Start IR: RIGHT to set current position as distance to subject and ending theta
  
  NEXT: LEFT / RIGHT to move camera about subject, T_RIGHT to set as start position and start timelapse
    
  Start EnE: RIGHT to move to first target position and begin timelapse, moving straight to second target position
  
  Settings: RIGHT to enter Settings submenu
 
 

### Moving:

  Controls:
  
  T_LEFT: Go back to main menu
    
  T_RIGHT: Toggle primary / secondary rotation
    
  LEFT / RIGHT: Rotate primary or secondary rotator
    
  UP / DOWN: Extend or retract boom
    
  Notes:
  
  If the subject position is set, an asterisk will appear on the Moving screen when a complete rotation cannot be completed.
    
    
    
### Settings:

  A, C: Steps per Rad.  Adjust using LEFT / RIGHT.
  
  B: Steps per MM.  Adjust using LEFT / RIGHT.
  
  bMin, bMax: Boom extension limits.  Adjust using LEFT / RIGHT / T_LEFT / T_RIGHT.
  
  A Manual Calibrate: Make exactly one half turn to set the A coefficient.  LEFT / RIGHT to move A, T_RIGHT to set as one half turn.
  
  Camera Side: RIGHT to reverse secondary rotator direction polarity.
  
  Test Shutter: RIGHT to activate shutter (for testing connection).
  
  Back: RIGHT to save settings and return to main menu.
  
  
  
### UART:

  When connected to a PC, the recent history can be printed by entering 'h/'.
  
  A dummy IR program can be set by entering 's/'.
  
  The A, B, and C coefficients can be set by the 'aXXX/', 'bXXX/', and 'cXXX/' commands.
  
  
## License

Documentation is licensed under a [Creative Commons Attribution 4.0 International License](https://creativecommons.org/licenses/by/4.0/)

Software is licensed under a [GNU GPL v3 License](https://www.gnu.org/licenses/gpl-3.0.txt)
