/*
 * Error.h
 *
 *  Created on: 18 avr. 2009
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */
#pragma once

#include <stdexcept>
#include <windows.h>
#include "RESOURCE.H"
#include "COMPILER_OPTIONS.h"
#include "Trace.h"
#include "STRINGS.h"

#define TXT_OUT_OF_MEMORY \
  STR(ERROR_OUT_OF_MEMORY)

#define ERROR_FLAGS (MB_OK | MB_ICONQUESTION)

#define MESSAGE(text)     {DEBUG_WRITE(text); MessageBox(0,text, STR(ERROR_MESSAGE), ERROR_FLAGS); }
#define MESSAGEHWND(text) {DEBUG_WRITE(text); MessageBox(interface_.windowsHandle_,text, STR(ERROR_MESSAGE), ERROR_FLAGS); }

#define CATCH_BLOC(ErrorText)                              \
    catch (CImgException &e)                               \
    {                                                      \
      MessageBox(0,e.message, ErrorText, ERROR_FLAGS );    \
    }                                                      \
    catch (std::bad_alloc& )                               \
    {                                                      \
      MessageBox(0,TXT_OUT_OF_MEMORY,                      \
        ErrorText, MB_OK | MB_ICONQUESTION );              \
    }                                                      \
    catch (std::exception& e)                              \
    {                                                      \
      MessageBox(0,e.what(), ErrorText, ERROR_FLAGS );     \
    }                                                      \
    catch (char * txt)                                     \
    {                                                      \
      MessageBox(0,txt,  ErrorText, ERROR_FLAGS );         \
    }

#define INTERFACE_CATCH_BLOC(ErrorText)                    \
    catch (CImgException &e)                               \
    {                                                      \
      MessageBox(interface_.windowsHandle_,e.message, STR(ERROR_##ErrorText), ERROR_FLAGS );  \
    }                                                      \
    catch (std::bad_alloc& )                               \
    {                                                      \
      MessageBox(interface_.windowsHandle_,TXT_OUT_OF_MEMORY, STR(ERROR_##ErrorText), ERROR_FLAGS );\
    }                                                      \
    catch (std::exception& e)                              \
    {                                                      \
      MessageBox(interface_.windowsHandle_,e.what(), STR(ERROR_##ErrorText), ERROR_FLAGS ); \
    }                                                      \
    catch (char * txt)                                     \
    {                                                      \
      MessageBox(interface_.windowsHandle_,txt,  STR(ERROR_##ErrorText), ERROR_FLAGS );    \
    }

#ifdef SHOW_INTERNAL_ERRORS_IN_CLEAR_TEXT
  #define THROW_INTERNAL_ERROR(msg)  {DEBUG_WRITE(msg); throw msg;}
  #ifndef IGNORE_ASSERTS
    #define ASSERT(cond, msg)      {if (cond) { DEBUG_WRITE(msg); throw msg;}}
    #define ASSERTMUTEX(cond, msg) {if (cond) { MUTEXRELEASE; DEBUG_WRITE(msg); throw msg;}}
  #else
    #define ASSERT(cond, msg)      ;
    #define ASSERTMUTEX(cond, msg) ;
  #endif

#else
  #define THROW_INTERNAL_ERROR(msg) {throw STR(ERROR_STOPPED_ACTION);}
  #ifndef IGNORE_ASSERTS
    #define ASSERT(cond, msg)      {if (cond) { DEBUG_WRITE(STR(ERROR_STOPPED_ACTION)); throw STR(ERROR_STOPPED_ACTION);}}
    #define ASSERTMUTEX(cond, msg) {if (cond) { DEBUG_WRITE(STR(ERROR_STOPPED_ACTION)); MUTEXRELEASE; throw STR(ERROR_STOPPED_ACTION);}}
  #else
    #define ASSERT(cond, msg)      ;
    #define ASSERTMUTEX(cond, msg) ;
  #endif
#endif

#define THROW_MESSAGE_TO_USER(error) { DEBUG_WRITE(STR(ERROR_##error)); \
                                       throw STR(ERROR_##error);   }

#define THROW_MEMORY_LIMIT_REACHED()     \
  throw TXT_OUT_OF_MEMORY
