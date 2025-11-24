/*
 * Image.h
 *
 *  Created on: 06 fev. 2011
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */

#pragma once

#include "COMPILER_OPTIONS.h"
#include <CImg.h>
 
#include "SmartPointer.h"

using namespace cimg_library;

typedef CImg<unsigned char>         Image    ;
typedef CImg<unsigned short int>    Image16  ;
typedef SmartPointer<Image,false>   PImage   ;
typedef SmartPointer<Image16,false> PImage16 ;

#define ANY                               -100
#define CIMG_NO_INTERPOLATION             -1
#define CIMG_NO_INTERPOLATION_FILLED      0
#define CIMG_INTERPOLATION_NEAREST_POINT  1
#define CIMG_INTERPOLATION_MOVING_AVERAGE 2
#define CIMG_INTERPOLATION_LINEAR         3
#define CIMG_INTERPOLATION_GRID           4
#define CIMG_INTERPOLATION_BICUBIC        5

#ifdef DEBUG_TO_IMAGE_FILES
    #define DEBUG_SAVE_IMAGE(image, imagename) \
      {                                        \
        char path[_MAX_PATH] = "";             \
        GetTempPath( _MAX_PATH, path );        \
        strcat(path, (imagename));             \
        (image).save(path);                    \
      }
#else
  #define DEBUG_SAVE_IMAGE(image, imagename) {}
#endif

#define SAVE_IMAGE(image, imagename)     \
{                                        \
  char path[_MAX_PATH] = "";             \
  GetTempPath( _MAX_PATH, path );        \
  strcat(path, (imagename));             \
  (image).save(path);                    \
}
