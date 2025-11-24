/*
 * PSInterfaceDialogSave.cpp
 *
 *  Created on: 3 mai 2009
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */

const int   EXPORT_SKIP  = 1      ;

#include <stdexcept>
#include "PSApplication.h"
#include "View3D.h"
#include "RESOURCE.H"
#include "zip.h"
#include "File.h"
#include "Includes.h"
#include "ExportPly.h"

#define IMAGE_FILE_FILTER    STR(IMAGE_FORMATS)
#define IMAGE_ERRROR_MESSAGE STR(ERROR_INVALID_TYPE4)

void PSApplication::saveOneImage(char * filename, int type, int subdivision)
{
  // add the jpg extension by default if no extension was found
  char * extension = strrchr(filename, '.');
  if (extension == NULL)
    strcat(filename,".jpg");

  int ChoiceJpgBmp = readImageType( filename );

  if (ChoiceJpgBmp==-1)
      THROW_INTERNAL_ERROR(IMAGE_ERRROR_MESSAGE);

  const int nbcodes        = 10;
  const int codes[nbcodes] = { ACTION_MODE_DIFFUSE, ACTION_MODE_DISPLACE, ACTION_MODE_NORMAL,
    ACTION_MODE_AMBOCC, ACTION_MODE_SPECULAR, ACTION_MODE_OLD_3D, ACTION_MODE_ALPHA , 
    ACTION_MODE_BUMP, ACTION_MODE_LIMDISP, ACTION_MODE_TILE_MASK};

  result_.isInvertNormalX_ = GlobalOptions.isNormalInvertedX_;
  result_.isInvertNormalY_ = GlobalOptions.isNormalInvertedY_;

  try
  {
    for (int i=0 ; i<nbcodes ; i++)
      if (codes[i] == type)
      {
        Result resultInProgress = result_;

        resultInProgress.subdivision_ = subdivision;

        resultInProgress.updateAll(result_.getOriginalMap(),sources_.leftImage_,codes[i],
          result_.getShader());

        // Builds the image to be saved
        Image returned = resultInProgress.getImage();

        if (ChoiceJpgBmp == IMG_TYPE_BMP)
          returned.save_bmp(filename);
        
        else if (ChoiceJpgBmp == IMG_TYPE_JPG)
          returned.save_jpeg(filename, GlobalOptions.jpgPercent_);
        
        else if (ChoiceJpgBmp == IMG_TYPE_TIF)
          returned.save_tiff(filename);
        
        else if (ChoiceJpgBmp == IMG_TYPE_PNG)
          returned.save_png(filename);
      }
  }
  INTERFACE_CATCH_BLOC(SAVE_IMAGES)
}


void PSApplication::saveOneImageExt(char * filename, int type, int subdivision)
{
  if (subdivision <0 )
    subdivision = result_.subdivision_;

  // add the jpg extension by default if no extension was found
  char * extension = strrchr(filename, '.');
  if (extension == NULL)
    strcat(filename,".jpg");

  int ChoiceJpgBmp = readImageType( filename );

  if (ChoiceJpgBmp==-1)
      THROW_INTERNAL_ERROR(IMAGE_ERRROR_MESSAGE);

  const int nbcodes        = 10 ;
  const int codes[nbcodes] = { 
    ACTION_MODE_DIFFUSE,  ACTION_MODE_DISPLACE,
    ACTION_MODE_NORMAL,   ACTION_MODE_AMBOCC,
    ACTION_MODE_SPECULAR, ACTION_MODE_ALPHA,
    ACTION_MODE_BUMP,     ACTION_MODE_LIMDISP,
    ACTION_MODE_OLD_3D,   ACTION_MODE_TILE_MASK};

  result_.isInvertNormalX_ = GlobalOptions.isNormalInvertedX_;
  result_.isInvertNormalY_ = GlobalOptions.isNormalInvertedY_;

  const char * table[nbcodes]={ "-diffuse", "-displacement", "-normal", "-ambientocclusion" ,
    "-specular", "-alpha", "-bump", "-limiteddisp", "-3D" };

  try
  {
    for (int i=0 ; i<nbcodes ; i++)
      if (codes[i] == type)
      {
        char newname[_MAX_PATH] = {0}, ext[10] = {0};
        strcpy(newname,filename);
        extractExtension(ext, 10, filename);

        // remove .jpg or .bmp
        removeExtension(newname);

        // add -d, -n, ...
        strcat(newname, table[i]);

        // add .jpg or .bmp
        strcat(newname, ext);

        saveOneImage(newname, type, subdivision);
      }
  }
  INTERFACE_CATCH_BLOC(SAVE_IMAGES)
}

