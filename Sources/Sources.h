/*
 * Sources.h
 *
 *  Created on: 14 janv. 2011
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */

#pragma once

#include "Image.h"


class Source
{
public:
  Source ();

  void            setAlpha   (PImage alpha);
  PImage          getAlpha   () const      ;
  bool            isAlpha    () const      ;
  float           getEffectiveFocalLength();
  void            rotateImages(float leftimageangle, float centerimageangle, float rightimageangle, float alphaangle);

public:
  Image           leftImage_, rightImage_, additionalImage_ ;
  float           leftFocal_;
  char            leftFileName_[_MAX_PATH], rightFileName_[_MAX_PATH] ;
private:
  PImage          alpha_;
};
