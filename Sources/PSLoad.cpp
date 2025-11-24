/*
 * PSInterfaceLoad.cpp
 *
 *  Created on: 18 avr. 2009
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */

#define _WIN32_WINNT 0x0500

#include <fstream>
#include "PSApplication.h"
#include "resource.h"
#include "Win32Tools.h"
#include "ExportObj.h"
#include "Timer.h"
#include "Limit.h"
#include "Includes.h"
#include "DirectXControl.h"
#include "UserInterrupt.h"
#include "ImageGeneration.h"
#include "unzip.h"
#include "File.h"
#include "Convert.h"
#include "ExifTools.h"

#define IMAGE_FILE_FILTER  "All\0*.*\0Jpg image format\0*.jpg\0Bmp image format\0*.bmp\0Tiff image format (8bit only)\0*.tif\0Png image format\0*.png\0\0"
#define IMAGE_FILE_FILTER2 "All\0*.*\0Jpg image format\0*.jpg\0Bmp image format\0*.bmp\08bit or 16 bit Tif image format\0*.tif\0Png image format\0*.png\0\0"
#define IMAGE_ERRROR_MESSAGE "Error : not a valid image type (try jpg, bmp, tif of png image)"

void set3Channels(Image& image)
{
  if (image.is_empty())
    THROW_INTERNAL_ERROR("empty image (in set3Channels())");

  if (image.dimv() == 3) return; // nothing to do, it's already an image with 3 channels

  if (image.dimv() == 1)
  {
    Image work(image.dimx(), image.dimy(), 1, 3);
    
    int  sizex = image.dimx(), sizey = image.dimy(), nbchannels = 3;

    for     (int j = 0 ; j < sizey;      j ++)
      for   (int i = 0 ; i < sizex;      i ++)
        for (int k = 0 ; k < nbchannels; k ++)
          work(i,j,0,k) = image(i,j);

    image = work;
  }
  else
    THROW_INTERNAL_ERROR("unrecognised image (in set3Channels())");
}

Image loadImage(char* filename)
{
  Image image;
  image.load(filename);

  if (image.is_empty())
    THROW_INTERNAL_ERROR("empty image (in loadImage())");

  set3Channels(image);

  int rot = getExifRotation(filename);

  // rotates the image only if in landscape mode. If in portrait mode then the image has most likely been rotated already.
  if (image.dimx() > image.dimy())
  {
    // if image is rotated, the counterrotate in the other direction
    if (rot == -90) image.rotate(90.0f);
    if (rot == 90)  image.rotate(-90.0f);
  }

  return image;
}


// Warning recent is ignored for now, should be 0
void PSApplication::loadRecent(int index)
{
  DEBUG(loRe);

  char lFileName[_MAX_PATH] = {0};
  char rFileName[_MAX_PATH] = {0};

  if(askBeforeDelete())
  {
    try
    {
      loadfromRegistryFileName(registry_, lFileName, rFileName, index);

      if (isExtension(lFileName, ".sculpt"))
      {
        PROGRESS(LOADING_RECENT);

        savePath(lFileName);
        saveFileNameInRegistry(registry_,lFileName, "");
        updateMenuRecentFiles_();
        requestRedraw();
        sources_.leftImage_  = Image();
        sources_.rightImage_ = Image();

        loadSculptFile(lFileName);

        doActionOnFirstThread(ACTION_MODE_3D);
        interface_.changeTitle   (lFileName);

        doActionOnFirstThread(ACTION_REINIT_3D_DISPLAY);
      }
      else if (readImageType(lFileName) != IMG_TYPE_UNKNOWN)
      {
        saveFileNameInRegistry(registry_,lFileName, rFileName);
        updateMenuRecentFiles_();
        result_.clear();
        PROGRESS(LOADING_RECENT);
        loadPhotos(lFileName, rFileName);
      }
      else
        THROW_MESSAGE_TO_USER(INVALID_TYPE3);
    }
    INTERFACE_CATCH_BLOC(LOADING);
  }
}

