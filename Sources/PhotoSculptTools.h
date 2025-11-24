/*
 * PhotoSculptTools.h
 *
 *  Created on: 5 oct. 2009
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */

// 80 char per line standard :
/////////_/////////_/////////_/////////_/////////_/////////_/////////_/////////_

#ifndef PHOTOSCULPTTOOLS_H_
#define PHOTOSCULPTTOOLS_H_

#include "IntMap.h"
#include "VectorMap.h"
#include "IntMap.h"
#include "ImageGeneration.h"
#include "ProgressionBar.h"
#include "BurstMap.h"
#include "Parameter.h"
#include "EditCoordinates.h"
#include "SmartPointer.h"
#include "Color.h"

class PhotoSculptTools
{
public:
  static float      comparecolor    (ColorF pixA,ColorF pixB);

  static VectorMap  applyDisplace   (const Image& image, const VectorMap & map, 
                                    float strength, float midlevel);
  static IntMap     applyDisplace   (const Image& image, const IntMap & map, 
                                    float strength, float midlevel);

  static PFloatMap  computeEdgeDetection (const Image & image, int range);
  static FloatMap   computeEdgeDetectionX(const Image & image,  int range);
  static FloatMap   computeEdgeDetectionY(const Image & image,  int range);

  static PVectorMap smartBlur      (PVectorMap& map, const PImage& leftImage, 
                                    float threshold, int range);

  static VectorMap  blurMaskedMap   (const VectorMap& map, const FloatMap& mask, 
                                    int Skip, int range,
								                    float MaxColorMatchValue);

  static IntMap     blurMap         (const IntMap& map, int range);
  static VectorMap  blurMap         (const VectorMap& map, int range);

  static PVectorMap blurMapYOnly    (PVectorMap& map, int range);

  static VectorMap  reInterpolateMap(const VectorMap & map, int step);
  static void       reInterpolateMap(PVectorMap & map, int step);

  static FloatMap   computeValueMap (const Image& left, const Image& right, 
                                    const VectorMap & map, 
                                    int pixRange,float maxColorMatchValue);

  static Image      areaFill        (const Image& left);
  static Image      generateMorphed (const Image& right, const VectorMap & map, 
                                    bool extend = true);
  static Image      generateMorphedLeft(const Image& left, const VectorMap & map, 
                                    bool extend);
  static void       mixByAlpha      (PIntMap    original, const PIntMap modification,    
                                    const PImage alpha);
};



/// Compares 2 color values using the sum of square distance
//
inline float PhotoSculptTools::comparecolor(ColorF pixA, ColorF pixB)
{
  // Compares rvb colors of pixel using the formula
  //  compare pixel= deltaR*deltaR+deltaV*deltaV+deltaB*deltaB
  //  that's the square of the distance between the 2 colors
  //
  // info : it is assumed that pixA and pixB contain only values between
  //        0.0 and 1.0
  // info : the lower the value, the closest the match
  // info : values are in theory between 0.0 (100% color match) and 3.0
  //        (black against white)
  // info : acceptable match values are like 0.1 I think
  // info : smallest value element is (1/255)^2 = 0.0000153

  return (pixA.r-pixB.r)*(pixA.r-pixB.r) +
         (pixA.g-pixB.g)*(pixA.g-pixB.g) +
         (pixA.b-pixB.b)*(pixA.b-pixB.b);
}


#endif