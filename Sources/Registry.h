#pragma once
#include <windows.h>
#include "Mutex.h"

class Registry
{
public:
  Registry();
  Registry(char * registrykey);

  void  setRegistryKey(const char * key);

  void  setAllUsers();
  void  setCurrentUser();

  bool  loadDWord  (const char * subkey, DWORD & value);
  void  saveDWord  (const char * subkey, DWORD value);
  bool  loadString (const char * subkey, char * textresult, int maxsize) const;
  void  saveString (const char * subkey, const char * text) const;

private:
  static const int MAX_KEY_SIZE              = 200;
  char             registryKey_[MAX_KEY_SIZE];
  HKEY             base_;
  static Mutex     mutex_;
};