void PSApplication::loadDialogIdenticalLR()
{
  DEBUG(loDia);

  if (askBeforeDelete())
  {
    char FileName [_MAX_PATH] = {0};

    bool isSuccess = openFileDialog(FileName,interface_.windowsHandle_,
      "Open one image file", IMAGE_FILE_FILTER);

    if (isSuccess)
    {
      try 
      {

        int isJpegOrBmp1 = readImageType(FileName );

        if (isJpegOrBmp1 == -1)
          THROW_INTERNAL_ERROR(IMAGE_ERRROR_MESSAGE);

        // more than 2 files
        PROGRESS(LOADING);

        savePath(FileName);
        doActionOnFirstThread(ACTION_MODE_SOURCES);
        requestRedraw();

        sources_.leftImage_  = loadImage (FileName );
        sources_.rightImage_ = sources_.leftImage_;
        sources_.leftFocal_  = getExifFocalLenth35mm (FileName);

        strcpy(sources_.leftFileName_  , FileName );
		    strcpy(sources_.rightFileName_ , FileName );
        result_.clear();
        
      }
      INTERFACE_CATCH_BLOC(LOADING);
    }
  }
}

void PSApplication::loadDialog(bool isLeft)
{
  DEBUG(lodDia);

  if (askBeforeDelete())
  {
    char FileName [_MAX_PATH] = {0};
    char FileName2[_MAX_PATH] = {0};

    bool isSuccess=openFileDialog(FileName,FileName2,interface_.windowsHandle_,
      "Open 1 or 2 image file(s)", 
      IMAGE_FILE_FILTER);

    if (isSuccess)
    {
      try 
      {
        // is there a second file name from a multiselection?
        if (strlen(FileName2)==0)
        {
          if (isLeft)
		        loadPhotos(FileName, NULL);
          else		      
            loadPhotos(NULL, FileName);
        }
        else
        {
          int isJpegOrBmp1 = readImageType(FileName );
          int isJpegOrBmp2 = readImageType(FileName2);

          if (isJpegOrBmp1 == -1 || isJpegOrBmp2 == -1)
            THROW_INTERNAL_ERROR(IMAGE_ERRROR_MESSAGE);

          // more than 1 file
          PROGRESS(LOADING2);

          savePath(FileName);
          doActionOnFirstThread(ACTION_MODE_SOURCES);
          requestRedraw();

          sources_.leftImage_  = loadImage (FileName );
          sources_.rightImage_ = loadImage (FileName2);
          sources_.leftFocal_  = getExifFocalLenth35mm (FileName);

          strcpy(sources_.leftFileName_  , FileName );
		      strcpy(sources_.rightFileName_ , FileName2);
          result_.clear();
        }
      }
      INTERFACE_CATCH_BLOC(LOADING);
    }
  }
}

void PSApplication::loadAlphaDialog()
{
  DEBUG(loadAl);

  char FileName [_MAX_PATH] = {0};
  char FileName2[_MAX_PATH] = {0};

  bool isSuccess=openFileDialog(FileName,interface_.windowsHandle_,
    "Open Black and White Image File as Alpha Map",
    IMAGE_FILE_FILTER);

  if (isSuccess)
  {
    try 
    {
      // is there a second file name from a multiselection?
      if (strlen(FileName2)==0)
      {
        ProgressDialog progress("Loading alpha");

        savePath(FileName);
        requestRedraw();

        int isJpegOrBmp = readImageType(FileName);

        if (isJpegOrBmp == -1)
          THROW_INTERNAL_ERROR(IMAGE_ERRROR_MESSAGE);

        Image image; 
        image = loadImage (FileName);

        for (int i = 0; i < image.dimx(); i++)
          for (int j = 0; j < image.dimy(); j++)
          {
            int intensity = ( int( image(i,j,0,0))  + int (image(i,j,0,1)) + int (image(i,j,0,2) )) / 3 ;
            if (intensity>128)
              image(i,j,0,0) = image(i,j,0,1) = image(i,j,0,2) = 255; // white
            else
              image(i,j,0,0) = image(i,j,0,1) = image(i,j,0,2) = 0; // black
          }

        sources_.setAlpha(new Image(image));
      }

    }
    INTERFACE_CATCH_BLOC(LOADING);
  }
  
  doActionOnFirstThread(ACTION_REINIT_3D_DISPLAY);
}

void PSApplication::closeAlpha()
{
  const Image empty;
  sources_.setAlpha(new Image(empty));

  doActionOnFirstThread(ACTION_REINIT_3D_DISPLAY);
}

