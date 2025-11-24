/*
 * WindowsProcedure.cpp
 *
 *  Created on: 28 07 2009
 *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */


#include "stdafx.h"
#include "Image.h"
#include "PSApplication.h"
#include <stdexcept>

extern PSApplication MainApp;

extern HINSTANCE hInst;								// instance actuelle

//
//  FONCTION : WindowsProcedure(HWND, UINT, WPARAM, LPARAM)
//
//  BUT :  traite les messages pour la fenêtre principale.
//
//  WM_COMMAND	- traite le menu de l'application
//  WM_PAINT	- dessine la fenêtre principale
//  WM_DESTROY	- génère un message d'arrêt et retourne
//
//
LRESULT CALLBACK WindowsProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

  try
  {
	  switch (message)
	  {
	  case WM_COMMAND:
		  wmId    = LOWORD(wParam);
		  wmEvent = HIWORD(wParam);

      MainApp.doActionOnNewThreadIfNecessary(wmId);
		  break;

	  case WM_PAINT:

      hdc = BeginPaint(hWnd, &ps);
      MainApp.displayToWindow();
     	EndPaint(hWnd, &ps);

		  break;

    case WM_DRAWITEM: // processes the display of the STATIC

      break;

    case WM_LBUTTONDOWN:    
      {
        MainApp.interface_.mouseButton_ = true ;

        // Graps an order n° from user
        int action_nr   = MainApp.detectUserAction();
        MainApp.doActionOnNewThreadIfNecessary(action_nr);
      }
      
      break;

    case WM_LBUTTONDBLCLK:    
      {
        MainApp.interface_.mouseButton_ = true ;
        int xpos = GET_X_LPARAM(lParam);
        int ypos = GET_Y_LPARAM(lParam);

        MainApp.openLoadDialogWithDoubleClick(xpos, ypos);
      }
      
      break;

    case WM_LBUTTONUP:
      {
        MainApp.interface_.mouseButton_ = false ;

        // Graps an order n° from user
        int action_nr   = MainApp.detectUserAction();
        MainApp.doActionOnNewThreadIfNecessary(action_nr);
      }
      break;
    case WM_RBUTTONDOWN:    
      {
        MainApp.interface_.mouseButtonR_ = true ;

        // Graps an order n° from user
        int action_nr   = MainApp.detectUserAction();
        MainApp.doActionOnNewThreadIfNecessary(action_nr);
      }
      
      break;

    case WM_RBUTTONUP:
      {
        MainApp.interface_.mouseButtonR_ = false ;

        // Graps an order n° from user
        int action_nr   = MainApp.detectUserAction();
        MainApp.doActionOnNewThreadIfNecessary(action_nr);
      }
      break;

    case WM_MOUSEWHEEL:
      {
        MainApp.interface_.mouseWheel_ = GET_WHEEL_DELTA_WPARAM(wParam) ;

        int action_nr   = MainApp.detectUserAction();
        MainApp.doActionOnNewThreadIfNecessary(action_nr);
      }
      break;

    case WM_MOUSEMOVE: 
      {
        MainApp.interface_.mouseX_ = LOWORD(lParam);
        MainApp.interface_.mouseY_ = HIWORD(lParam);

        int action_nr   = MainApp.detectUserAction();
        MainApp.doActionOnNewThreadIfNecessary(action_nr);
        MainApp.setTrackMouseLeave();
      }
      break;

    case WM_MOUSELEAVE: 
      {
        MainApp.interface_.mouseX_ = -1;
        MainApp.interface_.mouseY_ = -1;
        MainApp.interface_.mouseButton_ = 0;

        int action_nr   = MainApp.detectUserAction();
        MainApp.doActionOnNewThreadIfNecessary(action_nr);
        MainApp.resetTrackMouseLeave();
      }
      break;    

    case WM_MENUSELECT: 
      {
        // if a menu item is highlighted by the mouse, 
        // send a signal (negative value) to stop dragging the 3d display
        MainApp.interface_.mouseX_ = -1 ;

        int action_nr   = MainApp.detectUserAction();
        MainApp.doActionOnNewThreadIfNecessary(action_nr);      
      }
      break;
 
    case WM_SIZE:
    {
      int windowSizeX=LOWORD(lParam);
      int windowSizeY=HIWORD(lParam);

      MainApp.resizeMainWindow(windowSizeX, windowSizeY);
      // a MainApp.requestRedraw() is automatically called 
      // after this WM_SIZE message

      break;
    }

    case WM_TIMER:
      if(wParam == (unsigned int) MainApp.interface_.sleepIcon_.getSleepIconTimerId())
      {
        MainApp.interface_.sleepIcon_.tickSleepIcon();
        MainApp.requestRedraw();
      }
      if(wParam == (unsigned int) MainApp.getAnimationTimerId())
      {
        MainApp.animate();
        MainApp.requestRedraw();
      }      
      break;

    case WM_DROPFILES:
      {
        HDROP hDrop = (HDROP) wParam;
        MainApp.dropFiles(hDrop);

        break;
      }

    case WM_CLOSE:
      MainApp.doAction(ACTION_EXIT);

      break;

	  case WM_DESTROY:
		  PostQuitMessage(0);
		  break;

	  default:
		  return DefWindowProc(hWnd, message, wParam, lParam);
	  }
  }
  CATCH_BLOC("Error while managing windows");

  return 0;
}
