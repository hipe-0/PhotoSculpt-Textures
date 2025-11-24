/*
 * Options.cpp
 *
 * Created on: 01 jan 2011
 *      Author: Hippolyte Mounier, Copyright 2011 All rights reserved
 */

#include "COMPILER_OPTIONS.h"
#include "RESOURCE.H"
#include "Options.h"
#include "ImageTools.h"
#include "ImageMath.h"
#include "MultiTasking.h"
#include "Limit.h"

const int Options::TRIANGLE_LIMITS[]= 
{50000, 100000, 200000, 400000, 800000, 1200000, 1500000, 2000000, 3000000, 4000000 };

Options::Options()
{
  nbTriangleDisplay_      = 400000; 
  nbTriangleIndex_        = 3     ;
  isNormalInvertedX_      = isNormalInvertedY_ = false;
  hue_                    = 212   ;
  saturation_             = 1.0f  ;
  jpgPercent_             = 95    ;
  luminosity_             = 255   ;
  blur_                   = 1.0f  ;
  isExport3DClosed_       = false ;
  isPerspectiveCorrected_ = true  ;
  isSoftwareMode_         = false ;
  nbThreadMax_            = MultiTasking::getNbCores() ;
  isSeamCorrectedDiffuse_ = false ;
  isHalfSpherePreview_    = false ;
  nbFlatModeSamples_      = FLAT_MAP_INIT_SIZE    ;
  interfaceOpacity_       = 100   ;
  isAutoLocalDetail_      = true  ;
}

Options GlobalOptions;

// note : not all options are saved
void OptionStore::saveToRegistry(const Options& options, Registry* r)
{
  r->saveDWord("UserPreferenceTriangleDisplayMax",   options.nbTriangleDisplay_);
  r->saveDWord("UserPreferenceTriangleDisplayIndex", options.nbTriangleIndex_  );
  r->saveDWord("UserPreferenceNormalInvertedX",      options.isNormalInvertedX_);
  r->saveDWord("UserPreferenceNormalInvertedY",      options.isNormalInvertedY_);
  r->saveDWord("UserPreferenceInterfaceHue",         options.hue_              );
  r->saveDWord("UserPreferenceInterfaceSaturation",  round(options.saturation_*100.0f));
  r->saveDWord("UserPreferenceJpgQuality",           options.jpgPercent_       );
  r->saveDWord("UserPreferenceBackgroundLuminosity", options.luminosity_       );
  r->saveDWord("UserPreferenceNbThreadMax",          options.nbThreadMax_      );
  r->saveDWord("UserPreferenceUserCorrectedDiffuse", options.isSeamCorrectedDiffuse_);
  r->saveDWord("UserPreferenceInterfaceOpacity",     options.interfaceOpacity_ );
}

void OptionStore::loadFromRegistry(Options& options, Registry* r)
{
  DWORD load = 0;
  if ( r->loadDWord("UserPreferenceTriangleDisplayMax"   ,load))  options.nbTriangleDisplay_ = load;
  if ( r->loadDWord("UserPreferenceTriangleDisplayIndex" ,load))  options.nbTriangleIndex_   = load;
  if ( r->loadDWord("UserPreferenceNormalInvertedX"      ,load))  options.isNormalInvertedX_ = load;
  if ( r->loadDWord("UserPreferenceNormalInvertedY"      ,load))  options.isNormalInvertedY_ = load;
  if ( r->loadDWord("UserPreferenceInterfaceHue"         ,load))  options.hue_               = load;
  if ( r->loadDWord("UserPreferenceInterfaceSaturation"  ,load))  options.saturation_        = float(load)/100.0f;
  if ( r->loadDWord("UserPreferenceJpgQuality"           ,load))  options.jpgPercent_        = load;
  if ( r->loadDWord("UserPreferenceBackgroundLuminosity" ,load))  options.luminosity_        = load;
  if ( r->loadDWord("UserPreferenceNbThreadMax"          ,load))  options.nbThreadMax_       = load;
  if ( r->loadDWord("UserPreferenceUserCorrectedDiffuse" ,load))  options.isSeamCorrectedDiffuse_ = load;
  if ( r->loadDWord("UserPreferenceInterfaceOpacity"     ,load))  options.interfaceOpacity_  = load;

  limit (options.nbThreadMax_, 1, 16);
}

