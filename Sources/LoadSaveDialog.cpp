/*
 * LoadSaveDialog.cpp
 *
 *  Created on: 14 mai 2009
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */

#include <windows.h>
#include <fstream>
#include "PSApplication.h"
#include "View3D.h"
#include "ExportObj.h"
#include "File.h"


void PSApplication::savePath(const char* filename)
  {
    // extract path from result
    char path[_MAX_PATH] = {0};
    strcpy(path, filename);

    changePathToBackslash(path);

    // search for a backslash in the string
    char * lastslash= strrchr(path,'\\');

    // was a backslash found?
    if (lastslash != NULL)
    {
      // change the string by adding a 0 character instead of the backslash
      // = cut the string right at this position
      *(lastslash) = 0;

      registry_.saveString("last open directory",path);
    }
  }

bool PSApplication::loadLastPath(char* path)
{
  DEBUG(loadLas);
  bool isdirectory = registry_.loadString("last open directory",path, MAX_PATH);

  // if opening a directory for the first time, (first use of photosculpt)
  if (!isdirectory)
    // then open the sample directory
    return loadSamplePath(path);

  // true : last open directory was found
  return true;
}

bool PSApplication::loadSamplePath(char* path)
{
  DEBUG(loadSam);
  bool isreadingregistryok = registryAll_.loadString("samples directory",path, MAX_PATH);
  return isreadingregistryok;
}


bool PSApplication::openFileDialog(char* result,char* result2, HWND& hwnd, char* title, char* filter)
{
  char szFile[_MAX_PATH] = {0};

  char path  [MAX_PATH] = {0};
  bool isreadingregistryok = loadLastPath(path);

  OPENFILENAME ofn;       // common dialog box structure

  // Initialize OPENFILENAME
  ZeroMemory(&ofn, sizeof(ofn));
  ofn.lStructSize = sizeof(ofn);
  ofn.hInstance   = NULL; // new
  ofn.hwndOwner   = hwnd; //hwnd;
  ofn.lpstrFile   = szFile;

  // Set lpstrFile[0] to '\0' so that GetOpenFileName does not
  // use the contents of szFile to initialize itself.
  //
  ofn.lpstrFile       = szFile; //= '\0';
  ofn.nMaxFile        = sizeof(szFile);
  ofn.lpstrFilter     = filter;
  ofn.nFilterIndex    = 1;
  ofn.lpstrTitle      = title;
  ofn.lpstrInitialDir = NULL;
  if (isreadingregistryok)
    ofn.lpstrInitialDir = LPCSTR(&path);

  ofn.Flags = OFN_PATHMUSTEXIST    | OFN_FILEMUSTEXIST   |
              OFN_ALLOWMULTISELECT | OFN_EXPLORER;

  // Display the Open dialog box.
  if (GetOpenFileName(&ofn)==TRUE)
  {

    if (strlen(szFile)>0)
    { 
      // converts the file to unix
      //backslash2Slash(szFile);

       // check if the result is multiple;
      char* nextString=(szFile+strlen(szFile)+1);

      if (*nextString==0) // the second string is empty = single string
      {
        // fills the 1st string with result
        strcpy(result,szFile);

        // clears the 2nd string
        *result2=0;
      }
      else // that's a multi select = more than 1 file to load
      {
        char* nextString2=(nextString+strlen(nextString)+1);

        // set results to have the same path
        strcpy(result, szFile);
        strcpy(result2,szFile);

        // add slashes
        strcat(result,"/");
        strcat(result2,"/");

		    // checks witch one of the 2 file names is the highest one
		    bool isSecondGreaterThanFirst=(strcmp(nextString,nextString2)<0);
		    if (isSecondGreaterThanFirst)
		    {
          // add the final file names with full path
          strcat(result,nextString);
          strcat(result2,nextString2);
		    }
		    else
		    {
          // add the final file names with full path
          strcat(result,nextString2);
          strcat(result2,nextString);
		    }
      }

      //backslash2Slash(result);
      //backslash2Slash(result2);

      savePath(result);

      return true;
    }
    return false;
  }

  return false;
}

bool PSApplication::openFileDialog(char* result, HWND& hwnd, char* title, char* filter)
{
  char szFile[_MAX_PATH] = {0};
  char path  [MAX_PATH] = {0};
  bool isreadingregistryok = loadLastPath(path);

  OPENFILENAME ofn;       // common dialog box structure

  // Initialize OPENFILENAME
  ZeroMemory(&ofn, sizeof(ofn));
  ofn.lStructSize = sizeof(ofn);
  ofn.hInstance   = NULL; // new
  ofn.hwndOwner   = hwnd; //hwnd;
  ofn.lpstrFile   = szFile;

  // Set lpstrFile[0] to '\0' so that GetOpenFileName does not
  // use the contents of szFile to initialize itself.
  //
  ofn.lpstrFile       = szFile; //= '\0';
  ofn.nMaxFile        = sizeof(szFile);
  ofn.lpstrFilter     = filter;
  ofn.nFilterIndex    = 1;
  ofn.lpstrTitle      = title;
  ofn.lpstrInitialDir = NULL;
  if (isreadingregistryok)
    ofn.lpstrInitialDir = LPCSTR(&path);

  ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER;

  // Display the Open dialog box.
  if (GetOpenFileName(&ofn)==TRUE)
    if (strlen(szFile)>0)
    { 
      // converts the file to unix
      //backslash2Slash(szFile);
      // fills the 1st string with result
      strcpy(result,szFile);

      savePath(result);

      return true;
    }

  return false;
}

bool PSApplication::saveAsFileDialog(char* result, HWND& hwnd,char* txt_filter, char* txt_ext)
{
  char szFile[_MAX_PATH] = {0};

  char path  [MAX_PATH] = {0};
  bool isreadingregistryok = loadLastPath(path);

  OPENFILENAME ofn;       // common dialog box structure

  // Initialize OPENFILENAME
  ZeroMemory(&ofn, sizeof(ofn));
  ofn.lStructSize = sizeof(ofn);
  ofn.hInstance  = NULL; // new
  ofn.hwndOwner = hwnd; //hwnd;
  ofn.lpstrFile = szFile;
  //
  // Set lpstrFile[0] to '\0' so that GetOpenFileName does not
  // use the contents of szFile to initialize itself.
  //
  ofn.lpstrFile=szFile; //= '\0';
  ofn.nMaxFile = sizeof(szFile);
  ofn.lpstrFilter = txt_filter;
  ofn.lpstrDefExt = txt_ext;
  ofn.nFilterIndex = 1;
  ofn.lpstrFileTitle = "Save a file";

  ofn.lpstrInitialDir = NULL;
  if (isreadingregistryok)
    ofn.lpstrInitialDir = LPCSTR(&path);

  ofn.Flags = OFN_EXPLORER|OFN_PATHMUSTEXIST|OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT;

  // Display the Open dialog box.
  if (GetSaveFileName(&ofn)==TRUE){

    if (strlen(szFile)>0) 
    {
      strcpy(result,szFile);

      // converts the file to unix
      //backslash2Slash(result);

      savePath(result);

      return true;
    }
  }

  return false;
}
