/*
 * ExportObj.h
 *
 *  Created on: 16 avr. 2009
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */
#pragma once

#include "IntMap.h"
#include "Image.h"

class ExportObj
{
public:

  /// Writes 3D model to disk using Alias Wavefront format (.obj)
  //  .obj file containing geometric data
  //  .mtl small file containing shader data
  //  .bmp file containing texture data
  //
  static bool exportAllObjFile (char* fileName, const Image& leftImage, const Image& alpha,
      const IntMap& displacementMap, 
      int skip, float xyScale, float zScale, int jpgquality, float focallengthinmm);

  static bool exportMtlFileOnly (char* filename, char* imagefilename);

  // Writes 3D model to disk using Alias Wavefront format (.obj)
  static bool exportObjFileOnly (char* filename, char* material, 
      const IntMap& D, const Image & leftImage, const Image& alpha,
      int skip, float xyscale, float zscale, float focallengthinmm);
};
