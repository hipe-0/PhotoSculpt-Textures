#include "PSApplication.h"
#include "File.h"

// CommandLineToArgvA
// from http://stackoverflow.com/questions/291424/canonical-way-to-parse-the-command-line-into-arguments-in-plain-c-windows-api

#include <assert.h>

LPSTR* CommandLineToArgvA(LPSTR lpCmdLine, INT *pNumArgs)
{
    int retval;
    retval = MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, lpCmdLine, -1, NULL, 0);
    if (!SUCCEEDED(retval))
        return NULL;

    LPWSTR lpWideCharStr = (LPWSTR)malloc(retval * sizeof(WCHAR));
    if (lpWideCharStr == NULL)
        return NULL;

    retval = MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, lpCmdLine, -1, lpWideCharStr, retval);
    if (!SUCCEEDED(retval))
    {
        free(lpWideCharStr);
        return NULL;
    }

    int numArgs;
    LPWSTR* args;
    args = CommandLineToArgvW(lpWideCharStr, &numArgs);
    free(lpWideCharStr);
    if (args == NULL)
        return NULL;

    int storage = numArgs * sizeof(LPSTR);
    for (int i = 0; i < numArgs; ++ i)
    {
        BOOL lpUsedDefaultChar = FALSE;
        retval = WideCharToMultiByte(CP_ACP, 0, args[i], -1, NULL, 0, NULL, &lpUsedDefaultChar);
        if (!SUCCEEDED(retval))
        {
            LocalFree(args);
            return NULL;
        }

        storage += retval;
    }

    LPSTR* result = (LPSTR*)LocalAlloc(LMEM_FIXED, storage);
    if (result == NULL)
    {
        LocalFree(args);
        return NULL;
    }

    int bufLen = storage - numArgs * sizeof(LPSTR);
    LPSTR buffer = ((LPSTR)result) + numArgs * sizeof(LPSTR);
    for (int i = 0; i < numArgs; ++ i)
    {
        assert(bufLen > 0);
        BOOL lpUsedDefaultChar = FALSE;
        retval = WideCharToMultiByte(CP_ACP, 0, args[i], -1, buffer, bufLen, NULL, &lpUsedDefaultChar);
        if (!SUCCEEDED(retval))
        {
            LocalFree(result);
            LocalFree(args);
            return NULL;
        }

        result[i] = buffer;
        bufLen -= retval;
        buffer += retval;
    }

    LocalFree(args);

    *pNumArgs = numArgs;
    return result;
}

// Parse command line arguments
void PSApplication::loadFromCommandLine()
{
  DEBUG(loadFromCom);

  LPTSTR *szArgList               ;
  int argCount                    ;
  int nbimageloaded       = 0     ;
  bool isnextstringoutput = false ;

  szArgList = CommandLineToArgvA(GetCommandLine(), &argCount);

  if (szArgList) 
  {
    for (int i = 1; i < argCount; i++) // skip item number 0 to avoid an error, that's the program itself = useless
    {
      LPTSTR argument = szArgList[i];
      removeQuotes(argument);

      if      (!strcmp ("-sculptatonce", argument))   commandLine_.isForcedSculpt_   = true;
      else if (!strcmp ("-saveallmaps",  argument))   commandLine_.isForcedSaveMaps_ = true;
      else if (!strcmp ("-exit",         argument))   commandLine_.isForcedExit_     = true;
      else if (!strcmp ("-obj",          argument))   commandLine_.isObj_            = true;
      else if (!strcmp ("-ply",          argument))   commandLine_.isPly_            = true;
      else if (!strcmp ("-stl",          argument))   commandLine_.isStl_            = true;
      else if (!strcmp ("-sculpt",       argument))   commandLine_.isSculpt_         = true;
      else if (!strcmp ("-displacement", argument))   commandLine_.isDisplacement_   = true;
      else if (!strcmp ("-normal",       argument))   commandLine_.isNormal_         = true;
      else if (!strcmp ("-specular",     argument))   commandLine_.isSpecular_       = true;
      else if (!strcmp ("-diffuse",      argument))   commandLine_.isDiffuse_        = true;
      else if (!strcmp ("-occlusion",    argument))   commandLine_.isOcclusion_      = true;
      else if (!strcmp ("-bump",         argument))   commandLine_.isBump_           = true;
      else if (!strcmp ("-limdispl",     argument))   commandLine_.isLimDisp_        = true;
      else if (!strcmp ("-alpha",        argument))   commandLine_.isAlpha_          = true;
      else if (!strcmp ("-3d",           argument))   commandLine_.is3D_             = true;

      else if (!strcmp ("-softwaremode", argument))   GlobalOptions.isSoftwareMode_  = true;

      else if (!strcmp ("-maxstep1",     argument))   commandLine_.maxStep_          = 1   ;
      else if (!strcmp ("-maxstep2",     argument))   commandLine_.maxStep_          = 2   ;
      else if (!strcmp ("-maxstep3",     argument))   commandLine_.maxStep_          = 3   ;
      else if (!strcmp ("-maxstep4",     argument))   commandLine_.maxStep_          = 4   ;
      else if (!strcmp ("-maxstep5",     argument))   commandLine_.maxStep_          = 5   ;
      else if (!strcmp ("-maxstep6",     argument))   commandLine_.maxStep_          = 6   ;
      else if (!strcmp ("-maxstep7",     argument))   commandLine_.maxStep_          = 7   ;
      else if (!strcmp ("-maxstep8",     argument))   commandLine_.maxStep_          = 8   ;
      else if (!strcmp ("-maxstep9",     argument))   commandLine_.maxStep_          = 9   ;
      else if (!strcmp ("-maxstep10",    argument))   commandLine_.maxStep_          = 10  ;
      else if (!strcmp ("-maxstep11",    argument))   commandLine_.maxStep_          = 11  ;

      else if (!strcmp ("-squaretile", argument))   commandLine_.isSquareTile_     = true;
      else if (!strcmp ("-2nsize",       argument))   commandLine_.is2nSize_         = true;
      
      else if (!strcmp ("-output",       argument))   isnextstringoutput             = true;
      else if (isnextstringoutput)
        {
          // stores the output path in the commandLine object
          strcpy_s(commandLine_.outputPath_, MAX_PATH, argument);
          isnextstringoutput     = false ;
          commandLine_.isOutput_ = true  ;
        }
      else if (!strcmp ("-minimum",      argument))   commandLine_.isMinimum_        = true;

      else if (isExtension(argument, ".sculpt"))      loadSculptFromCommandLine(argument);

      else if (readImageType(argument) != IMG_TYPE_UNKNOWN)
      {
        if      (nbimageloaded == 0)  strncpy(commandLine_.frontPhotoFileName_, argument, MAX_PATH);
        else if (nbimageloaded == 1)  strncpy(commandLine_.rightPhotoFileName_, argument, MAX_PATH);
        else if (nbimageloaded == 2)  THROW_MESSAGE_TO_USER(MORE_THAN_2_IMAGES);
        nbimageloaded++;
      }
      // if none of this has worked
      else 
        THROW_MESSAGE_TO_USER(INSTRUCTION);
    }

  LocalFree(szArgList);
  }

  if (commandLine_.isForcedSculpt_)
    postActionOnFirstThread(ACTION_COMMANDED_SCULPT);
}

