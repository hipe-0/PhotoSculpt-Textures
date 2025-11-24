/*
 * BatchSculpt.cpp 
 *
 *  Created on: 24 févr. 2011
  *      Author: Hippolyte Mounier, Copyright 2011 All rights reserved
 */

#include "PSApplication.h"
#include "BatchSculpt.h"

HWND global_window_handle;
static TCHAR previousfilename[ MAX_PATH ] = "";

void PSApplication::actionBatchSculpt( int mode )
{
  if (mode != ACTION_BATCH_SCULPT_TILES && mode != ACTION_BATCH_SCULPT_FOLDER)
    THROW_INTERNAL_ERROR("Unknown input mode (in actionBatchSculpt())");

  if(askBeforeDelete())
  {
    global_window_handle = interface_.windowsHandle_;
    
    char path[MAX_PATH] = "";
    if (browseFolderDialog(path))
    {
      searchFolderForSculpt( path, mode );
      
      commandLine_.clear();
    }
  }
}

#include "Frame.h"
#include "Color.h"

bool isSufficientlySimilar(const Image& imagea, const Image& imageb)
{
  DEBUG(isSuff);

  if (imagea.is_empty())  return false;
  if (imageb.is_empty()) return false;
  if (imagea.dimx() != imageb.dimx()) return false;
  if (imagea.dimy() != imageb.dimy()) return false;

  int sizex = imagea.dimx();
  int sizey = imagea.dimy();

  Frame::fit(sizex, sizey, CHECK_IMAGE_SIMILARITY_SIZE, CHECK_IMAGE_SIMILARITY_SIZE);

  #define MOVING_AVERAGE_INTERPOLATION 2
  Image smalla = imagea.get_resize(sizex, sizey,-100,-100,MOVING_AVERAGE_INTERPOLATION);
  Image smallb = imageb.get_resize(sizex, sizey,-100,-100,MOVING_AVERAGE_INTERPOLATION);

  smalla.blur(IMAGE_SINGULARITY_BLUR);
  smallb.blur(IMAGE_SINGULARITY_BLUR);

  //DEBUG_SAVE_IMAGE(smalla, "Small image a.jpg");
  //DEBUG_SAVE_IMAGE(smallb, "Small image b.jpg");

  float sumscore = 0.0f;
  for (int x = 0; x < sizex; x++)
    for (int y = 0; y < sizey; y++)
    {
      ColorF colora = getPixelF(smalla, x,y);
      ColorF colorb = getPixelF(smallb, x,y);

      float sq_r = colora.r-colorb.r;
      float sq_g = colora.g-colorb.g;
      float sq_b = colora.b-colorb.b;

      sumscore += sq_r*sq_r + sq_g*sq_g + sq_b*sq_b ;
    }
  
  float mean0to1 = sqrtf (sumscore / (3 * sizex * sizey));
  DEBUG_VALUE("  similarity mean per pixel (between 0 and 1)", mean0to1);

  if (mean0to1 <= IMAGE_SIMILARITY_LIMIT) return true;
  else                                    return false;
}

bool PSApplication::executeCommandedExtraction(CommandLineOrder command)
{
  DEBUG(exec);
  NEVER_EXECUTE_ON_WINDOWS_THREAD();

  clearSculptAndResults();
  
  commandLine_ = command;
  if (strlen(commandLine_.frontPhotoFileName_) == 0 ||
      strlen(commandLine_.rightPhotoFileName_) == 0 )
    return false;

  loadPhotos(commandLine_.frontPhotoFileName_, commandLine_.rightPhotoFileName_);

  if (isSufficientlySimilar(sources_.leftImage_, sources_.rightImage_))
  {
    if (commandLine_.isSquareTile_ || commandLine_.is2nSize_ )
    {
      Cropper crop;
      crop.setCropDimensions(0.0f, 0.0f, 1.0f, 1.0f); // full screen crop

      if (commandLine_.is2nSize_ )      
        crop.set2N            (true);

      if (commandLine_.isSquareTile_)
      {
        crop.setSeamless      (true);
        crop.setSquareRectify (true, sources_.leftImage_.dimx(), sources_.leftImage_.dimy());
      }

      result_.setCropper(crop);
    }

    runExtraction();
  }

  return commandLine_.isOutput_;
}


//BROWSE FOLDER - Opens a browse folder dialog.
bool browseFolderDialog(char * returnpath)
{
  DEBUG(Brows);
  // clear the string previousfilename
  previousfilename[0] = '\0';

  BROWSEINFO bi = { 0 };
  bi.lpszTitle = ("Specify the folder that contains the Photos to sculpt automatically.");
  bi.ulFlags   = BIF_USENEWUI ; // BIF_USENEWUI = new user inerface

  LPITEMIDLIST pidl = SHBrowseForFolder ( &bi );

  if ( pidl != 0 )
  {
    // get the name of the folder and put it in path
    SHGetPathFromIDList ( pidl, returnpath );

    // free memory used
    IMalloc * imalloc = 0;
    if ( SUCCEEDED( SHGetMalloc ( &imalloc )) )
    {
      imalloc->Free ( pidl );
      imalloc->Release ( );
      return true;
    }
  }
  return false;
}

static TCHAR pathbak[ MAX_PATH ];

