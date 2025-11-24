/*
 * PSInterfaceDrop.cpp
 *
 *  Created on: 26 oct. 2009
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */

#include "PSApplication.h"
#include "Includes.h"
#include "File.h"
#include "ExifTools.h"

extern Image loadImage(char* filename);

int dropFilesInfo(const HDROP& hDrop, char * filename1, char * filename2, int& mousex, int& mousey)
{
  DEBUG(drpFi);
  NANOBEGIN;
  ALWAYS_EXECUTE_ON_WINDOWS_LOOP_THREAD();
  // clears filenames and mouse coordinates
  *filename1 = *filename2 = 0; 
  mousex=mousey = -1;

  char name[MAX_PATH]={0};
  int nb = DragQueryFile(hDrop, 0xFFFFFFFF, name, MAX_PATH);

  if (nb>=1)
  {
    POINT point;
    int isInClientArea = DragQueryPoint(hDrop, &point);
    mousex = point.x ;
    mousey = point.y ;

    int isok1 = DragQueryFile(hDrop, 0, filename1, MAX_PATH);
    if (!isok1)
      THROW_INTERNAL_ERROR( "Error : Could not receive droped file" );

    if (nb>=2)
    {
      char name2[MAX_PATH]="";
      int isok2 = DragQueryFile(hDrop, 1, filename2, MAX_PATH);
      if (!isok2)
        THROW_INTERNAL_ERROR( "Error : Could not receive droped file" );
    }
  }

  DragFinish(hDrop);

  NANOEND;
  return nb;
}

void PSApplication::dropFiles(const HDROP& hDrop)
{
  DEBUG(dropFi);
  try
  {
    ALWAYS_EXECUTE_ON_WINDOWS_LOOP_THREAD();
    char name1[MAX_PATH]="", name2[MAX_PATH]="";
    int mousex, mousey;

    int nb = dropFilesInfo(hDrop, name1, name2, mousex, mousey);

    if (nb>=1)
    {
      bool isSculpt = isExtension(name1, ".sculpt");

      if (isSculpt)
      {
        if (nb == 1)
        {
          if (askBeforeDelete())
          {
            ProgressDialog progress;
            stop3DDisplay();
            
            setPage(ACTION_MODE_3D);
            requestRedraw();
            saveFileNameInRegistry(registry_,name1, "");
            updateMenuRecentFiles_();
            sources_.leftImage_  = Image();
            sources_.rightImage_ = Image();

            loadSculptFile        (name1);
            interface_.changeTitle(name1);
            reinit3DDisplay();

            savePath(name1);
          }
        }
        else
          THROW_MESSAGE_TO_USER(1_SCULPT_FILE);
      }
      else if (result_.getMode() == ACTION_MODE_SOURCES || result_.getMode() == ACTION_MODE_3D)
      {
        if (askBeforeDelete())
        {
          // single file
          if (nb==1)
          {
            bool isLeft = mousex < interface_.image_.dimx() / 2 ;
            int  type   = readImageType( name1 );

            if (type != IMG_TYPE_UNKNOWN)
            {
              if (isLeft) loadPhotos(name1, NULL);
              else        loadPhotos(NULL, name1);
            }
            else
              THROW_MESSAGE_TO_USER(INVALID_TYPE1);
          }
          else if (nb==2)
          {

            int type1 = readImageType(name1);
            int type2 = readImageType(name2);

            if (type1 == IMG_TYPE_UNKNOWN || type2 == IMG_TYPE_UNKNOWN)
              THROW_MESSAGE_TO_USER(INVALID_TYPE2);

            // Order so first photo is left and second photo is right
            bool isAlphabeticalOrder = (strcmp(name1,name2)<0);

            if (isAlphabeticalOrder)   loadPhotos(name1,name2);
            else                       loadPhotos(name2,name1);
          }
          else if (nb>2)
            THROW_MESSAGE_TO_USER(MORE_THAN_2_IMAGES);
        }
        else
          THROW_MESSAGE_TO_USER(CANNOT_RECEIVE);
      }
    }
    else // < 1 file
      THROW_INTERNAL_ERROR( "Error : Could not receive droped file" );
  }
  INTERFACE_CATCH_BLOC(DROPING);
}