void PSApplication::saveAsPerCommandLine(const CommandLineOrder &command)
{
  DEBUG(saveAsPerCom);
  
  ProgressDialog progress;
  MultiQuery query;

  query.isSaveModel = true;

  query.isStl       = command.isStl_    ;
  query.isObj       = command.isObj_    ;
  query.isPly       = command.isPly_    ;
  query.isSculpt    = command.isSculpt_ ;
  query.isListSizeModel[0] = true; // full size (item 0 in list) is selected
 
  query.isSaveMap   = true;

  query.isListSizeMap  [0] = true; // full size (item 0 in list) is selected
    
  query.nbSizes = 1; // only one map size available, the map number 0

  query.isListMapType  [0] = command.isDiffuse_      ;
  query.isListMapType  [1] = command.isDisplacement_ ;
  query.isListMapType  [2] = command.isNormal_       ;
  query.isListMapType  [3] = command.isOcclusion_    ;
  query.isListMapType  [4] = command.isSpecular_     ;
  query.isListMapType  [5] = command.isAlpha_        ;
  query.isListMapType  [6] = command.isBump_         ;
  query.isListMapType  [7] = command.isLimDisp_      ;
  query.isListMapType  [8] = command.is3D_           ;

  if (command.isForcedSaveMaps_)
    for (int i = 0; i<=7; i++)
      query.isListMapType[i] = true; 

  if (!command.isOutput_)
    saveAsPerMultiQuery(query, sources_.leftFileName_);
  else
    saveAsPerMultiQuery(query, command.outputPath_);
}

CommandLineOrder::CommandLineOrder() 
  { 
    clear();
  }

void CommandLineOrder::clear()
  {
   isForcedSculpt_   = false ;
   isForcedSaveMaps_ = false ;
   isForcedExit_     = false ;

   isObj_            = false ;
   isPly_            = false ;
   isStl_            = false ;
   isSculpt_         = false ;

   isDisplacement_   = false ;
   isNormal_         = false ;
   isSpecular_       = false ;
   isDiffuse_        = false ;
   isOcclusion_      = false ;
   isBump_           = false ;
   isAlpha_          = false ;
   isLimDisp_        = false ;
   is3D_             = false ;

   isOutput_         = false ;

   outputPath_[0]         = '\0'; // clear strings
   frontPhotoFileName_[0] = '\0';
   rightPhotoFileName_[0] = '\0';

   isMinimum_        = false ;
   maxStep_          = 99    ;
   isSquareTile_     = false ;
   is2nSize_         = false ;  
}