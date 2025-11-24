/*
 * SleepIcon.h
 *
 *  Created on: 15 déc. 2010
 *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */

#pragma once

#include <windows.h>
#include "Image.h"

class SleepIcon
{
public:
  SleepIcon();

  void startSleepIcon        (HWND windowshandle);
  void stopSleepIcon         (HWND windowshandle);
  void drawSleepIcon         (int x, int y, Image &image);
  void tickSleepIcon         ();
  int  getSleepIconTimerId   ();

protected:
  static const int NB_SLEEP_ICON_VALUES  =2            ;
  static const int ID_SLEEP_ICON_TIMER                 ;
  static const int NB_MILISECONDS_PER_SLEEP_ICON_ANIM  ;
  static const int SLEEP_ICON_LOC_X                    ;
  static const int SLEEP_ICON_LOC_Y                    ;
  static const int SLEEP_ICON_SIZE                     ;

  static const unsigned char SLEEP_ICON_COLOR_R        ,
                             SLEEP_ICON_COLOR_G        ,
                             SLEEP_ICON_COLOR_B        ;
    
  volatile int    sleepIconCounter_                    ;
  bool           isSleepAnimationOn_                   ;

};

