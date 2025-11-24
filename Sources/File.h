/*
 * File.h
 *
 *  Created on: 1st december 2009
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */

#pragma once

// converts a file name from windows to unix
inline void changePathToSlash(char * szFile)
{
  for( unsigned int i=0; i < strlen(szFile); i++ )
    if (szFile[i] == 92)
      szFile[i] = 47; // Replace '\' with '/'.
}

// converts a file name from unix to windows
inline void changePathToBackslash(char * szFile)
{
  for( unsigned int i=0; i < strlen(szFile); i++ )
    if (szFile[i] == 47)
      szFile[i] = 92; // Replace '/' with '\'.
}

inline bool isFileExist(char * filename)
{
   WIN32_FIND_DATA FindFileData;
   HANDLE hFind;

   hFind = FindFirstFile(filename, &FindFileData);
   if (hFind == INVALID_HANDLE_VALUE) 
     return false;

   FindClose(hFind);
   return true;
}

inline void removeQuotes(char * string)
{
  int length = strlen(string);
  char * buffer = new char[length+1];

  if (buffer == NULL)
    THROW_MEMORY_LIMIT_REACHED();

  char * ptr1 , *ptr2 = buffer;
  for ( ptr1 = string; *ptr1 != 0 ; ptr1++)
    if (*ptr1 != '\"')
    {
      *ptr2 = *ptr1;
      ptr2++;
    }
  *ptr2 = 0;

  int l1 = strlen(buffer);
  strcpy(string , buffer);
  delete buffer;
}

#define IMG_TYPE_UNKNOWN -1
#define IMG_TYPE_JPG 0
#define IMG_TYPE_BMP 1
#define IMG_TYPE_TIF 2
#define IMG_TYPE_PNG 3

inline int readImageType(char * FileName)
{
  // searches if the end of the string is jpg or bmp
  char * extension = strrchr(FileName, '.');

  if (extension==0)    return IMG_TYPE_UNKNOWN;

  if (strcmp(extension, ".bmp" )==0 || strcmp(extension, ".Bmp" )==0 ||
      strcmp(extension, ".BMP" )==0)      return IMG_TYPE_BMP;

  if (strcmp(extension, ".jpg" )==0 || strcmp(extension, ".JPG" )==0 ||
      strcmp(extension, ".Jpg" )==0 || strcmp(extension, ".JPEG")==0 ||
      strcmp(extension, ".jpeg")==0)      return IMG_TYPE_JPG;

  if (strcmp(extension, ".tif" )==0 || strcmp(extension, ".Tif" )==0 ||
      strcmp(extension, ".TIF" )==0 || strcmp(extension, ".tiff")==0 ||
      strcmp(extension, ".Tiff")==0 ||
      strcmp(extension, ".TIFF")==0 )     return IMG_TYPE_TIF;

  if (strcmp(extension, ".png" )==0 || strcmp(extension, ".Png" )==0 ||
      strcmp(extension, ".PNG" )==0 )     return IMG_TYPE_PNG;

  return IMG_TYPE_UNKNOWN;
}

inline void convertUpperCase(char * str)
{
  int i = 0;
  while (str[i])
  {
    str[i]=toupper(str[i]);
    i++;
  }
}
inline void convertLowerCase(char * str)
{
  int i = 0;
  while (str[i])
  {
    str[i]=tolower(str[i]);
    i++;
  }
}

inline bool extractExtension (char * dest, int buffersize, const char * filename)
{
  const char * extension = strrchr (filename, '.');

  if (extension==0)
    return false; // no extension was found

  if (strlen(extension)+1 > unsigned int (buffersize) )
    return false;

  strcpy_s(dest, buffersize, extension);
  return true;
}

inline bool isExtension(char * filename, char * extensionToCheck)
{
  char extension[MAX_PATH]="";
  bool isok = extractExtension(extension, MAX_PATH, filename);
  if (!isok)
    return false ;  // no extension was found
  if (strlen(extension) >9) // an extension > 9 char is no good
    return false ;

  convertUpperCase(extension);
  bool isEqualUpper = ( strcmp(extension,extensionToCheck) == 0 );
  convertLowerCase(extension);
  bool isEqualLower = ( strcmp(extension,extensionToCheck) == 0 );

  if (isEqualUpper || isEqualLower)
    return true;
  else
    return false;
}

inline void removeExtension (char * filename)
{
  char * extension = strrchr(filename, '.');
  char * backslash = strrchr(filename, '\\');

  if (backslash>extension) 
    return; // no extension was found

  if (strlen(extension) <= 9U) // 9 characters for an extension is too much
    if (extension!=0)
      extension[0]='\0';
}

inline char * pointToFileNameOnly(char * filename)
{
  char * extension = strrchr(filename, '\\');
  if (extension!=0) return extension+1;

  extension = strrchr(filename, '/');
  if (extension!=0) return extension+1;

  return filename;
}