//SEARCH FOLDER - Searches folder
//reading every file it comes across
void searchFolderForSculpt( TCHAR * path, int mode) 
{     
  DEBUG(searchFold);

  WIN32_FIND_DATA FindFileData;
  HANDLE hFind;

  //Make a backup of the directory the user chose         
  strcpy( pathbak, path );
        
  SetCurrentDirectory ( path );

  //Find the first file in the directory the user chose     
  hFind = FindFirstFile ( "*.*", &FindFileData );

  do     
  {
    if ( hFind != INVALID_HANDLE_VALUE )         
    {             
      //Is it a . or .. directory? If it is, skip, or we'll go forever

      if ( ! ( strcmp( FindFileData.cFileName, "." ) ) ||
           ! ( strcmp( FindFileData.cFileName, ".." ) ) )
        continue;

      //Restore the original directory chosen by the user
      strcpy( path, pathbak );

      //Append the file found on to the path of the
      //directory the user chose
      sprintf( path, "%s\\%s", path, FindFileData.cFileName );

      //if (SetCurrentDirectory(path))
        //SearchFolder(path); // for recursivity

      bool isok = batchSculptFilesFromPath(path, mode);

      if (!isok) break;
    }
  }
  while ( FindNextFile ( hFind, &FindFileData )
      && hFind != INVALID_HANDLE_VALUE );

  FindClose ( hFind );
}

int getSeconds(char * filename)
{
  FILETIME ftCreate, ftAccess, ftWrite;

  //opuverture du handle de fichier si existe
  HANDLE hFile = CreateFile(filename,0,FILE_SHARE_READ,NULL,
  OPEN_EXISTING,0,NULL);
  if (hFile != INVALID_HANDLE_VALUE)
  {
    //Recupere les temps du fichier
    if (!GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite))
      return -1;
    else
    CloseHandle(hFile);
  }
  else
    return -1;

  //Converti le temp de creation en temp local
  SYSTEMTIME systemtime;
  FileTimeToSystemTime(&ftWrite, &systemtime);
  
  return int (systemtime.wMonth)        * 60 * 60 * 24 * 31 +
         int (systemtime.wDay)          * 60 * 60 * 24 +
         int (systemtime.wHour)         * 60 * 60 +
         int (systemtime.wMinute)       * 60 +
         int (systemtime.wSecond)       ;
}

extern HWND global_window_handle;
extern PSApplication  MainApp;

bool batchSculptFilesFromPath(char * currentfilename, int mode)
{
  DEBUG(batchScul);

  if ( isExtension(currentfilename, ".jpg") )
  {
    int secondscurrentfile = getSeconds( currentfilename );
    DEBUG_WRITE(currentfilename);

    if (isExtension(previousfilename, ".jpg"))
    {
      int secondspreviousfile = getSeconds( previousfilename );

      int secondsbetweenshots = 1000 ;
      if (secondscurrentfile != -1 && secondspreviousfile != -1)
        secondsbetweenshots = secondscurrentfile - secondspreviousfile;
      DEBUG_VALUE("  seconds between shots", secondsbetweenshots);

      if (secondsbetweenshots < MAX_SECONDS_BETWEEN_SHOTS)
      {
        /*
        char exe[MAX_PATH] = "";
        GetModuleFileName(NULL, exe, 1000);

        char text[1000] = "\"";
        strncat(text, previousfilename, 1000);
        strncat(text, "\" \"", 1000);
        strncat(text, currentfilename, 1000);
        strncat(text, "\" -saveallmaps -exit -sculpt -sculptatonce", 1000);
        DEBUG_WRITE(text);
        // ShellExecute(global_window_handle, "open", exe, text, NULL, SW_SHOWNORMAL);
        */
        CommandLineOrder command;

        strncpy(command.frontPhotoFileName_, previousfilename, MAX_PATH);
        strncpy(command.rightPhotoFileName_, currentfilename,  MAX_PATH);

        command.isForcedSculpt_   = true  ;
        command.isForcedSaveMaps_ = false ;
        command.isForcedExit_     = false ;
        command.isObj_            = false ;
        command.isPly_            = false ;
        command.isStl_            = false ;
        command.isDisplacement_   = true  ;
        command.isNormal_         = true  ;
        command.isSpecular_       = true  ;
        command.isDiffuse_        = true  ;
        command.isOcclusion_      = true  ;
        command.isBump_           = false ;
        command.isAlpha_          = false ;
        command.isLimDisp_        = false ;
        command.is3D_             = true  ;
        command.isOutput_         = true  ;

        if (mode == ACTION_BATCH_SCULPT_FOLDER)
        {
          command.isSculpt_         = true  ;
          command.maxStep_          = 7     ;
          command.isSquareTile_     = false ;
          command.is2nSize_         = false ;
          strncpy(command.outputPath_, pathbak, MAX_PATH);
          strncat(command.outputPath_, DEFAULT_BATCH_OUT_DIRECTORY, MAX_PATH);
        }
        else if (mode == ACTION_BATCH_SCULPT_TILES)
        {
          command.isSculpt_         = false ; // sculpt file is slow to create in 2048x2048
          command.maxStep_          = 99    ;
          command.isSquareTile_     = true  ;
          command.is2nSize_         = true  ;
          strncpy(command.outputPath_, pathbak, MAX_PATH);
          strncat(command.outputPath_, DEFAULT_BATCH_OUT_DIR_TILES, MAX_PATH);
        }

        if (!SetCurrentDirectory(command.outputPath_))
          CreateDirectory(command.outputPath_, NULL);

        char * filenamewithoutpath = pointToFileNameOnly(previousfilename);
        char newfilename[MAX_PATH] = "";
        strncpy(newfilename, filenamewithoutpath, MAX_PATH);
        removeExtension(newfilename);
        strncat(newfilename, ".png", MAX_PATH);

        strncat(command.outputPath_, newfilename, MAX_PATH);

        bool isok = MainApp.executeCommandedExtraction(command);
        if (!isok) return false;

        *currentfilename = '\0'; // erases the string so it's not used again
      }
    }
  }

  strncpy(previousfilename, currentfilename, MAX_PATH);
  return true;
}