
/*
 * Timer.cpp
 *
 *  Created on: 14 may 2009
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */

// 80 char per line standard :
/////////_/////////_/////////_/////////_/////////_/////////_/////////_/////////_

#include "Timer.h"
#include "windows.h"

Timer::Timer()
{
  reset();
}

float Timer::getSeconds() const
{
  unsigned long durationMiliSeconds = GetTickCount()-timeAtCreation_;

  return float(durationMiliSeconds) / 1000.f;
}

void Timer::reset()
{
  timeAtCreation_= GetTickCount();  // stores the time  
}
