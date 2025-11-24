/*
 * Trace.h
 *
 *  Created on: 01 janv. 2011
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */
#pragma once

#include "COMPILER_OPTIONS.h"
#include "Timer.h"

class Trace
{
public:
  Trace(char * functionname, int counter, float * secondcounter);
  ~Trace();

  static void write(char * content);
  static void write(char * content, int content2);
  static void write(char * content, float content2);
  static void write(char * content, char * content2);
  static void cleanFile();

private:
  char        function_[100];
  Timer       timer_;
  float *     secondsCounter_;
};

#ifdef TRACE_TO_FILE
  #define DEBUG(functiontext)                   \
    static int debugcounter__=0               ; \
    static float debugcumulatedtime__ = 0.0f  ; \
    Trace debug__(#functiontext, ++debugcounter__, &debugcumulatedtime__);

  #define DEBUG_WRITE(text)           Trace::write(text)
  #define DEBUG_VALUE(text,integerorfloat) Trace::write(text,integerorfloat)
  #define DEBUG_CLEAN_FILE()               Trace::cleanFile()
  #define DEBUG_GET_COUNTER()         debugcounter__
  #define DEBUG_GET_CUMULATED_TIME()  debugcumulatedtime__

#else
  #define DEBUG(functiontext)       ;
  #define DEBUG_WRITE(text)         ;
  #define DEBUG_VALUE(text,integer) ;
  #define DEBUG_CLEAN_FILE()        ;
  #define DEBUG_GET_COUNTER()         0
  #define DEBUG_GET_CUMULATED_TIME()  0.0f

#endif