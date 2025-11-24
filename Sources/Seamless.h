/*
 * Seamless.h
 *
 *  Created on: 29 sept. 2009
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */

// 80 char per line standard :
/////////_/////////_/////////_/////////_/////////_/////////_/////////_/////////_

#ifndef SEAMLESS_H_
#define SEAMLESS_H_

#include "IntMap.h"
#include "Image.h"

class Seamless
{
public:
  static IntMap computeFlattenMap               (const IntMap& intmap, int nbsamples);
  static IntMap computeBorderedMap              (const IntMap& intmap,
                                                 int singleBorderX, int singleBorderY, float zscale);
  static IntMap computeSeamlessMapFormBorderedUncroped  (const IntMap & bordered,
                                                 int cropx, int cropy);
  static IntMap cropTextureMap(const IntMap & intmap, int cropx, int cropy);

  static Image  computeSeamlessImageFromBordered(const IntMap & bordered,
                                                 const Image & image,
                                                 int bordersizeX, int bordersizeY);

  static Image  computeSeamlessMaskFromBordered (const IntMap & bordered,
                                                 int singleBorderX, int singleBorderY);
};

#endif