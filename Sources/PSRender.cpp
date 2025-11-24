/*
 * PSInterfaceRender.cpp
 *
 *  Created on: 18 avr. 2009
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */
#define _WIN32_WINNT 0x0500

#include "PSApplication.h"
#include "Limit.h"
#include "Win32Tools.h"
#include "File.h"

const unsigned char MAXIMUM_OPACITY = 255 ;

void PSApplication::reinit3DDisplay()
{
  ALWAYS_EXECUTE_ON_WINDOWS_LOOP_THREAD();

  if (result_.getMode() != ACTION_MODE_3D)
    return;
  
  if (!result_.isResult())
    return;

  if (interface_.directX_.isOn())
    stop3DDisplay();

  // Reads the texture from the result object
  PImage texture = new Image(result_.getImage());

  PFloatMap model = result_.getFloatMap();

  PImage alpha = new Image;
  
  if (sources_.isAlpha())
  {
    Result tempresult= result_;
    tempresult.setAlpha(sources_.getAlpha());
    tempresult.updateAll(result_.getOriginalMap(), sources_.leftImage_, 
                         ACTION_MODE_ALPHA, result_.getShader());
    alpha = new Image(tempresult.getImage());
  }

  if (!interface_.image_.is_empty())
  {
    int sx = interface_.image_.dimx();
    int sy = interface_.image_.dimy() - interface_.getBottomBarSizeY() - interface_.topTabBar_.getSizeY();

    interface_.frontPage_.changeButton(ACTION_IMAGE_CENTER, 0, 
      interface_.topTabBar_.getSizeY(), sx, sy+interface_.topTabBar_.getSizeY());

    float focal = sources_.getEffectiveFocalLength() / result_.getCropper().getWidthRatio();

    bool isOk = interface_.directX_.initD3D(interface_.contentZone_.getWindowNr(),  sx, sy,
                     model, texture, alpha, result_.getShader(), result_.isSeamless(), focal);
    if (!isOk)
      THROW_MESSAGE_TO_USER(DIRECTX);
  }
}

void PSApplication::change3DShader(int shadercode)
{
  ALWAYS_EXECUTE_ON_WINDOWS_LOOP_THREAD();

  if (shadercode != ACTION_3D_TEXTURE   && 
      shadercode != ACTION_3D_WHITE     && 
      shadercode != ACTION_3D_WIREFRAME)
    THROW_INTERNAL_ERROR( "Wrong Parameter (in change3DShader())" );

  if (result_.getMode() != ACTION_MODE_3D && result_.getMode() != ACTION_MODE_OLD_3D )
    return;
  
  if (!result_.isResult())
    return;

  result_.updateShaderOnly(shadercode);

  // Reads the texture from the result object
  PImage texture = new Image(result_.getImage());

  if ( result_.getMode() == ACTION_MODE_3D )
    if (!interface_.image_.is_empty())
      interface_.directX_.changeShaderOnly(texture, shadercode, result_.isSeamless());
  
  if ( result_.getMode() == ACTION_MODE_OLD_3D )
    postActionOnFirstThread(ACTION_MODE_OLD_3D); // redraw
}


void PSApplication::stop3DDisplay()
{
  //ALWAYS_EXECUTE_ON_WINDOWS_LOOP_THREAD();

  if (interface_.directX_.isOn())
    interface_.directX_.cleanD3D();
}
