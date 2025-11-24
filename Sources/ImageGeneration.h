/*
 * ImageGeneration.h
 *
 *  Created on: 3 févr. 2009
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */

#ifndef IMAGEGENERATION_H_
#define IMAGEGENERATION_H_

#include "IntMap.h"
#include "VectorMap.h"
#include "FloatMap.h"
#include "Image.h"

class ImageGeneration
{
public:
  static Image  generateThreshold   (const FloatMap & mask, float TresholdValue);

  static Image  generateMask        (const FloatMap & ColorMatchMap, float TresholdValue);

  static Image  generateDepth       (const IntMap&   tablecorresp);
  static Image16  generateDepth16   (const IntMap&   tablecorresp);
  static Image  generateDepth       (const FloatMap& tablecorresp);
  static Image  generateBump        (const IntMap&   tablecorresp);
  static Image  generateDisplace    (const IntMap&   tablecorresp);

  static Image  generateOcclusion   (const IntMap& disp, int radius, float contrast);

  static Image  generateSpecular    (const IntMap& disp,  const Image& image, 
                                     int radius, float contrast);
  
  static Image  generateFakeDepth   (const Image& image);

  static IntMap improveDepthMapWithNormalMap (const Image& map);
  static IntMap loadDepthFromImage16 (const Image16& map);
  static IntMap improveDepthMapWithNormalMap (const Image& map, const IntMap& currentDepthmap);
  static IntMap improveDepthMapWithNormalMap (const Image& map, const Image& normalmap);
};

#endif /* IMAGEGENERATION_H_ */
