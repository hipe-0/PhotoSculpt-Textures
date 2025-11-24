/*
 * Registry.cpp
 *
 *  Created on: 28 august 2009
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */
 
#include "RESOURCE.H"
#include "Registry.h"
#include "string.h"
#include "Includes.h"

Mutex  Registry::mutex_;

Registry::Registry()
{
  registryKey_[0] = '\0';
  base_ = HKEY_CURRENT_USER;
}

Registry::Registry(char * registrykey)
{
  setRegistryKey(registrykey);
  base_ = HKEY_CURRENT_USER;
} 

void  Registry::setAllUsers()
{
  base_ = HKEY_LOCAL_MACHINE;
}
void  Registry::setCurrentUser()
{
  base_ = HKEY_CURRENT_USER;
}

void Registry::setRegistryKey(const char * registrykey) 
{
  if (strlen(registrykey)>MAX_KEY_SIZE-1)
    THROW_INTERNAL_ERROR( "Registry key length exceeds max size in setRegistryKey" );

  strcpy_s(registryKey_, MAX_KEY_SIZE, registrykey);
}

bool Registry::loadDWord (const char * keystring, DWORD & value)
{
  DEBUG(ladDod);
  if (registryKey_[0] == '\0')
    THROW_INTERNAL_ERROR( "Registry key not initialised in loadDWord" );

  DWORD dwDisposition;
  HKEY hkey;

  DWORD isfail = RegCreateKeyEx(base_, 
    LPCTSTR(registryKey_), 
    0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hkey, &dwDisposition);

  DEBUG_VALUE("idfail", int(isfail));
  if (isfail!=ERROR_SUCCESS)
    THROW_INTERNAL_ERROR( "Windows could not give write access to registry in loadDWord" );

  DWORD type, size = sizeof (DWORD);

  DWORD isfail1 = RegQueryValueEx(hkey, LPCTSTR (keystring),    0, &type, (BYTE*) (&value)    , &(size));

  RegCloseKey(hkey);

  if ( isfail1 != ERROR_SUCCESS )
    return false;

  if ( type != REG_DWORD )
    THROW_INTERNAL_ERROR( "Registry did not returned an integer type in loadDWord" );

  return true;
}

void Registry::saveDWord (const char * keystring, DWORD value)
{
  MUTEX;
  
  DEBUG(sveWod);
  if (registryKey_[0] == '\0')
    THROW_INTERNAL_ERROR( "Registry key not initialised in saveDWord" );

  DWORD dwDisposition;
  HKEY hkey;

  DWORD isfail = RegCreateKeyEx(base_, 
                                LPCTSTR (registryKey_), 
                                0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,
                                NULL, &hkey, 
                                &dwDisposition);
  DEBUG_VALUE("  idfail", int(isfail));

  if ( isfail != ERROR_SUCCESS ) 
  {
    MUTEXRELEASE;
    THROW_INTERNAL_ERROR( "Windows could not give write access to registry in saveDWord" );
  }

  DWORD Dvalue = value;

  // write to registry
  DWORD isfail2 = RegSetValueEx(hkey, LPCTSTR (keystring), 0, REG_DWORD , (BYTE*) (&Dvalue), sizeof(Dvalue));
  DEBUG_VALUE("  idfail2", int(isfail2));

  RegCloseKey(hkey);
  
  MUTEXRELEASE;

  if ( isfail2 != ERROR_SUCCESS ) 
    THROW_INTERNAL_ERROR( "Windows could not give write access to registry in saveDWord" );
}

bool Registry::loadString(const char * keystring, char * textreturn, int maxsize) const
{
  MUTEX;
  
  DEBUG(lodSrng);
  char key[200]="";
  strcpy_s(key, 200, keystring);
  DEBUG_VALUE("  key ", key);

  if (registryKey_[0] == '\0')
  {
    MUTEXRELEASE;
    THROW_INTERNAL_ERROR( "Registry key not initialised in loadString" );
  }

  DWORD dwDisposition;
  HKEY hkey;

  DWORD isfail = RegCreateKeyEx(base_, 
                                LPCTSTR (registryKey_),
                                0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,
                                NULL, &hkey, 
                                &dwDisposition);
 
  DEBUG_VALUE("  idfail", int(isfail));
  if (isfail != ERROR_SUCCESS)
  {
    char msg[200]="";
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, isfail, 0, msg, 200, 0);
    DEBUG_VALUE("!!! error with key ", key);
    DEBUG_VALUE("!!! error ", msg);

    MUTEXRELEASE;
    THROW_INTERNAL_ERROR( "Windows could not give access to registry in loadString" );
  }

  DWORD type, size=maxsize;

  DWORD isfail2 = RegQueryValueEx(hkey, LPCTSTR (keystring),
	                              0,
								                &type,
								                (BYTE*) (textreturn),
								                &(size));

  RegCloseKey(hkey);
  MUTEXRELEASE;

  DEBUG_VALUE("  idfail2", int(isfail2));

  if (isfail2 != ERROR_SUCCESS)
  {
    // Clears both strings : 
	  // writes a zero in each string, clearing the "null terminated" C style strings
	  (*textreturn)  = 0;
    return false;
  }

  if (type != REG_SZ) // info REG_SZ=1
    THROW_INTERNAL_ERROR( "Registry did not returned a string type in loadString" );
 
  if ( size > DWORD (maxsize) ) 
    THROW_INTERNAL_ERROR( "Could not load image due to file path too long in loadString" );

  return true;
}

void Registry::saveString(const char * keystring, const char * text) const
{
  DEBUG(sveSin);
  if (registryKey_[0] == '\0')
    THROW_INTERNAL_ERROR( "Registry key not initialised in saveString" );

  DWORD dwDisposition;
  HKEY hkey;

  DWORD isfail = RegCreateKeyEx(base_, 
                                LPCTSTR (registryKey_), 
                                0, NULL, 
								                REG_OPTION_NON_VOLATILE, 
								                KEY_ALL_ACCESS, NULL, 
								                &hkey, &dwDisposition);

  DEBUG_VALUE("  idfail", int(isfail));
  if (isfail!=ERROR_SUCCESS) 
    THROW_INTERNAL_ERROR( "Windows could not give write access to registry in saveString" );
  
  // write string to registry
  DWORD isfail2 = RegSetValueEx(hkey, LPCTSTR (keystring)    , 0, REG_SZ , 
	                       (BYTE*) (text), 
	                       DWORD(strlen(text) + 1)                  );

	RegCloseKey(hkey);

  if (isfail2!=ERROR_SUCCESS) 
    THROW_INTERNAL_ERROR( "Windows could not give write access to registry in saveString" );
  DEBUG_VALUE("  idfail2", int(isfail2));
}

