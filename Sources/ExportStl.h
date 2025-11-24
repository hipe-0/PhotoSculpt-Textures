/*
 * ExportStl.h
 *
 *  Created on: 05 may 2009
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */

#ifndef EXPORTSTL_H_
#define EXPORTSTL_H_

#include "IntMap.h"
#include "Image.h"

class ExportStl
{
public:

  /// Writes 3D model to disk using Stereolithography format (.Stl)
  // see 
  //        http://en.wikipedia.org/wiki/STL_(file_format)
  //
  //  .Stl file containing geometric data
  //
  static bool exportStlFile (char* fileName, const IntMap& displacementMap,
      const Image& alpha, int skip, float xyScale, float zScale, float focallengthinmm);
};

#endif /* EXPORTSTL_H_ */
