/*
 * DialogWindow.h
 *
 *  Created on: 06 feb. 2011
 *      Author: Hippolyte Mounier, Copyright 2011 All rights reserved
 */
#pragma once

#include <Windows.h>
#include "Image.h"

class DialogWindow
{
public:
  DialogWindow                    (HINSTANCE hinstance, HWND hwnd, int titleressource, 
                                   int windowclassressource, int iconressource, int acceleratorressource);
  virtual void launchDialog       (bool exclusive);
  void InitInstance               (HINSTANCE hInstance, HWND parenthwnd, int nCmdShow);
  ATOM registerClass              (HINSTANCE hInstance, int titleressource, 
                                   int windowclassressource, int iconressource);
  
  virtual void displayToWindow    ();
  virtual void resizeMainWindow   (int windowSizeX, int windowSizeY);
  virtual void requestRedraw      ();
  virtual void detectAction       ();
  virtual void doAction           (int action);

  volatile bool mouseButton_      ;
  volatile int  mouseX_, mouseY_  ;
  
  virtual void destroy            ();
  bool         isActive           () const;

protected:
  virtual int  getClientSizeX_    ();
  virtual int  getClientSizeY_    ();
  virtual int  getWindowSizeX_    ();
  virtual int  getWindowSizeY_    ();

  ////////////////////////////////////   PRIVATE VARIABLES
  Image                   interfaceImage_       ;
  HWND                    interfacehWnd_        ;
  HINSTANCE               interfacehInst_       ;
  HWND                    interfaceParenthwnd_  ;
  HWND                    paintZonehWnd_        ;
  int                     ressourceAccelerator_ ;

  static const int        MAX_LOADSTRING = 100  ;

  TCHAR szTitle_          [MAX_LOADSTRING];			// Le texte de la barre de titre
  TCHAR szWindowClass_    [MAX_LOADSTRING];			// le nom de la classe de fenêtre principale

  static bool isPosStored_;
  static int  savedSizeX_, savedSizeY_, savedX_, savedY_;
  bool        isActive_;
  bool        isModal_;
};