void PSApplication::save16BitTiffDialog()
{
  if (!result_.isResult())
    THROW_MESSAGE_TO_USER(NOTHING_SAVE);

  char filename[_MAX_PATH] = {0};

  bool isSuccess =  saveAsFileDialog(filename,interface_.windowsHandle_,
                    STR(IMAGE_FORMATS2), "*.tif" );
  if (isSuccess)
  {
    ProgressDialog progress(STR(PROGRESS_SAVING_TIFF));
    requestRedraw();

    Image16 returned = ImageGeneration::generateDepth16(result_.getMap());

    returned.save_tiff(filename);
  }
}

void PSApplication::saveAsObjFile(char * filename, int subdivmodel, int subdivimage)
{
  try
  {
    bool isEqualObj = isExtension(filename,".obj");

    if (isEqualObj)
    {
      // get the model
      Result rescopy = result_;
      rescopy.subdivision_ = subdivmodel;
      
      rescopy.updateAll(result_.getOriginalMap(),sources_.leftImage_,ACTION_MODE_3D,
        result_.getShader());

      IntMap  modelToBeExported = rescopy.getMap  ();

      // precomputes scale using model data
      float scale = rescopy.zScale_ / MULTIPLY / float(rescopy.subdivision_);

      if (subdivmodel != subdivimage)
      {
        // get the image
        rescopy = result_;
        rescopy.subdivision_ = subdivimage;
        
        rescopy.updateAll(result_.getOriginalMap(),sources_.leftImage_,ACTION_MODE_3D,
          result_.getShader());
      }
      Image   imageToBeExported = rescopy.getImage();
      
      Image  alphaToBeExported;
      if (sources_.isAlpha())
      {
        rescopy.updateAll(result_.getOriginalMap(),sources_.leftImage_,ACTION_MODE_ALPHA,
          result_.getShader());

        alphaToBeExported = rescopy.getImage ();
      }

      rescopy.clear(); // free memory

      // Writes 3D model to disk

      bool isExported = ExportObj::exportAllObjFile(filename, imageToBeExported, alphaToBeExported,
        modelToBeExported, EXPORT_SKIP, 1.0, scale, GlobalOptions.jpgPercent_, 
        result_.bestPOV_.getFocalLength());
    }
  }
  INTERFACE_CATCH_BLOC(SAVE_3D)
}

void PSApplication::saveAsStlFile(char * filename, int subdivision)
{
  if ( subdivision < 0 )
    subdivision = result_.subdivision_;

  try
  {
    bool isEqualStl = isExtension(filename,".stl");

    if (isEqualStl)
    {
      Result rescopy = result_;
      rescopy.subdivision_ = subdivision;
      rescopy.updateAll(result_.getOriginalMap(),sources_.leftImage_,ACTION_MODE_3D,result_.getShader());

      IntMap  modelToBeExported = rescopy.getMap  ();

      Image  alphaToBeExported;
      if (sources_.isAlpha())
      {
        rescopy.updateAll(result_.getOriginalMap(),sources_.leftImage_,ACTION_MODE_ALPHA,
          result_.getShader());

        alphaToBeExported = rescopy.getImage ();
      }

      // Writes 3D model to disk
      bool isExported = ExportStl::exportStlFile(filename,
        modelToBeExported, alphaToBeExported, EXPORT_SKIP, 1.0, 
        rescopy.zScale_/MULTIPLY/float(rescopy.subdivision_), 
        sources_.leftFocal_);
    }
  }
  INTERFACE_CATCH_BLOC(SAVE_3D)
}


