#include "PSApplication.h"
#include "File.h"


void PSApplication::saveInterfacePosition()
{
  bool isMainInterfaceWindowMaximised = IsZoomed(interface_.windowsHandle_);

  if (!isMainInterfaceWindowMaximised)
  {
    RECT rect;
    bool isok = GetWindowRect(interface_.windowsHandle_, &rect);
    if (!isok)
      THROW_INTERNAL_ERROR("Could not getWindowRect (in saveInterfacePosition())");

    DWORD top    = rect.top    ;
    DWORD bottom = rect.bottom ;
    DWORD left   = rect.left   ;
    DWORD right  = rect.right  ;

    registry_.saveDWord("window top",    top);
    registry_.saveDWord("window bottom", bottom);
    registry_.saveDWord("window left",   left);
    registry_.saveDWord("window right",  right);
  }
}

void PSApplication::saveFileNameInRegistry(const Registry& reg, char * filenameL, char * filenameR)
{
  DEBUG(saveFiNaReg);
  char nameL [9][MAX_PATH];
  char nameR [9][MAX_PATH];

  changePathToBackslash(filenameL);
  changePathToBackslash(filenameR);
  
  for (int i=0; i<NB_RECENT_FILES; i++)
  {
    nameL[i][0] = 0; nameR[i][0] = 0; 
  }

  // flags that tells that our currently loaded images are not found in registry
  int found = -1 ;

  // searches from bottom to top
  for (int i=NB_RECENT_FILES-1; i>=0; i--)
  {
    char keyl[50] = "\0", keyr[50] = "\0" ;
    sprintf(keyl, "last opened left image %d",  i+1);
    sprintf(keyr, "last opened right image %d", i+1);

    bool ok1 = reg.loadString(keyl,nameL[i], MAX_PATH);
    bool ok2 = reg.loadString(keyr,nameR[i], MAX_PATH);

    // check if access to registry was granted
    if (ok1 && ok2)
    {
      changePathToBackslash(nameL[i]);
      changePathToBackslash(nameR[i]);
      bool isSameL= (strcmp(nameL[i], filenameL) == 0);
      bool isSameR= (strcmp(nameR[i], filenameR) == 0);

      // have we found our current images already saved in registry ?
      if (isSameL && isSameR)
        // yes then store the index (0-3)
        found=i;
    }
  }

  // if not in registry already
  if (found == -1) 
  {
    // save the current filenames as first element in registry
    reg.saveString(KEY_LAST_L1, filenameL);
    reg.saveString(KEY_LAST_R1, filenameR);

    // pushes the entire stack (element 0 becomes element 1, 1 -> 2, 2 ->3 ...
    for (int i=0; i<NB_RECENT_FILES-1; i++)
    {
      char keyl[50] = "\0", keyr[50] = "\0" ;
      sprintf(keyl, "last opened left image %d",  i+2);
      sprintf(keyr, "last opened right image %d", i+2);

      reg.saveString(keyl, nameL[i]);
      reg.saveString(keyr, nameR[i]);
    }
  }
  else // was found in registry
  { 
    // found in position other than the first one (=then do nothing)
    if (found>=1)
    {
      // save the current filenames as first element in registry
      reg.saveString(KEY_LAST_L1, filenameL);
      reg.saveString(KEY_LAST_R1, filenameR);

      for (int i=0; i<found; i++)
      {
        char keyl[50] = "\0", keyr[50] = "\0" ;
        sprintf(keyl, "last opened left image %d",  i+2);
        sprintf(keyr, "last opened right image %d", i+2);

        reg.saveString(keyl, nameL[i]);
        reg.saveString(keyr, nameR[i]);
      }
    }
  }
}

bool PSApplication::loadfromRegistryFileName( const Registry& reg,  char * returnLeftImage,
										                                  char * returnRightImage, int index)
{
  DEBUG(loegisFi);
  char keyl[50] = "\0", keyr[50] = "\0" ;
  sprintf(keyl, "last opened left image %d",  index+1);
  sprintf(keyr, "last opened right image %d", index+1);

  bool ok1 = reg.loadString(keyl, returnLeftImage,  MAX_PATH);
  bool ok2 = reg.loadString(keyr, returnRightImage, MAX_PATH);

  return ok1 && ok2;
}

