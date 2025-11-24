/*
 * PSInterfaceExtract.cpp
 *
 *  Created on: 26 sept 2009
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */
#define _WIN32_WINNT 0x0500

#include "PSApplication.h"
#include "Includes.h"
#include "PerspectiveAnalyser.h"
#include "ExposureAnalyser.h"
#include "STRINGS.h"

static char *       GlobalCurrentText;
static const char * GlobalRessourceImage;
static HINSTANCE    GlobalInstance;

// Gestionnaire de messages pour la boîte de dialogue À propos de.
BOOL APIENTRY callBackWarning(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  DEBUG(cllBcWan);
  static HBITMAP hBitmap;

	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
    {
      HWND hwndTxt      = GetDlgItem(hDlg, IDC_WARNING_TXT);
      HWND hwndImg      = GetDlgItem(hDlg, IDC_WARNING_IMG);

      hBitmap = (HBITMAP) LoadImage(GlobalInstance, GlobalRessourceImage, 
        IMAGE_BITMAP, 0, 0, 
        LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_VGACOLOR);

      if (hBitmap == NULL)
        THROW_INTERNAL_ERROR( "Could not retrieve Bitmap (in callBackWarning())" );

      SetWindowText(hwndTxt, GlobalCurrentText);
      SendMessage(hwndImg, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);

  		return TRUE;
    }
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
      EndDialog(hDlg, LOWORD(wParam));
      DeleteObject(hBitmap);

			return TRUE;
		}
		break;
	}
	return FALSE;
}

void PSApplication::warning(char * warningtxt, const char * warningressource)
{
  DEBUG(wnig);
  GlobalCurrentText    = warningtxt;
  GlobalRessourceImage = warningressource;
  GlobalInstance       = interface_.instanceHandle_;

  HWND hdlg = CreateDialog(interface_.instanceHandle_, MAKEINTRESOURCE(IDD_WARNINGBOX), 
              interface_.windowsHandle_,  (DLGPROC)callBackWarning);
}

void PSApplication::analyseAndWarnUser()
{
  DEBUG(alyendaUer);
  if ( sculpt_.computeStep() == 4  )
  {
    PerspectiveAnalyser p;
    p.init(sculpt_.getMap());

    float percent = sculpt_.getGoodPixelPercent();
    if ( percent < 50.0f )              warning( STR(NO_MATCH)    , WARNING);
    else
    {
      if ( p.isTooOffCenter      () ) warning( STR(OFF_CENTER)  , WARNING_IMAGE_11);
      if ( p.isTooZoomed         () ) warning( STR(TOO_ZOOMED)  , WARNING_IMAGE_13);
      if ( p.isTooMuchAngle      () ) warning( STR(TOO_ANGLE)   , WARNING);

      if ( p.isLeftRightInverted () ) warning( STR(L_R_INVERT)  , WARNING_IMAGE_8);
      else if ( p.isRotate90     () ) warning( STR(ROTATE_90)   , WARNING_IMAGE_1);
      else 
      {
        if ( p.isNotEnoughAngle  () ) warning( STR(TOO_SIMILAR) , WARNING_IMAGE_12);
        if ( p.isCameraNotLevel  () ) warning( STR(NOT_LEVEL)   , WARNING_IMAGE_14);
        if ( p.isTooStrongAngle  () ) warning( STR(BAD_HORIZON) , WARNING_IMAGE_2);
      }
    }


    ExposureAnalyser e;
    e.init(sculpt_.computeLeftImage(), sculpt_.computeRightImage());

    if ( e.isTooMuchContrast  () ) warning( STR(HICONTRAST)  , WARNING_IMAGE_7);
    else if ( e.isTooDark     () ) warning( STR(DARK)        , WARNING_IMAGE_5);
    else if ( e.isTooLight    () ) warning( STR(LIGHT)       , WARNING_IMAGE_9);
    
    if ( e.isTooDifferent     () ) warning( STR(DIFFERENT)   , WARNING_IMAGE_6);
    if ( e.isTooShiny         () ) warning( STR(SHINY)       , WARNING);

    if ( e.isLeftBlurred      () ) warning( STR(LEFTBLUR)    , WARNING_IMAGE_3);
    if ( e.isRightBlurred     () ) warning( STR(RIGHTBLUR)   , WARNING_IMAGE_4);

    int unused = 1;
  }
}

