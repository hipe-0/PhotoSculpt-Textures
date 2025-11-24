#pragma once

#include "SecuredSections.h"

#include <windows.h> /* For declaration of GetEnvironmentVariable() */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


inline bool getArmadilloUserName(char* name) 
{
  *name=0; // empty the name string

  // Check the ALTUSERNAME; it should always be set if the program is protected
  if (GetEnvironmentVariable("ALTUSERNAME", name, 255) == 0) 
    return false;

  if (!_stricmp(name, "DEFAULT")) 
  { //Program is using the default key; no username or key is available./
    *name = 0; // empty the name string
    return false;
  }

  return true;
}

inline bool checkArmadilloProtection()
{
  char name[256]="";
  // Check the ALTUSERNAME; it should always be set if the program is protected
  return GetEnvironmentVariable("ALTUSERNAME", name, 256) != 0;
}

inline bool getArmadilloKey(char* key) 
{
  *key=0; // empty the key string
  char name[256]="";

  // Check the ALTUSERNAME; it should always be set if the program is protected
  if (!checkArmadilloProtection())
    return false;

  if (GetEnvironmentVariable("USERKEY", key, 255 ) == 0) 
   strcpy_s(key, 256, "No key!");

  if (!_stricmp(name, "DEFAULT")) 
  { //Program is using the default key; no username or key is available.
    *key = 0; // empty the key string
    return false;
  }

  return true;
}
