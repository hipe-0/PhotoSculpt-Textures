/*
 * Timer.h
 *
 *  Created on: 3 févr. 2009
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */

// 80 char per line standard :
/////////_/////////_/////////_/////////_/////////_/////////_/////////_/////////_

#pragma once 

class Timer
{
public:
  Timer(); // inits the timer

  float getSeconds() const; // get the time since timer was created in seconds
  void  reset();

private:
  // Script start timing
  unsigned long timeAtCreation_;
};
