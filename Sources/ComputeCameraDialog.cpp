/*
 * ComputeCameraDialog.cpp
 *
 *  Created on: 2011/01/08
  *      Author: Hippolyte Mounier, Copyright 2011 All rights reserved
 */

#include <Windows.h>
#include "RESOURCE.H"
#include "POVFinder.h"
#include <process.h>

INT_PTR CALLBACK callBackComputeCamera(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

static bool iscompute  = false;
static bool isThreadOn = false;

bool computeCameraDialog(HINSTANCE inst, HWND hwnd, POVFinder * finder)
{
  int isokcancel = DialogBoxParam(inst, MAKEINTRESOURCE(IDD_3DPOSITION_BOX), 
            hwnd,  callBackComputeCamera, LPARAM(finder) );

  iscompute = false;
  while(isThreadOn);

  if (isokcancel == IDOK )   return true;
  return false;
}

void POV2string(const POV& pov, char * text)
{
  FloatVector povparameters = pov.get();

  char txt[100]="";
  sprintf(txt, "position x : %.5f \n",   povparameters[0]);
  strcat(text, txt);
  sprintf(txt, "position y : %.5f \n",   povparameters[1]);
  strcat(text, txt);
  sprintf(txt, "position z : %.5f \n",   povparameters[2]);
  strcat(text, txt);
  sprintf(txt, "focal : %.5f \n",        povparameters[3]);
  strcat(text, txt);
  sprintf(txt, "zenith angle: %.5f \n",  povparameters[4]);
  strcat(text, txt);
  sprintf(txt, "azimuth angle: %.5f \n", povparameters[5]);
  strcat(text, txt);
  sprintf(txt, "roll angle : %.5f \n",   povparameters[6]);
  strcat(text, txt);
  sprintf(txt, "zScale : %.5f \n",       povparameters[7]);
  strcat(text, txt);
  sprintf(txt, "distort : %.5f \n",      povparameters[8]);
  strcat(text, txt);
}

static HWND        GlobalHandleWindow ;
static POVFinder * GlobalPovFinder    ;

void action(PVOID pvoid_UNUSED)
{
  isThreadOn = true;
  do
  {
    POV pov = GlobalPovFinder->findBestView();

    char text[2000]="";
    POV2string(pov, text);

    char txt[200];

    int sizex = GlobalPovFinder->getMapSizeX();
    int sizey = GlobalPovFinder->getMapSizeY();

    Vector3 cameraposL = pov.computeCameraPositionLeft (sizex, sizey);
    float camxL=cameraposL.x, camyL=cameraposL.y, camzL=cameraposL.z;
    sprintf_s(txt, 200, "camera Front X Y Z %.5f %.5f %.5f ;", camxL, camyL, camzL );
    strcat_s(text, 2000, txt);

    Vector3 cameraposR = pov.computeCameraPositionRight(sizex, sizey);
    float camxR=cameraposR.x, camyR=cameraposR.y, camzR=cameraposR.z;
    sprintf_s(txt, 200, "camera Right X Y Z %.5f %.5f %.5f ;", camxR, camyR, camzR );
    strcat_s(text, 2000, txt);

    sprintf_s(txt, 200, "score : %.5f \n", GlobalPovFinder->getCurrentBestScore() );
    strcat_s(text, 2000, txt);

    SetDlgItemText(GlobalHandleWindow, FOCAL_EDIT, text);
  }
  while (iscompute);
  isThreadOn = false;
}



// Gestionnaire de messages pour la boîte de dialogue À propos de.
INT_PTR CALLBACK callBackComputeCamera(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  static POVFinder * finder;

	switch (message)
	{
	case WM_INITDIALOG:
    {
      finder  = (POVFinder *) lParam ;
      POV pov = finder->getCurrentBestPOV();
     
      char text[2000]="";
      POV2string(pov, text);

      SetDlgItemText(hDlg, FOCAL_EDIT, text);
      return TRUE;
    }

	case WM_COMMAND:
    {
      switch (LOWORD(wParam))
      {
      case IDOK:
      case IDCANCEL:
		    { 
          if (isThreadOn)
            MESSAGE("Please stop the calculation first before exiting")
          else			    
            EndDialog(hDlg, LOWORD(wParam));

			    return (INT_PTR)TRUE;
		    }
      case POSITION_COMPUTE:
        {
          if (isThreadOn)
            MESSAGE("Calculation is already running")
          else
          {
            GlobalHandleWindow =hDlg;
            GlobalPovFinder = finder ;
            iscompute = true;
            _beginthread(action,0,NULL);
          }
 
			    return (INT_PTR)TRUE;
        }      
      case POSITION_STOP:
        {
          iscompute = false;

			    return (INT_PTR)TRUE;
        }
      }
		break;
    }
	}
	return (INT_PTR)FALSE;
}