void PSApplication::loadDepthMapDialog()
{
  DEBUG(loDep);
  
  char FileName[_MAX_PATH] = "";

  bool isSuccess = openFileDialog(FileName,interface_.windowsHandle_, 
    "Open a grayscale, depth-coded image file",
    IMAGE_FILE_FILTER2);

  if (isSuccess)
  {
    requestRedraw();
    PROGRESS(LOADING3);

    try 
    {
      int isJpegOrBmp = readImageType(FileName);

      if (isJpegOrBmp == -1)
        THROW_INTERNAL_ERROR(IMAGE_ERRROR_MESSAGE);

      Image depthmapimage;
      Image16 depthmapimage16;

      if (isJpegOrBmp == 2)
      {
        depthmapimage16.load(FileName);

        IntMap v = ImageGeneration::loadDepthFromImage16(depthmapimage16);

        result_.updateAll(v,sources_.leftImage_,result_.getMode(),result_.getShader());
      }
      else
      {
        depthmapimage = loadImage (FileName);

        IntMap v = ImageGeneration::improveDepthMapWithNormalMap(depthmapimage);

        result_.updateAll(v,sources_.leftImage_,result_.getMode(),result_.getShader());
      }

      doActionOnFirstThread(ACTION_MODE_3D);
    }
    INTERFACE_CATCH_BLOC(LOADING);
  }
}

void PSApplication::loadSculptFile(char * FileName)
{
  DEBUG(loadScu);
  
  try 
  {
    // free memory by clearing result and directx
    result_.clear();

    char * extension = strrchr(FileName, '.');

    if (extension==0)
      THROW_INTERNAL_ERROR("Error : could not read sculpt format (you should open a .sculpt file)");

    bool isEqualSculpt=(strcmp(extension,".sculpt")==0 ||
                     strcmp(extension,".SCULPT")==0 ||
                     strcmp(extension,".Sculpt")==0);

    if (!isEqualSculpt)
      THROW_INTERNAL_ERROR("Error : not a valid sculpt format (you should open a .sculpt file)");
    
    HZIP hz = OpenZip(FileName,0);
    ZIPENTRY ze; 
    ZRESULT isError = GetZipItem(hz,-1,&ze); 

    if (isError != ZR_OK )
      THROW_MESSAGE_TO_USER(CANNOT_LOAD1);

    int numitems=ze.index;
    // -1 gives overall information about the zipfile

    char temppath[_MAX_PATH] = "";
    GetTempPath( _MAX_PATH, temppath );   // getTempPath get a full path ended with a backslash

    SetUnzipBaseDir( hz, temppath );

    for (int zi=0; zi<numitems; zi++)
    { 
      ZIPENTRY ze; 
      GetZipItem(hz,zi,&ze);              // fetch individual details
      UnzipItem(hz, zi, ze.name);         // e.g. the item's name.
    }
    CloseZip(hz);

    // Load Ambient image
    char nameambient[_MAX_PATH] = "";
    GetTempPath( _MAX_PATH, nameambient ); // getTempPath get a full path ended with a backslash
    strcat(nameambient, "ambient.bmp");
    
    if (!isFileExist(nameambient))
      THROW_MESSAGE_TO_USER(CANNOT_LOAD2 );

    sources_.leftImage_.load(nameambient);
    strcpy(sources_.leftFileName_,"");

    // Load Depth image
    char namedepth[_MAX_PATH] = "";
    GetTempPath( _MAX_PATH, namedepth );   // getTempPath get a full path ended with a backslash
    strcat(namedepth, "depth.bmp");

    // Load Normal image
    char namenormal[_MAX_PATH] = "";
    GetTempPath( _MAX_PATH, namenormal );   // getTempPath get a full path ended with a backslash
    strcat(namenormal, "normal.bmp");

    Image depthmapimage;
    Image normalmapimage;
    if (!isFileExist(namedepth))
      THROW_MESSAGE_TO_USER(CANNOT_LOAD3);
    if (!isFileExist(namenormal))
      THROW_MESSAGE_TO_USER(CANNOT_LOAD4);

    depthmapimage. load_bmp( namedepth  );
    normalmapimage.load_bmp( namenormal );
    IntMap model3d = ImageGeneration::improveDepthMapWithNormalMap( depthmapimage, normalmapimage );

    // Update the result container with both image and depth model
    result_.zScale_ = 1.0f;
    result_.updateAll(model3d,sources_.leftImage_,result_.getMode(),result_.getShader());

    DeleteFile( nameambient );
    DeleteFile( namedepth   );
    DeleteFile( namenormal  );
  }
  INTERFACE_CATCH_BLOC(LOADING);
}

