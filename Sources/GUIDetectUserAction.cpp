/*
 * PSInterfaceDetectUserAction.cpp
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

void chooseBestAction(int& current, int new_action)
{
  if (new_action>current)
    current = new_action;
}

int GraphicUserInterface::detectPopup(int currentmode)
{
  int launch_action_popup = NOACTION;

  bool isMouseC = frontPage_.isInside(mouseX_, mouseY_, ACTION_IMAGE_CENTER      );
  bool isMouseL = frontPage_.isInside(mouseX_, mouseY_, ACTION_IMAGE_SOURCE_LEFT );
  bool isMouseR = frontPage_.isInside(mouseX_, mouseY_, ACTION_IMAGE_SOURCE_RIGHT);
  bool isMouseA = frontPage_.isInside(mouseX_, mouseY_, ACTION_IMAGE_SOURCE_ALPHA);

  static bool previousC = false, previousL = false, previousR = false, previousA = false;

  if (currentmode == ACTION_MODE_SOURCES)
  {
    popup3d_  .hide();
    popupSave_.hide();

    // is there a change?
    if (previousL != isMouseL || previousR != isMouseR || previousA != isMouseA)
    {
      popupLoadL_.hide();
      popupLoadR_.hide();      
      popupLoadAlpha_.hide();      
      
      if (isMouseL)  popupLoadL_.show();
      if (isMouseR)  popupLoadR_.show();
      if (isMouseA)  popupLoadAlpha_.show();

      launch_action_popup = ACTION_REPAINT_SCREEN ;
    }
  }
  else
  {
    popupLoadL_.hide();
    popupLoadR_.hide();
    popupLoadAlpha_.hide();

    if (isMouseC)
    {
      popup3d_  .show();
      popupSave_.show();
    }
    else
    {
      popup3d_  .hide();
      popupSave_.hide();
    }  

    if (previousC != isMouseC)
      launch_action_popup = ACTION_REPAINT_SCREEN ;
  }

  previousC = isMouseC ;  previousL = isMouseL ;  previousR = isMouseR ; previousA = isMouseA;

  return launch_action_popup;
}

int PSApplication::detectUserAction()
{
  return interface_.detectUserAction(result_);
}

int GraphicUserInterface::detectUserAction(Result& result)
{
  int action = NOACTION;
  const float PI = 3.141592f;
  const float MOUSE_ROTATE_FACTOR = 4.0f / (PI / 180.0f) ;
  float const TRANSL_FACTOR = 20.0f, TRANSL_LIMIT = 20.0f;
 
  chooseBestAction(action, bottomSolo_ .detectUserAction ( mouseX_, mouseY_ , mouseButton_ ));

  if (!isSculptBarMinimised_)
    chooseBestAction(action, bottomBar_  .detectUserAction ( mouseX_, mouseY_ , mouseButton_ ));
  else
    chooseBestAction(action, bottomMiniBar_ .detectUserAction ( mouseX_, mouseY_ , mouseButton_ ));

  chooseBestAction(action, topTabBar_    .detectUserAction ( mouseX_, mouseY_ , mouseButton_ ));
  chooseBestAction(action, detectPopup(result.getMode()));

  switch (result.getMode())
  {
  case ACTION_MODE_SOURCES:
    chooseBestAction(action, popupLoadL_    .detectUserAction ( mouseX_, mouseY_ , mouseButton_ ));
    chooseBestAction(action, popupLoadR_    .detectUserAction ( mouseX_, mouseY_ , mouseButton_ ));
    chooseBestAction(action, popupLoadAlpha_.detectUserAction ( mouseX_, mouseY_ , mouseButton_ ));
    break;

  case ACTION_MODE_3D:
    {
      static int lastMouseX = mouseX_;
      static int lastMouseY = mouseY_;

      // has mouse left button been pressed?
      if ((mouseButton_ && !mouseButtonR_)  && mouseX_ > 0 && lastMouseX > 0)
      {
        float deltaX = float( mouseX_ - lastMouseX ) / float (image_.dimx()); // positive if going right
        float deltaY = float( mouseY_ - lastMouseY ) / float (image_.dimx()); // positive if going down

        result.azimuth_ -= deltaX * MOUSE_ROTATE_FACTOR;
        result.zenith_  += deltaY * MOUSE_ROTATE_FACTOR;
        limit(result.azimuth_, -90.0f, 90.0f);
        limit(result.zenith_,  -90.0f, 90.0f);

        chooseBestAction(action, ACTION_REPAINT_SCREEN);
      }

      // has mouse right button been pressed?
      if ((mouseButtonR_ && !mouseButton_) && mouseX_ > 0 && lastMouseX > 0)
      {
        float deltaX = float( mouseX_ - lastMouseX ) / float (image_.dimx()); // positive if going right
        float deltaY = float( mouseY_ - lastMouseY ) / float (image_.dimx()); // positive if going down

        result.xTrans_  += deltaX * TRANSL_FACTOR;
        result.yTrans_  += deltaY * TRANSL_FACTOR;
        limit(result.xTrans_, -TRANSL_LIMIT, TRANSL_LIMIT);
        limit(result.yTrans_, -TRANSL_LIMIT, TRANSL_LIMIT);

        chooseBestAction(action, ACTION_REPAINT_SCREEN);
      }

      // has mouse left and right button been pressed together?
      if ((mouseButtonR_ && mouseButton_) && mouseX_ > 0 && lastMouseX > 0)
      {
        float deltaX = float( mouseX_ - lastMouseX ) / float (image_.dimx()); // positive if going right
        float deltaY = float( mouseY_ - lastMouseY ) / float (image_.dimx()); // positive if going down

        result.roll_ -= deltaX * MOUSE_ROTATE_FACTOR;
        limit(result.roll_, -90.0f, 90.0f);

        chooseBestAction(action, ACTION_REPAINT_SCREEN);
      }
      
      if (result.isAnimation_)
        result.roll_ -= 0.005f * MOUSE_ROTATE_FACTOR;

      // has mouse wheel been activated?
      if (mouseWheel_ != 0)
      {
        float deltaW = float( mouseWheel_ ) / float (WHEEL_DELTA);
        result.modelScale_  *= 1.0f + deltaW * 0.1f;
        limit(result.modelScale_, 0.1f , 10.0f);

        mouseWheel_ = 0;

        chooseBestAction(action, ACTION_REPAINT_SCREEN);
      }

      chooseBestAction(action, popupSave_ .detectUserAction( mouseX_ , mouseY_ , mouseButton_));
      chooseBestAction(action, popup3d_   .detectUserAction( mouseX_ , mouseY_ , mouseButton_));

      lastMouseX = mouseX_;
      lastMouseY = mouseY_;
    }
    break;

  case ACTION_MODE_DISPLACE:     case ACTION_MODE_NORMAL:   case ACTION_MODE_AMBOCC:
  case ACTION_MODE_SPECULAR:         case ACTION_MODE_DIFFUSE:  case ACTION_MODE_ALPHA:
  case ACTION_MODE_BUMP:             case ACTION_MODE_LIMDISP: case ACTION_MODE_TILE_MASK:
    {
      chooseBestAction(action, popupSave_ .detectUserAction( mouseX_, mouseY_ , mouseButton_));
      chooseBestAction(action, popup3d_   .detectUserAction( mouseX_, mouseY_ , mouseButton_));
    }
    break;

  case ACTION_MODE_OLD_3D:
    {
      static int lastMouseX = mouseX_;
      static int lastMouseY = mouseY_;

      // has mouse left button been pressed?
      if ((mouseButton_ && !mouseButtonR_)  && mouseX_ > 0 && lastMouseX > 0)
      {
        float deltaX = float( mouseX_ - lastMouseX ) / float (image_.dimx()); // positive if going right
        float deltaY = float( mouseY_ - lastMouseY ) / float (image_.dimx()); // positive if going down

        result.azimuth_ -= deltaX * MOUSE_ROTATE_FACTOR;
        result.zenith_  += deltaY * MOUSE_ROTATE_FACTOR;
        limit(result.azimuth_, -90.0f, 90.0f);
        limit(result.zenith_,  -90.0f, 90.0f);

        chooseBestAction(action, ACTION_MODE_OLD_3D);
      }
      chooseBestAction(action, popupSave_ .detectUserAction( mouseX_, mouseY_ , mouseButton_));
      chooseBestAction(action, popup3d_   .detectUserAction( mouseX_, mouseY_ , mouseButton_));

      lastMouseX = mouseX_;
      lastMouseY = mouseY_;
    }
    break;

  }

  return action;
}
