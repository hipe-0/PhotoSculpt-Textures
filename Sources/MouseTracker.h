/*
 * GraphicUserInterface.h
 *
 *  Created on: 15 déc. 2010
 *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */

#pragma once

class MouseTracker
{
public:
  MouseTracker() 
  { 
    isOn_ = false;
  }

  void set(HWND hwnd)
  { 
    if (hwnd == NULL)
      THROW_INTERNAL_ERROR("MouseTracker cannot track a null window handle (in set())");

    if (!isOn_)
    {
      TRACKMOUSEEVENT track;
      ZeroMemory(&track, sizeof(track));
      track.cbSize    = sizeof(track);
      track.dwFlags   = TME_LEAVE;
      track.hwndTrack = hwnd;
      BOOL isok = TrackMouseEvent( &track);
      if (!isok)
        THROW_INTERNAL_ERROR("Could not set TRACKMOUSEEVENT (in set())");
      isOn_ = true;
    }
  }
  void reset()
  {
    isOn_ = false;
  }

private:
  bool isOn_;
};
