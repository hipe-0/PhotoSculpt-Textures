/*
 * ExifTools.cpp
 *
 *  Created on: 2nd dec 2010
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */

#include <iomanip>
#include "ExifImageFile.h"

#include "Error.h"

#ifndef EXIFTAG_FOCALLENGTHIN35MMFILM
  #define EXIFTAG_FOCALLENGTHIN35MMFILM 41989	/* Focal length in 35 mm film */
#endif

int getExifRotation(char* filename)
{
  int rotation = 0, rotationangle = 0;

  ExifImageFile inImageFile;
  
  // Open the file in read-only mode and verify that it succeeds
  if ( inImageFile.open( filename, "r" ) == EXIF_OK)
  {
    // Get all the AppSeg 1 - "Exif" tags 
    ExifPathsTags app1PathsTags ;
    inImageFile.getAllTags( 0xFFE1, "Exif", app1PathsTags ) ;

    ExifPathsTagsIter crntPathsTags = app1PathsTags.begin() ;
    ExifPathsTagsIter endPathsTags = app1PathsTags.end() ;
    while( crntPathsTags != endPathsTags )
    {
        ExifIFDPathIter crntPath = (*crntPathsTags).first.begin() ;
        ExifIFDPathIter endPath = (*crntPathsTags).first.end() ;
        while( crntPath != endPath )
        {
            exiftag_t ifd = (*crntPath).first  ;
            uint16 idx = (*crntPath).second ;
            crntPath++ ;
        }
         
        ExifTagsIter crnt = (*crntPathsTags).second.begin() ;
        ExifTagsIter end = (*crntPathsTags).second.end() ;
                
        while( crnt != end )
        {
            ExifTagEntryT<uint16> * tag = (ExifTagEntryT<uint16> *) *(crnt) ;

            exiftag_t  tagnum = tag->getTagNum() ;
            uint16     type   = tag->getType  () ;
            exif_int32 count  = tag->getCount () ;
            int        val    = tag->getValue();

            if (tagnum == EXIFTAG_ORIENTATION)
            {
              rotation = val;
              // for debug :
              //char message[40]="";
              //sprintf(message,"Exif rotation code = %d", rotation);
              //MESSAGE(message);
            }
            crnt++ ;
        }
        crntPathsTags++ ;
    }
            
    // Close the file
    if( inImageFile.close() != EXIF_OK )
      THROW_INTERNAL_ERROR(STR(ERROR_EXIF_CLOSE));
  }

  if (rotation == 6) rotationangle = -90 ; // -90 degree (left)
  if (rotation == 8) rotationangle =  90 ; // 90 degree (right)

  return rotationangle;
}

float getExifFocalLenth35mm(char* filename)
{
  float focal = 0;

  ExifImageFile inImageFile;
  
  // Open the file in read-only mode and verify that it succeeds
  if ( inImageFile.open( filename, "r" ) == EXIF_OK)
  {
    // Get all the AppSeg 1 - "Exif" tags 
    ExifPathsTags app1PathsTags ;
    inImageFile.getAllTags( 0xFFE1, "Exif", app1PathsTags ) ;

    ExifPathsTagsIter crntPathsTags = app1PathsTags.begin() ;
    ExifPathsTagsIter endPathsTags = app1PathsTags.end() ;
    while( crntPathsTags != endPathsTags )
    {
        ExifIFDPathIter crntPath = (*crntPathsTags).first.begin() ;
        ExifIFDPathIter endPath = (*crntPathsTags).first.end() ;
        while( crntPath != endPath )
        {
            exiftag_t ifd = (*crntPath).first  ;
            uint16 idx = (*crntPath).second ;
            crntPath++ ;
        }
         
        ExifTagsIter crnt = (*crntPathsTags).second.begin() ;
        ExifTagsIter end = (*crntPathsTags).second.end() ;
                
        while( crnt != end )
        {
            ExifTagEntryT<uint16> * tag = (ExifTagEntryT<uint16> *) *(crnt) ;

            exiftag_t  tagnum = tag->getTagNum() ;
            uint16     type   = tag->getType  () ;
            exif_int32 count  = tag->getCount () ;
            int        val    = tag->getValue () ;

            if (tagnum == EXIFTAG_FOCALLENGTHIN35MMFILM)
            {
              focal = float(val);
              // for debug :
              //char message[40]="";
              //sprintf(message,"Exif focal = %d mm", focal);
              //MESSAGE(message);
            }
            crnt++ ;
        }
        crntPathsTags++ ;
    }
            
    // Close the file
    if( inImageFile.close() != EXIF_OK )
      THROW_INTERNAL_ERROR(STR(ERROR_EXIF_CLOSE));
  }

  return focal;
}
