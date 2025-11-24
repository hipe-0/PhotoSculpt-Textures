/*
 * PSInterfaceDisplay.cpp
 *
 *  Created on: 21 sept 2009
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */
#define _WIN32_WINNT 0x0500

#include <windows.h>
#include <fstream>
#include <stdexcept>
#include "PSApplication.h"
#include "resource.h"
#include "Win32Tools.h"
#include "ExportObj.h"
#include "Timer.h"
#include "Limit.h"
#include "Includes.h"
#include "DirectXControl.h"

void PSApplication::requestRedraw()
{
  //DEBUG(requestRedraw);

  // tells window that the current main window is invalidated : it will trigger 
  // a message WM_PAINT and a WM_DRAWITEM witch will trigger a displayToWindow()
  // but it may not be immediate
  InvalidateRect(interface_.windowsHandle_, NULL, FALSE);
}

void PSApplication::displayToWindow() // triggered by WM_PAINT message at any time!
{
  MUTEX;
  DEBUG(diply);

  setButtonLogic();

  UserInfo info = getUserInfo();

  interface_.displayToWindow(info, sources_, result_.getMode(), result_);

  MUTEXRELEASE;
}

