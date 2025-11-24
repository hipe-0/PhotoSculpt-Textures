/*
 * GraphicUserInterface.h
 *
 *  Created on: 15 déc. 2010
 *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */

#pragma once

#include <windows.h>
#include <shellapi.h>
#include "ManualEditInterface.h"
#include "View3D.h"
#include "VectorMap.h"
#include "ButtonManager.h"
#include "DirectXControl.h"
#include "Result.h"
#include "Registry.h"
#include "EditCoordinates.h"
#include "Obfuscate.h"
#include "ProgressDialog.h"
#include "ToolTip.h"
#include "MouseTracker.h"
#include "ImageFrame.h"
#include "Options.h"
#include "ThreadId.h"
#include "Includes.h"
#include "SleepIcon.h"
#include "Sources.h"

class UserInfo
{
public:
  bool isResult        ;
  int  totalused       ;
  bool isSculptModel   ;
  bool isSculpting     ;
  bool isAutomaticMode ;
  bool isProtected     ;
  bool isLastStep      ;
  int  resultSizeX     ;
  int  resultSizeY     ;
  int  resultSubdivition ;
  int  step            ;
  int  substep         ;
  int  maxlevel        ;
  float timeToEnd      ;
  int  nbTriangleDisplayed ;
};

class GraphicUserInterface
{
public:
  /////////////////////
  // MEMBER FUNCTIONS
  /////////////////////
  GraphicUserInterface();

  void createMainWindow(HINSTANCE hInstance, int nCmdShow, Registry& registry);

  void initSculptIcons    ();
  void initSculptMiniIcons();
  void initPopup3d        ();
  void initPopupLoadLeft  ();
  void initPopupLoadRight ();
  void initPopupLoadAlpha ();
  void initPopupSave      ();
  void initSculptSolo     ();
  void initTab            ();
  void initButtonBarsIcons();
  void createTransparentMenuIcons();

  int  getClientSizeX     ();
  int  getClientSizeY     ();
  int  getContentSizeY    ();
  int  getBottomBarSizeY  ();
  int  getBottomBarPosY   ();

  ATOM registerMainWindowClass(HINSTANCE hInstance);
  void changeTitle        (char * new_string)      ;

  int  detectPopup        (int currentmode)        ;
  int  detectUserAction   (Result& result)         ;
   
  void activate           (int action)             ;
  void desactivate        (int action)             ;
  void drawText              (char* text, int positionx, int positiony);
  void drawTextOnOpaqueWhite (char* text, int positionx, int positiony);

  void renderBottomBar    (const UserInfo& info);
  void renderContent2D    (int mode, const Image& resultimage);
  void renderSourceImages (Source& sources);
  void displayToWindow    (const UserInfo& info, Source& sources, int mode, Result & result);

  // VARIABLES
  Image           image_           ;
  Image           alpha_           ;
  HWND            windowsHandle_   ;
  HINSTANCE       instanceHandle_  ;
  HMENU           menuHandle_      ;

  volatile bool   mouseButton_     ;
  volatile bool   mouseButtonR_    ;
  volatile int    mouseX_, mouseY_ ;
  volatile int    mouseWheel_      ;
  
  DirectXControl  directX_         ;

  Image           leftPhotoicon_, rightPhotoicon_                            ;

  ButtonManager   frontPage_, bottomBar_, bottomMiniBar_, contentZone_       ;
  ButtonManager   popupSave_, popup3d_, topTabBar_, bottomSolo_, bottomZone_ ;
  ButtonManager   popupLoadL_, popupLoadR_, popupLoadAlpha_                  ;
  ButtonManager   manualInterfacebuttons_                                    ;
    
  static const unsigned char   textColor_[3];

  static const int MAX_LOADSTRING = 100 ;
  TCHAR           szTitle_         [MAX_PATH];		  // Le texte de la barre de titre
  TCHAR           szWindowClass_   [MAX_LOADSTRING]; // le nom de la classe de fenêtre principale

  SleepIcon       sleepIcon_       ;
  ToolTip         toolTip_         ;

  ProgressionBar  progressionBar_  ;
  bool            isSculptBarMinimised_;
  ImageFrame      frame_           ;

  bool            isAeroGlass_     ;
};
