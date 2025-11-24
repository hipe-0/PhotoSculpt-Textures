/*
 * SelectionEditor.h
 *
 *  Created on: 30 sept. 2009
 *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */
#pragma once

#include "stdafx.h"
#include "ButtonManager.h"
#include "ImageFrame.h"
#include "MonoScreen.h"
#include "DialogWindow.h"

class SelectionEditor: public DialogWindow
{
public:
  SelectionEditor(HINSTANCE hInstance, HWND parenthwnd, const PImage alpha, const PImage left);
  void displayToWindow();
  void resizeMainWindow(int windowSizeX, int windowSizeY);
  void detectAction();
  bool isOk               ();
  void setButtonLogic     ();
  void doAction           (int action);
  void addPointToLasso    (float mousefloatx, float mousefloaty);
  void applyPaintWithLasso();
  void cancelLasso        ();
  PImage getAlpha         ();

protected:
  //////////////   PRIVATE METHODS
  void             renderSourceImages_();
  void             initButtons_  ();

  //////////////   PRIVATE VARIABLES
  MonoScreen       monoScreen_;
  bool             isOk_;
  PImage           alpha_, alphaSavedForUndo_, source_;
  cimg_library::CImgList<int> selectedPoints_, displayedPoints_;
  ButtonManager    buttons_;
  ImageFrame       frame_  ;
  int              color_  ; // ACTION_SELECT_WHITE or ACTION_SELECT_BLACK
  int              tool_   ; 
};
