/*
 * View3D.h
 *
 *  Created on: 16 avr. 2009
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */

#ifndef VIEW3D_H_
#define VIEW3D_H_

#include "Vector3.h"
#include "Image.h"
#include "FloatMap.h"
#include "IntMap.h"
#include "VectorMap.h"
#include "Vector3.h"

Vector3 compute_normal(const VectorMap& d,int x,int y, float zScale);

class View3D
{
public:
  static Image generate3DImage(const IntMap& tablecorresp, Image LeftImage, 
                               float azimuth, float zenith, float zScale);

  static Image generateNormalMapImage(const IntMap& tablecorresp, float zScale, bool isRight, bool isDown);
  //static IntMap revertNormalMapToVectorMap(const Image& image);

  static Vector3 compute_normal(const IntMap& d,int x,int y, float zScale);
  static Vector3 computeNormal(const FloatMap& d,int x,int y);

protected:
  static Vector3 rotationAlongYX(Vector3 v, Vector3 center, float azimuth=0.0, float zenith=0.0);

  static Vector3 rotationAlongYX(Vector3 v,  float azimuth=0.0, float zenith=0.0);

  static Vector3 colorToNormalVector(unsigned char* const normalColor);

  static float shadowSimple(unsigned char* normalColor);

  // Computes a float value between 0.0 (no light) and 1.0 (full light)
  //
  static float shadow(Vector3 normalVector, Vector3 lightVector);
};

#endif /* VIEW3D_H_ */
