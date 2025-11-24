/*
 * PSInterfaceExtract3Photos.cpp
 *
 *  Created on: 24 nov 2010
   *      Author: Hippolyte Mounier, Copyright 2010 All rights reservedlyte Mounier, Copyright 2010 All rights reserved
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
#include "Optimizer.h"
#include "Convert.h"

#define NB_ITER_SCALE_OFFSET_SEARCH 10000
#define SCALE_SEARCH 0.5f
#define OFFSET_SEARCH 1.0f
  
float findScaleAndOffsetBetweenMap(IntMap& map1, IntMap& map2, float& scale, float& offset)
{
  int sx = map1.getSizeX();
  int sy = map1.getSizeY();

  Optimizer optimizer;
  optimizer.declareParameter (&scale,  SCALE_SEARCH );
  optimizer.declareParameter (&offset, OFFSET_SEARCH);

  for (int iter=0; iter<NB_ITER_SCALE_OFFSET_SEARCH; iter++)
  {
    float score = 0.0f;
    int   count = 0;

    int xstep = sx/10;
    if (xstep == 0) 
      xstep = 1;
    int ystep = sy/10;
    if (ystep == 0) 
      ystep = 1;

    for   (int x=0; x < sx; x+=xstep)
      for (int y=0; y < sy; y+=ystep)
      {
        float currentvalue1 = float (map1(x,y));
        float currentvalue2 = float (map2(x,y)) * scale + offset*sx;

        float sq = (currentvalue1-currentvalue2)/float(sx);
        score += sq * sq;
        count ++;
      }

    score = sqrtf(score) / count ;

    optimizer.shuffleToMinimizeScore(score);
  }

  float bestscore = optimizer.getBestScore();
  optimizer.retrieveBestParameters();

  return bestscore;
}


void mixmap2(VectorMap& map1, VectorMap& map2)
{
  IntMap imap1 = getMapX(map1);
  IntMap imap2 = -getMapX(map2);


  static float scale  = 1.0;
  static float offset = 0.0;

  float score = findScaleAndOffsetBetweenMap(imap1, imap2, scale, offset);

  int sx = map1.getSizeX();

  IntMap mean = ( imap1 + (imap2 * scale) + offset*sx ) * 0.5f ;

  setMapX(map1, imap1);
  setMapX(map2, -imap2);
}

void PSApplication::extractDisplaceMap3Photos()
{
  DEBUG(extrDisMap3P);
  NEVER_EXECUTE_ON_WINDOWS_THREAD();

  sculpt_.clearCurrentSculptInProgress();

  interface_.changeTitle("");

  if (!sculpt_.isModel())
  { // make a set of resized left and right texture into different smaller versions
    sculpt_          .init(sources_.leftImage_,sources_.rightImage_     );
    sculptAdditional_.init(sources_.leftImage_,sources_.additionalImage_);

    sculpt_          .setFocalLength(sources_.leftFocal_);
    sculptAdditional_.setFocalLength(sources_.leftFocal_);
  }

  // Initialises the alpha map = read the alpha map from the "Result" container
  sculpt_            .setAlpha(sources_.getAlpha());
  sculptAdditional_  .setAlpha(sources_.getAlpha());

  initializeProgressionBar();

  DEBUG_WRITE("Stahilop");  

  bool iscontinue = true;
  do
  {
    DEBUG_VALUE(" Step", sculpt_.computeStep());

    iscontinue = sculpt_           .growAndOptimizeThisLevel();
    iscontinue = sculptAdditional_ .growAndOptimizeThisLevel();

    {
      VectorMap map1 = sculpt_           .getMap();
      VectorMap map2 = sculptAdditional_ .getMap();

      mixmap2(map1, map2);

      sculpt_           .setMap(map1, true); // true says 'sculpting is done'
      sculptAdditional_ .setMap(map2, true);
    }
   
    doActionOnFirstThread(ACTION_ANALYSIS);

    createResultFromSculpt();
    
    if (!iscontinue)
    {
      sculpt_          .clear(); // free mem before directX redraw
      sculptAdditional_.clear();
    }
      
    isCurrentModelJustSculpted_ = false;
      
    // changes to 3d model tab at step 1
    if (sculpt_.computeStep() == 1 &&
        (result_.getMode() == ACTION_MODE_SOURCES ))
      // this message starts directx too, so no need to do it again afterwards
      doActionOnFirstThread( ACTION_MODE_3D );
    else
      doActionOnFirstThread( result_.getMode() );
  }
  while (iscontinue);

  DEBUG_WRITE("Enhiloop");

  isCurrentModelJustSculpted_ = true;
}
