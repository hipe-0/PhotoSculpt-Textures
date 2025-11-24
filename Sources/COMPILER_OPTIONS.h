/* COMPILER_OPTIONS.h
 * Created on: 20 jan 2011
 * Author: Hippolyte Mounier, Copyright 2011 All rights reserved
 */
// 80 char per line standard :
/////////_/////////_/////////_/////////_/////////_/////////_/////////_/////////_
#pragma once

// unused options:
  //#define BUTTON_DEBUG
  //#define THREADID_PROTECTION
  //#define FRENCH_LANGUAGE
  //#define VECTOR_DEBUG
  //#define INITIAL_SCULPT_IS_FLAT

#ifdef _DEBUG    //////// debug mode
  #define OBFUSCATED_VARIABLES_ARE_CLEAR
  #define SHOW_INTERNAL_ERRORS_IN_CLEAR_TEXT
  #define TRACE_TO_FILE
  #define DEBUG_TO_IMAGE_FILES
#else
  #ifdef FINAL_RELEASE //////// release final
    #define TRACE_TO_FILE
    #define IGNORE_ASSERT
  #else                //////// internal release
    #define SHOW_INTERNAL_ERRORS_IN_CLEAR_TEXT
    #define TRACE_TO_FILE
    //#define DEBUG_TO_IMAGE_FILES
    //#define IGNORE_ASSERT
  #endif 
#endif

#define DEBUG_FILE_NAME "photosculptcrashdump.txt"

#define cimg_use_jpeg
#define cimg_use_tiff
#define cimg_use_png

#define cimg_debug 0 // quiet mode, no popup window

