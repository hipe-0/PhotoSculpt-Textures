/*
 * BatchSculpt.h
 *
 *  Created on: 24 févr. 2011
  *      Author: Hippolyte Mounier, Copyright 2011 All rights reserved
 */

#pragma once

#include <windows.h>
#include <string.h>
#include <shlobj.h>
#include "Trace.h"
#include "File.h"

bool browseFolderDialog       (char * returnpath);
void searchFolderForSculpt    (TCHAR * path, int mode);
int  getSeconds               (char * filename);
bool batchSculptFilesFromPath (char * currentfilename, int mode);
