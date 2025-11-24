/*
 * PSInterfaceSaveMultiple.cpp
 *
 *  Created on: 10 12 2009
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */

#include "RESOURCE.H"
#include "PSApplication.h"
#include "MultiQuery.h"
#include "File.h"

static MultiQuery query ;

MultiQuery::MultiQuery()
{
  isStl=false; isObj=false; isPly = false; isSculpt=true; 
  isSaveModel=true; isSaveMap=true; isSize2n=false;

  // clear all boolean tables
  for (int i=0; i<MultiQuery::NB_MAP_TYPES ; i++)
    isListMapType  [i] = false;

  nbSizes = 1;
  for (int i=0; i<nbSizes ; i++)
  {
    isListSizeModel[i] = false;
    isListSizeMap  [i] = false;
  }
}

char MultiQuery::listMapType[NB_MAP_TYPES][TEXT_LEN] = {
  "Diffuse map","Displacement map", "Normal map", "Occlusion map", "Specular map", 
  "Alpha map", "Bump map", "Limited displ.", "software3D" };

// Gestionnaire de messages pour la boîte de dialogue À propos de.
INT_PTR CALLBACK callBackSaveMultiple(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	DEBUG(calSaM);
  
  UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
     {
      HWND hwndList      = GetDlgItem(hDlg, MULTI_LIST_MAP_TYPE);
      HWND hwndListModel = GetDlgItem(hDlg, MULTI_LIST_SIZE_MODEL);
      HWND hwndListMap   = GetDlgItem(hDlg, MULTI_LIST_SIZE_MAP);

      for (int i=0; i<MultiQuery::NB_MAP_TYPES ; i++)
        SendMessage(hwndList, LB_ADDSTRING, 0, (LONG)query.listMapType[i]);

      SendMessage(hwndList, LB_SETSEL, TRUE, -1); // True = "select",  -1 = "all"

      for (int i=0; i<query.nbSizes ; i++)
      {
        SendMessage(hwndListModel, LB_ADDSTRING, 0, (LONG)query.listSize[i]);
        SendMessage(hwndListMap,   LB_ADDSTRING, 0, (LONG)query.listSize[i]);
      }
      SendMessage(hwndListModel, LB_SETSEL, TRUE, 0);
      SendMessage(hwndListMap,   LB_SETSEL, TRUE, 0);

      if( query.isStl )       CheckDlgButton(hDlg, MULTI_STL,        BST_CHECKED);
      if( query.isObj )       CheckDlgButton(hDlg, MULTI_OBJ,        BST_CHECKED);
      if( query.isPly )       CheckDlgButton(hDlg, MULTI_PLY,        BST_CHECKED);
      if( query.isSculpt )    CheckDlgButton(hDlg, MULTI_SCULPT,     BST_CHECKED);
      if( query.isSaveModel ) CheckDlgButton(hDlg, MULTI_SAVE_MODEL, BST_CHECKED);
      if( query.isSaveMap )   CheckDlgButton(hDlg, MULTI_SAVE_MAP,   BST_CHECKED);
      if( query.isSize2n )    CheckDlgButton(hDlg, MULTI_2N_SIZE,    BST_CHECKED);

      return TRUE;
     }

	case WM_COMMAND:
		if ( LOWORD(wParam) == IDOK )
		{ // OK was pressed

      // Read the radio and checkbox buttons
      query.isStl      =(IsDlgButtonChecked( hDlg, MULTI_STL        ) == BST_CHECKED);
      query.isObj      =(IsDlgButtonChecked( hDlg, MULTI_OBJ        ) == BST_CHECKED);
      query.isPly      =(IsDlgButtonChecked( hDlg, MULTI_PLY        ) == BST_CHECKED);
      query.isSculpt   =(IsDlgButtonChecked( hDlg, MULTI_SCULPT     ) == BST_CHECKED);
      query.isSaveModel=(IsDlgButtonChecked( hDlg, MULTI_SAVE_MODEL ) == BST_CHECKED);
      query.isSaveMap  =(IsDlgButtonChecked( hDlg, MULTI_SAVE_MAP   ) == BST_CHECKED);
      query.isSize2n   =(IsDlgButtonChecked( hDlg, MULTI_2N_SIZE    ) == BST_CHECKED);

      HWND hwndList      = GetDlgItem(hDlg, MULTI_LIST_MAP_TYPE);
      HWND hwndListModel = GetDlgItem(hDlg, MULTI_LIST_SIZE_MODEL);
      HWND hwndListMap   = GetDlgItem(hDlg, MULTI_LIST_SIZE_MAP);

      // Read the multiselection lists
      for (int i=0; i<MultiQuery::NB_MAP_TYPES ; i++)
        query.isListMapType[i] = (SendMessage(hwndList, LB_GETSEL, i, 0) > 0);

      for (int i=0; i<query.nbSizes ; i++)
      {
        query.isListSizeModel[i] = (SendMessage(hwndListModel, LB_GETSEL, i, 0) > 0);
        query.isListSizeMap  [i] = (SendMessage(hwndListMap,   LB_GETSEL, i, 0) > 0);
      }

			EndDialog(hDlg, LOWORD(wParam));
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

void PSApplication::actionSaveMultiple()
{
  DEBUG(acSavMule);
  
  if (!result_.isResult())
    THROW_MESSAGE_TO_USER(NOTHING_SAVE);

  int sizex=result_.getModelSizeX(), sizey=result_.getModelSizeY();

  query.nbSizes = 0;

  // fills the lists that contains the image sizes and sizes in 2n mode
  for (int i=0; i<MultiQuery::NB_SIZES_MAX; i++)
  {
    char txt[MultiQuery::TEXT_LEN]="";
    sprintf(txt, "%dx%d", sizex, sizey);

    strcpy(query.listSize[i], txt);
    strcpy(query.listSize2n[i], txt);
    sizex /= 2;
    sizey /= 2;
    query.nbSizes ++;

    if (sizex<=0 || sizey<=0) break;
  }

  int isokcancel = DialogBox(interface_.instanceHandle_, MAKEINTRESOURCE(IDD_SAVE_MULTIPLE_BOX), 
            interface_.windowsHandle_,  callBackSaveMultiple);

  if (isokcancel == IDOK )
    saveAsPerMultiQueryDialog(query);
  
}

void PSApplication::saveAsPerMultiQueryDialog(const MultiQuery& query)
{
  DEBUG(ssPerMiQuialog);

  char filename[MAX_PATH] = {0};

  bool isSuccess = saveAsFileDialog(filename,interface_.windowsHandle_,
                   "Png image format\0*.png\0Jpg image format\0*.jpg\0Bmp image format\0*.bmp\0Tiff image format (8bit only)\0*.tif\0All\0*.*\0\0",
                   "*.png");

  if (isSuccess)
  {
    requestRedraw();

    ProgressDialog dialog;

    saveAsPerMultiQuery(query, filename);
  }
}

void PSApplication::saveAsPerMultiQuery(const MultiQuery& query, const char * filename)
{
  DEBUG(sasPrMltiQuy);
  char extension[10] = "";

  bool isextension = extractExtension(extension, 10, filename); // returns .jpg or .bmp
  if (!isextension)
    strcpy_s(extension,10,".png");

  int subdiv=1;

  for (int i=0; i<query.nbSizes ; i++)
  {
    char lodnb[10] = "";

	  // the first level (i=0, not subdivided) has no exention.
	  if (i!=0)
	    sprintf(lodnb, "-%d", i);

    char newimagename[MAX_PATH]="", newmodelname[MAX_PATH]="";
    strcpy(newimagename, filename);
    removeExtension(newimagename);
    strcat(newimagename,lodnb);
    strcpy(newmodelname, newimagename);

    if (query.isSaveModel && query.isListSizeModel[i])
    {
      if (query.isObj)
      {
        ProgressDialog::setText(STR(PROGRESS_SAVING1));
        strcat(newmodelname, "Model.obj");
        saveAsObjFile(newmodelname, subdiv, 1);
      }
      else if (query.isPly)
      {
        ProgressDialog::setText(STR(PROGRESS_SAVING2));
        strcat(newmodelname, "Model.ply");
        saveAsPlyFile(newmodelname, subdiv, 1);
      }
      else if (query.isStl)
      {
        ProgressDialog::setText(STR(PROGRESS_SAVING3));
        strcat(newmodelname, "Model.stl");
        saveAsStlFile(newmodelname, subdiv);
      }
      else if (query.isSculpt)
      {
        ProgressDialog::setText(STR(PROGRESS_SAVING4));
        strcat(newmodelname, "Model.sculpt");
        saveAsSculptFile(newmodelname, subdiv);
      }
    }

    if (query.isSaveMap && query.isListSizeMap[i])
    {
      strcat(newimagename, extension); // adds ".jpg"

      for (int j=0; j<MultiQuery::NB_MAP_TYPES ; j++)
        if (query.isListMapType[j])
        // save texture map with this size
        {
          const int codes[MultiQuery::NB_MAP_TYPES] = { 
            ACTION_MODE_DIFFUSE,           ACTION_MODE_DISPLACE,
            ACTION_MODE_NORMAL,            ACTION_MODE_AMBOCC, 
            ACTION_MODE_SPECULAR,          ACTION_MODE_ALPHA, 
            ACTION_MODE_BUMP,              ACTION_MODE_LIMDISP, 
            ACTION_MODE_OLD_3D };
          
          char* text[MultiQuery::NB_MAP_TYPES] = {
            STR(PROGRESS_SAVING5), STR(PROGRESS_SAVING6),
            STR(PROGRESS_SAVING7), STR(PROGRESS_SAVING8),
            STR(PROGRESS_SAVING9), STR(PROGRESS_SAVING10), 
            STR(PROGRESS_SAVING11),STR(PROGRESS_SAVING12), 
            STR(PROGRESS_SAVING13)};

          ProgressDialog::setText(text[j]);

          saveOneImageExt(newimagename, codes[j], subdiv);
        }
    }
    subdiv *= 2 ;
  }
}

