/*
 * CropEditor.h
 *
 *  Created on: 30 sept. 2009
 *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */

#pragma once

#include "stdafx.h"
#include "ButtonManager.h"
#include "Cropper.h"
#include "ImageFrame.h"
#include "MonoScreen.h"
#include "DialogWindow.h"

class CropEditor: public DialogWindow
{
public:
  CropEditor              (HINSTANCE hinstance, HWND hwnd);
  void setCropperAndImage (Cropper* cropper, Image& source);

  void displayToWindow    ();
  void resizeMainWindow   (int windowSizeX, int windowSizeY);
  void detectAction       ();
  void doAction           (int action);

  bool isOk               ();
  Cropper getCropper      ();
  void finalise           ();

  void setMode            (int mode);
  void setButtonLogic     ();

protected:
  /////////////////////   PRIVATE METHODS

  void renderSourceImages_();
  void initButtons_       ();
  int  detectHandle_      (float mousefloatx, float mousefloaty);
  int  detectHandle2_     (float mousefloatx, float mousefloaty);
  void dragHandle_        (int corner, float mousefloatx, float mousefloaty );
  void dragHandle2_       (int corner, float mousefloatx, float mousefloaty );
  int  switchHandle_      (int corner, float mousefloatx, float mousefloaty );

  /////////////////////   PRIVATE VARIABLES
  
  static const int        CURSOR_SNAP_DISTANCE = 2  ;

  float                   snapDistance_ ;
  bool                    isOk_         ;
  Cropper                 cropEdited_   ;
  Cropper                 cropBackup_   ;
  Cropper *               cropDisplayed_;
  ButtonManager           buttons_      ;
  PImage                  source_       ;
  MonoScreen              monoScreen_   ;
  ImageFrame              frame_        ;
};

