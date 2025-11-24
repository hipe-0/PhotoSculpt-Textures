/*
 * ProgressDialog.cpp
 *
 *  Created on: 02 12 2009
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */

#pragma once

#include <windows.h>

class ProgressDialog
{
public:
  ProgressDialog();
  ProgressDialog(bool isshow); // true to show, false not to show
  ProgressDialog(char* texttodisplay );
  ~ProgressDialog();

  static void init                 (HINSTANCE handleinstance, HWND handleparentwindow);
  void        show                 ();
  void        hide                 ();
  bool        isDisplaying         ();
  static void setText              (char * text);

public:
  static HWND       handleText_            ;
 
private:
  static void       registerDialogClass_ ();
  void              createDialogBox_     ();

  static HINSTANCE  handleinstance_        ;
  static HWND       handleParentWindow_    ;
  static HWND       handleNewWindow_       ;
  static int        numberOfCalls_         ;
  bool              isThisInstanceShowing_ ;
};

#define PROGRESS(info) ProgressDialog progress(STR(PROGRESS_##info));