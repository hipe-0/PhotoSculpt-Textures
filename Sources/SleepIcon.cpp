/*
 * SleepIcon.cpp
 *
 *  Created on: 15 déc. 2010
 *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */

#include "SleepIcon.h"

const int          SleepIcon::ID_SLEEP_ICON_TIMER                = 57   ;
const int          SleepIcon::NB_MILISECONDS_PER_SLEEP_ICON_ANIM = 1000 ;
const int          SleepIcon::SLEEP_ICON_LOC_X                   = 379  ;
const int          SleepIcon::SLEEP_ICON_LOC_Y                   = 34   ;
const int          SleepIcon::SLEEP_ICON_SIZE                    = 5    ;

const unsigned char SleepIcon::SLEEP_ICON_COLOR_R                 = 218 ,
                    SleepIcon::SLEEP_ICON_COLOR_G                 = 120 ,
                    SleepIcon::SLEEP_ICON_COLOR_B                 = 50  ;

SleepIcon::SleepIcon()
{
  sleepIconCounter_     = 0     ;
  isSleepAnimationOn_   = false ;
}
void SleepIcon::startSleepIcon(HWND windowshandle)
{
  isSleepAnimationOn_=true;
  SetTimer(windowshandle, ID_SLEEP_ICON_TIMER, NB_MILISECONDS_PER_SLEEP_ICON_ANIM, NULL);
}

void SleepIcon::stopSleepIcon(HWND windowshandle)
{
  KillTimer(windowshandle,ID_SLEEP_ICON_TIMER);
  isSleepAnimationOn_=false;
}


void SleepIcon::drawSleepIcon(int x, int y, Image& image)
{
  if (isSleepAnimationOn_)
  {
    unsigned char color[3]={0,0,0}; // black
    if (sleepIconCounter_==1)
    {
      color[0]=SLEEP_ICON_COLOR_R; 
      color[1]=SLEEP_ICON_COLOR_G; 
      color[2]=SLEEP_ICON_COLOR_B;
    }

    image.draw_rectangle(
      SLEEP_ICON_LOC_X + x,
      SLEEP_ICON_LOC_Y + y,
      SLEEP_ICON_LOC_X+SLEEP_ICON_SIZE + x , // interface_.bottomBar_.getPositionY() 
      SLEEP_ICON_LOC_Y+SLEEP_ICON_SIZE + y ,
      color, 1.0f);
  }
}

void SleepIcon::tickSleepIcon()
{
  sleepIconCounter_ ++;
  sleepIconCounter_ %= NB_SLEEP_ICON_VALUES;
}

int SleepIcon::getSleepIconTimerId()
{
  return ID_SLEEP_ICON_TIMER;
}