static Options TemporaryOptions;
// Gestionnaire de messages pour la boîte de dialogue À propos de.
INT_PTR CALLBACK callBackOptions(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
    {
      HWND htb = GetDlgItem(hDlg, OPTION_TRIANGLE_MAX);
      SendMessage(htb, TBM_SETRANGE,FALSE, MAKELONG(0, Options::NB_TRIANGLE_LIMIT-1));
      SendMessage(htb, TBM_SETPOS,TRUE, TemporaryOptions.nbTriangleIndex_);
      int triangleval    = Options::TRIANGLE_LIMITS[TemporaryOptions.nbTriangleIndex_];
      char text[20]="";
      sprintf(text, "%dK triangles", triangleval/1000);
      SetDlgItemText(hDlg, OPTION_TRIANGLE_TXT, text);

      htb = GetDlgItem(hDlg, OPTION_INTERFACE_HUE);
      SendMessage(htb, TBM_SETRANGE,FALSE, MAKELONG(0, 360));
      SendMessage(htb, TBM_SETPOS,TRUE, TemporaryOptions.hue_);
      sprintf(text, "%d°", TemporaryOptions.hue_);
      SetDlgItemText(hDlg, OPTION_HUE_TXT, text);

      htb = GetDlgItem(hDlg, OPTION_INTERFACE_SAT);
      SendMessage(htb, TBM_SETRANGE,FALSE, MAKELONG(0, 100));
      SendMessage(htb, TBM_SETPOS,TRUE, int(TemporaryOptions.saturation_*100.0f));
      sprintf(text, "%d%%", int(TemporaryOptions.saturation_*100.0f));
      SetDlgItemText(hDlg, OPTION_SAT_TXT, text);

      htb = GetDlgItem(hDlg, OPTION_JPEG_PERCENT);
      SendMessage(htb, TBM_SETRANGE,FALSE, MAKELONG(0, 100));
      SendMessage(htb, TBM_SETPOS,TRUE, TemporaryOptions.jpgPercent_);
      sprintf(text, "%d%%", TemporaryOptions.jpgPercent_);
      SetDlgItemText(hDlg, OPTION_JPEG_TXT, text);

      htb = GetDlgItem(hDlg, OPTION_INTERFACE_LUMINOSITY);
      SendMessage(htb, TBM_SETRANGE,FALSE, MAKELONG(0, 255));
      SendMessage(htb, TBM_SETPOS,TRUE, TemporaryOptions.luminosity_);
      sprintf(text, "%d", TemporaryOptions.luminosity_);
      SetDlgItemText(hDlg, OPTION_LUM_TXT, text);

      htb = GetDlgItem(hDlg, OPTION_INTERFACE_OPACITY);
      SendMessage(htb, TBM_SETRANGE,FALSE, MAKELONG(0, 255));
      SendMessage(htb, TBM_SETPOS,TRUE, TemporaryOptions.interfaceOpacity_);
      sprintf(text, "%d", TemporaryOptions.interfaceOpacity_);
      SetDlgItemText(hDlg, OPTION_OPAC_TXT, text);

      if( TemporaryOptions.isNormalInvertedX_ ) 
        CheckDlgButton(hDlg, OPTION_INVERT_NORMAL_X,        BST_CHECKED);
      if( TemporaryOptions.isNormalInvertedY_ ) 
        CheckDlgButton(hDlg, OPTION_INVERT_NORMAL_Y,        BST_CHECKED);

      // init the "number of thread max" field
      CHAR st[20]="";
      sprintf_s(st, 20, "%d", TemporaryOptions.nbThreadMax_);
      SetDlgItemText(hDlg, CORES_EDIT, st);

      return TRUE;
    }

   case WM_CTLCOLORSTATIC:
    if(GetDlgItem(hDlg, OPTION_HUE_TXT) == (HWND)lParam)
    {
        HDC hDC = (HDC)wParam;

        // read the hue slider
        HWND    htb = GetDlgItem(hDlg, OPTION_INTERFACE_HUE);
        float64 hue = float64(SendMessage(htb, TBM_GETPOS, 0, 0)) / 360.0F;

        // read the saturation slider
        htb         = GetDlgItem(hDlg, OPTION_INTERFACE_SAT);
        float64 sat = float64(SendMessage(htb, TBM_GETPOS, 0, 0))/100.0F;

        uint8 r,g,b;
        Color_HlsToRgb(hue, 0.5F, sat, &r,&g,&b);
        COLORREF c = RGB( r,g,b );
        SetBkColor(hDC, RGB(255,255,255) );
        SetTextColor(hDC, c);
        //SetBkMode(hDC, TRANSPARENT);
        return (INT_PTR)CreateSolidBrush(GetSysColor(COLOR_BTNFACE));			
    }
    if(GetDlgItem(hDlg, OPTION_LUM_TXT) == (HWND)lParam)
    {
        HDC hDC = (HDC)wParam;

        // read the luminosity slider
        HWND htb = GetDlgItem(hDlg, OPTION_INTERFACE_LUMINOSITY);
        int gray = SendMessage(htb, TBM_GETPOS, 0, 0);

        SetBkColor(hDC, RGB(gray,gray,gray) );
        //SetTextColor(hDC, c);
        //SetBkMode(hDC, TRANSPARENT);
        return (INT_PTR)CreateSolidBrush(GetSysColor(COLOR_BTNFACE));			
    }    break;

  case WM_HSCROLL:
    {
      // read the triangle slider
      HWND htb = GetDlgItem(hDlg, OPTION_TRIANGLE_MAX);
      int slider = SendMessage(htb, TBM_GETPOS, 0, 0);
      int triangleval    = Options::TRIANGLE_LIMITS[slider];
      char text[20]="";
      sprintf(text, "%dK triangles", triangleval/1000);
      SetDlgItemText(hDlg, OPTION_TRIANGLE_TXT, text);


      // read the hue slider
      htb = GetDlgItem(hDlg, OPTION_INTERFACE_HUE);
      slider = SendMessage(htb, TBM_GETPOS, 0, 0);
      *text=0;
      sprintf(text, "%d°", slider);
      SetDlgItemText(hDlg, OPTION_HUE_TXT, text);


      // read the saturation slider
      htb = GetDlgItem(hDlg, OPTION_INTERFACE_SAT);
      slider = SendMessage(htb, TBM_GETPOS, 0, 0);
      *text=0;
      sprintf(text, "%d%%", slider);
      SetDlgItemText(hDlg, OPTION_SAT_TXT, text);


      // read the jpg slider
      htb = GetDlgItem(hDlg, OPTION_JPEG_PERCENT);
      slider = SendMessage(htb, TBM_GETPOS, 0, 0);
      *text=0;
      sprintf(text, "%d%%", slider);
      SetDlgItemText(hDlg, OPTION_JPEG_TXT, text);

      // read the Luminosity slider
      htb = GetDlgItem(hDlg, OPTION_INTERFACE_LUMINOSITY);
      slider = SendMessage(htb, TBM_GETPOS, 0, 0);
      *text=0;
      sprintf(text, "%d", slider);
      SetDlgItemText(hDlg, OPTION_LUM_TXT, text);

      // read the Opacity slider
      htb = GetDlgItem(hDlg, OPTION_INTERFACE_OPACITY);
      slider = SendMessage(htb, TBM_GETPOS, 0, 0);
      *text=0;
      sprintf(text, "%d", slider);
      SetDlgItemText(hDlg, OPTION_OPAC_TXT, text);

      return TRUE;
    }

	case WM_COMMAND:
		if ( LOWORD(wParam) == IDOK )
		{ // OK was pressed

      // Read the radio and checkbox buttons
      TemporaryOptions.isNormalInvertedX_ =
        (IsDlgButtonChecked( hDlg, OPTION_INVERT_NORMAL_X ) == BST_CHECKED);
      TemporaryOptions.isNormalInvertedY_ =
        (IsDlgButtonChecked( hDlg, OPTION_INVERT_NORMAL_Y ) == BST_CHECKED);

      // read the triangle slider
      HWND htb                            = GetDlgItem(hDlg, OPTION_TRIANGLE_MAX);
      TemporaryOptions.nbTriangleIndex_   = SendMessage(htb, TBM_GETPOS, 0, 0);
      TemporaryOptions.nbTriangleDisplay_ = Options::TRIANGLE_LIMITS[TemporaryOptions.nbTriangleIndex_];

      // read the hue slider
      htb                                 = GetDlgItem (hDlg, OPTION_INTERFACE_HUE);
      TemporaryOptions.hue_               = SendMessage(htb, TBM_GETPOS, 0, 0);

      // read the saturation slider
      htb                                 = GetDlgItem(hDlg, OPTION_INTERFACE_SAT);
      TemporaryOptions.saturation_        = float(SendMessage(htb, TBM_GETPOS, 0, 0))/100.0f;

      // read the jpg slider
      htb                                 = GetDlgItem (hDlg, OPTION_JPEG_PERCENT);
      TemporaryOptions.jpgPercent_        = SendMessage(htb, TBM_GETPOS, 0, 0);
      
      // read the luminosity slider
      htb                                 = GetDlgItem (hDlg, OPTION_INTERFACE_LUMINOSITY);
      TemporaryOptions.luminosity_        = SendMessage(htb, TBM_GETPOS, 0, 0);

      // read the luminosity slider
      htb                                 = GetDlgItem (hDlg, OPTION_INTERFACE_OPACITY);
      TemporaryOptions.interfaceOpacity_  = SendMessage(htb, TBM_GETPOS, 0, 0);

      CHAR st[20];
      GetDlgItemText(hDlg, CORES_EDIT, st, 20);

      int value = 0;
      int numberofitemsscanned = sscanf_s(st, "%d", &value, 20);

      if (numberofitemsscanned == 1 && value >= 1 && value <= 10 )
      {
        TemporaryOptions.nbThreadMax_ = value;
 			  EndDialog(hDlg, LOWORD(wParam));
      }
      else
      {
        MessageBox(hDlg, "Wrong argument, a value between 1 and 10 is expected", "Press ok to retry", MB_OK);
			  return (INT_PTR)TRUE;
      }

			return (INT_PTR)TRUE;
		}
    if ( LOWORD(wParam) == IDCANCEL )
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

bool OptionStore::launchDialog(Registry * registry, HINSTANCE inst, HWND hwnd)
{
  TemporaryOptions = GlobalOptions;

  int isokcancel = DialogBox(inst, MAKEINTRESOURCE(IDD_OPTION_BOX), 
            hwnd,  callBackOptions);

  if (isokcancel == IDOK ) // change options
  {
    GlobalOptions=TemporaryOptions;
   
    OptionStore::saveToRegistry(GlobalOptions, registry);
    return true;
  }

  return false;
}
