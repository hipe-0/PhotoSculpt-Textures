/*
 * ExportPly.h
 *
 *  Created on: 23 feb. 2011
  *      Author: Hippolyte Mounier, Copyright 2011 All rights reserved
 */
#pragma once

#include "IntMap.h"
#include "Image.h"

class ExportPly
{
public:

  /// Writes 3D model to disk using Ply format

  static bool exportFile (char* fileName, const Image& leftImage, 
      const Image& alpha, const IntMap& displacementMap, 
      int skip, float xyScale, float zScale, float focallengthinmm);
};
