/*
 * Edit.h
 *
 *  Created on: 30 sept. 2009
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */
#pragma once

#include "stdafx.h"
#include "PSApplication.h"
#include "ButtonManager.h"
#include "SourceScreens.h"
#include "DialogWindow.h"

class PinEditor: public DialogWindow
{
public:
  PinEditor(HINSTANCE hinstance, HWND parenthwnd, PSApplication* p);
  void displayToWindow();
  void resizeMainWindow(int windowSizeX, int windowSizeY);
  void detectAction();
  bool isOk();
  void finalise();
  void setMode(int mode);

private:
  ////////   PRIVATE METHODS

  void renderSourceImages();
  void drawConstraints();
  int  searchConstraintL(float x, float y, float withindist);
  int  searchConstraintR(float x, float y, float withindist);
  void dropConstraint();
  void initButtons();

  //////     PRIVATE VARIABLES

  PSApplication*  psinterface_;

  SourceScreens sources_;
  VectorMap map_;
  static const int CURSOR_SNAP_DISTANCE = 2;

  EditCoordinates editCoordinates_;

  float snapDistance_;

  bool isOk_;

  int editMode_;
  static const int BAR_SIZE_Y = 80;
  ButtonManager buttons_;
};