void PSApplication::loadSculptFileDialog()
{
  DEBUG(loadScuDia);
  
  char FileName[_MAX_PATH] = "";

  bool isSuccess = openFileDialog(FileName,interface_.windowsHandle_,"Open a .sculpt file",
    "Sculpt open file format\0*.sculpt\0All\0*.*\0\0");

  if (isSuccess)
  {
    PROGRESS(LOADING4);
    requestRedraw();

    saveFileNameInRegistry(registry_,FileName, "");
    updateMenuRecentFiles_();
    doActionOnFirstThread(ACTION_MODE_3D);

    sources_.leftImage_  = Image();
    sources_.rightImage_ = Image();

    loadSculptFile(FileName);
    interface_.changeTitle   (FileName);

    doActionOnFirstThread(ACTION_REINIT_3D_DISPLAY);
  }
}

void PSApplication::loadPhotos(char * filenameL, char * filenameR)
{
  PROGRESS(LOADING5);

  doActionOnFirstThread(ACTION_MODE_SOURCES);
  requestRedraw();

  if (filenameL != NULL)
  {
    savePath(filenameL);

    int imagetype = readImageType(filenameL);

    if (imagetype == IMG_TYPE_UNKNOWN)
      THROW_INTERNAL_ERROR(IMAGE_ERRROR_MESSAGE);

    sources_.leftImage_ = loadImage (filenameL);
    sources_.leftFocal_ = getExifFocalLenth35mm (filenameL);
	  strcpy(sources_.leftFileName_,  filenameL);
  }
  if (filenameR != NULL)
  {
    savePath(filenameR);


    int imagetype = readImageType(filenameR);

    if (imagetype == IMG_TYPE_UNKNOWN)
      THROW_INTERNAL_ERROR(IMAGE_ERRROR_MESSAGE);

    sources_.rightImage_ = loadImage (filenameR);
	  strcpy(sources_.rightFileName_,  filenameR);
  }

  doActionOnFirstThread(ACTION_MODE_SOURCES);
  requestRedraw();
}

void PSApplication::loadSculptFromCommandLine(char * lpCmdLine)
{
  DEBUG(loaCo);
  
  try
  {
    char FileName[_MAX_PATH] = "";
    strcpy(FileName, lpCmdLine);
    removeQuotes(FileName);

    if (isExtension(FileName, ".sculpt"))
    {
      PROGRESS(LOADING4);

      savePath(FileName);
      requestRedraw();

      saveFileNameInRegistry(registry_,FileName, "");
      updateMenuRecentFiles_();
      doActionOnFirstThread(ACTION_MODE_3D);
      sources_.leftImage_  = Image();
      sources_.rightImage_ = Image();

      loadSculptFile(FileName);
      interface_.changeTitle   (FileName);

      doActionOnFirstThread(ACTION_REINIT_3D_DISPLAY);
    }
  }
  CATCH_BLOC("Error loading from command line");
}


void PSApplication::loadAdditionalPhotoDialog()
{
  DEBUG(loadAd);
  
  if (askBeforeDelete())
  {
    char FileName [_MAX_PATH] = {0};

    bool isSuccess = openFileDialog(FileName,interface_.windowsHandle_,
      "Open left image file", IMAGE_FILE_FILTER);

    if (isSuccess)
    {
      try 
      {
        // is there a second file name from a multiselection?
        int imagetype = readImageType(FileName );

        if (imagetype == -1 ) THROW_INTERNAL_ERROR(IMAGE_ERRROR_MESSAGE);

        // more than 2 files
        PROGRESS(LOADING6);

        savePath     (FileName);
        doActionOnFirstThread      (ACTION_MODE_SOURCES);
        requestRedraw();

        sources_.additionalImage_ = loadImage (FileName );
        result_.clear();
      }
      INTERFACE_CATCH_BLOC(LOADING);
    }
  }
}