void PSApplication::saveAsPlyFile(char * filename, int subdivmodel, int subdivimage)
{
  try
  {
    bool isok = isExtension(filename,".ply");

    if (isok)
    {
      // get the model
      Result rescopy = result_;
      rescopy.subdivision_ = subdivmodel;
      
      rescopy.updateAll(result_.getOriginalMap(),sources_.leftImage_,ACTION_MODE_3D,
        result_.getShader());

      IntMap  modelToBeExported = rescopy.getMap  ();


      // precomputes scale using model data
      float scale = rescopy.zScale_ / MULTIPLY / float(rescopy.subdivision_);

      if (subdivmodel != subdivimage)
      {
        // get the image
        rescopy = result_;
        rescopy.subdivision_ = subdivimage;
        
        rescopy.updateAll(result_.getOriginalMap(),sources_.leftImage_,ACTION_MODE_3D,
          result_.getShader());
      }
      Image   imageToBeExported = rescopy.getImage();
      
      Image  alphaToBeExported;
      if (sources_.isAlpha())
      {
        rescopy.updateAll(result_.getOriginalMap(),sources_.leftImage_,ACTION_MODE_ALPHA,
          result_.getShader());

        alphaToBeExported = rescopy.getImage ();
      }

      rescopy.clear(); // free memory

      // Writes 3D model to disk

      bool isExported = ExportPly::exportFile(filename, imageToBeExported, alphaToBeExported,
        modelToBeExported, EXPORT_SKIP, 1.0, scale, sources_.leftFocal_);
    }
  }
  INTERFACE_CATCH_BLOC(SAVE_3D)
}


void PSApplication::saveAsSculptFile(char * filename, int subdivision)
{
  if (subdivision <0 )
    subdivision = result_.subdivision_;

  try
  {
    bool isEqualSculpt = isExtension(filename,".sculpt");

    if (isEqualSculpt)
    {
      // Builds the depth image to be saved
      Result tempresult = result_;

      tempresult.subdivision_ = subdivision;

      tempresult.updateAll(result_.getOriginalMap(),sources_.leftImage_,ACTION_MODE_DISPLACE,
        result_.getShader());
      Image imagedepth = tempresult.getImage();

      // Important: do not invert any normal map when exporting sculpt!
      tempresult.isInvertNormalX_ = false;
      tempresult.isInvertNormalY_ = false;

      // Builds the normal image to be saved
      tempresult.updateAll(result_.getOriginalMap(),sources_.leftImage_,ACTION_MODE_NORMAL,
        result_.getShader());
      Image imagenormal = tempresult.getImage();

      tempresult.updateAll(result_.getOriginalMap(),sources_.leftImage_,ACTION_MODE_DIFFUSE,
        result_.getShader());
      Image imagecolor = tempresult.getImage();

      char nameWOExtension[_MAX_PATH] = "temp0125";

      // Save the ambient color image file to temp directory
      char nameambient[_MAX_PATH] = "";
      GetTempPath( _MAX_PATH, nameambient ); // getTempPath get a full path ended with a backslash
      strcat(nameambient, nameWOExtension);
      strcat(nameambient, ".bmp");
      imagecolor.save_bmp(nameambient);

      // Save the depth file to temp directory
      char namedepth[_MAX_PATH] = "";
      GetTempPath( _MAX_PATH, namedepth );   // getTempPath get a full path ended with a backslash
      strcat(namedepth, nameWOExtension);
      strcat(namedepth, "_d.bmp");
      imagedepth.save_bmp(namedepth);

      // Save the normal map file to temp directory
      char namenormal[_MAX_PATH] = "";
      GetTempPath( _MAX_PATH, namenormal );   // getTempPath get a full path ended with a backslash
      strcat(namenormal, nameWOExtension);
      strcat(namenormal, "_n.bmp");
      imagenormal.save_bmp(namenormal);

      // Save the zip file
      changePathToBackslash(filename);

      HZIP hz = CreateZip(filename,0);
      ZRESULT isError1 = ZipAdd( hz, "ambient.bmp", nameambient );
      ZRESULT isError2 = ZipAdd( hz, "depth.bmp"  , namedepth   );
      ZRESULT isError3 = ZipAdd( hz, "normal.bmp" , namenormal  );
      if (isError1 != ZR_OK || isError2 != ZR_OK || isError3 != ZR_OK)
        THROW_INTERNAL_ERROR("Error : could not save the sculpt file");

      CloseZip(hz);

      DeleteFile(nameambient);
      DeleteFile(namedepth);
      DeleteFile(namenormal);
    }
  }
  INTERFACE_CATCH_BLOC(SAVE_3D)
}

