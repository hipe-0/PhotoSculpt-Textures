/*
 * PSInterfaceExtract.cpp
 *
 *  Created on: 26 sept 2009
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
#include "UserInterrupt.h"

void PSApplication::extractDisplaceMap()
{
  DEBUG(extrDis);
  NEVER_EXECUTE_ON_WINDOWS_THREAD();

  sculpt_.clearCurrentSculptInProgress();

  interface_.changeTitle("");

  if (!sculpt_.isModel())
  {
    sculpt_.init(sources_.leftImage_,sources_.rightImage_);
    
    sculpt_.setFocalLength(sources_.leftFocal_);
  }
  sculpt_.setAlpha(sources_.getAlpha());

  initializeProgressionBar();

  DEBUG_WRITE("Stt Whilop");

  bool iscontinue = true;
  do
  {
    int step = sculpt_.computeStep();
    DEBUG_VALUE(" Step", step);
    if (step >= commandLine_.maxStep_)    break;
    
    iscontinue = sculpt_.growAndOptimizeThisLevel();
    
    ProgressDialog progress(result_.getModelSizeX()+result_.getModelSizeY()> 600); // 600 pixels
    progress.setText("Building 3D model");

    if (!commandLine_.isOutput_)
      doActionOnFirstThread(ACTION_ANALYSIS);

    createResultFromSculpt(); // peak at +380 Mb but +55 mb final, 30s

    if (!iscontinue)
      sculpt_.clear(); // free 100 mb before directX redraw
        
    // changes to 3d model tab
    if (sculpt_.computeStep() == 1 && // was 3
        (result_.getMode() == ACTION_MODE_SOURCES ))
      doActionOnFirstThread( ACTION_MODE_3D );
    else
      doActionOnFirstThread( result_.getMode() ); // +260 mb in 3d mode
  }
  while (iscontinue);

  DEBUG_WRITE("E Whlloop");

  isCurrentModelJustSculpted_ = true;
}