void PSApplication::saveAsModelDialog()
{
  if (!result_.isResult())
    THROW_MESSAGE_TO_USER(NOTHING_SAVE);

  char filename[_MAX_PATH] = "";

  bool isSuccess = saveAsFileDialog(filename,interface_.windowsHandle_,
      STR(IMAGE_FORMATS3) "\0", "*.obj");

  if (isSuccess)
  {
    requestRedraw();
    PROGRESS(SAVING_MODEL);

    bool isEqualStl    = isExtension(filename, ".stl"   );
    bool isEqualObj    = isExtension(filename, ".obj"   );
    bool isEqualSculpt = isExtension(filename, ".sculpt");
    bool isEqualPly    = isExtension(filename, ".ply"   );

    requestRedraw();

    if (isEqualObj)          saveAsObjFile (filename,result_.subdivision_,1);
    else if (isEqualStl)     saveAsStlFile (filename,result_.subdivision_  );
    else if (isEqualPly)     saveAsPlyFile (filename,result_.subdivision_,1);
    else if (isEqualSculpt)  
    {
      saveFileNameInRegistry(registry_,filename, "");
      updateMenuRecentFiles_();
      saveAsSculptFile      (filename, result_.subdivision_);
      interface_.changeTitle(filename);
    }
    else
      THROW_MESSAGE_TO_USER(UNKNOWN_FORMAT);
  }
}


void PSApplication::saveAsDialog()
{
  if (!result_.isResult())
    THROW_MESSAGE_TO_USER(NOTHING_SAVE);

  if (result_.getMode() == ACTION_MODE_DISPLACE || 
      result_.getMode() == ACTION_MODE_NORMAL   || 
      result_.getMode() == ACTION_MODE_AMBOCC   || 
      result_.getMode() == ACTION_MODE_SPECULAR ||
      result_.getMode() == ACTION_MODE_DIFFUSE  ||
      result_.getMode() == ACTION_MODE_ALPHA    ||
      result_.getMode() == ACTION_MODE_OLD_3D   ||
      result_.getMode() == ACTION_MODE_BUMP     ||
      result_.getMode() == ACTION_MODE_LIMDISP  ||
      result_.getMode() == ACTION_MODE_TILE_MASK)

    saveAsImageDialog();

  else if (result_.getMode() == ACTION_MODE_3D)
    saveAsModelDialog();
}

void PSApplication::saveAsImageDialog()
{
  if (!result_.isResult())
    THROW_MESSAGE_TO_USER(NOTHING_SAVE);

  int mode = result_.getMode();

  if (mode == ACTION_MODE_DISPLACE || 
      mode == ACTION_MODE_NORMAL   || 
      mode == ACTION_MODE_AMBOCC   || 
      mode == ACTION_MODE_SPECULAR ||
      mode == ACTION_MODE_DIFFUSE  ||
      mode == ACTION_MODE_ALPHA    ||
      mode == ACTION_MODE_OLD_3D   ||
      mode == ACTION_MODE_BUMP     ||
      mode == ACTION_MODE_LIMDISP  ||
      mode == ACTION_MODE_TILE_MASK)
  {
    char filename[_MAX_PATH] = {0};

    bool isSuccess = saveAsFileDialog(filename,interface_.windowsHandle_,
                     IMAGE_FILE_FILTER,
                     "*.png");

    if (isSuccess)
    {
      ProgressDialog progress(STR(PROGRESS_SAVING_IMAGE));

      requestRedraw();

      saveOneImage(filename, mode, result_.subdivision_);
    }
  }
  else if (mode == ACTION_MODE_3D)
  {
    char filename[_MAX_PATH] = {0};

    bool isSuccess = saveAsFileDialog(filename,interface_.windowsHandle_,
                     IMAGE_FILE_FILTER,
                     "*.jpg");

    if (isSuccess)
    {
      PROGRESS(SAVING_SOFT);
      requestRedraw();

      saveOneImage(filename, ACTION_MODE_OLD_3D, result_.subdivision_);
    }
  }
}
