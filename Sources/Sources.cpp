/*
 * Sources.cpp
 *
 *  Created on: 14 janv. 2011
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */
#include "Sources.h"
#include "Error.h"

Source::Source ()           
{ 
  alpha_ = new Image(); 
}

void   Source::setAlpha(PImage alpha)    
{ 
  alpha_ = alpha; 
}

PImage Source::getAlpha() const
{ 
  return alpha_;  
}

bool Source::isAlpha() const
{
  return !alpha_->is_empty();
}

#define DEFAULT_FOCAL_LENGTH 50.0f

float Source::getEffectiveFocalLength()
{
  if (leftImage_.is_empty())
    return DEFAULT_FOCAL_LENGTH;

  float focallength = leftFocal_;
  if (leftFocal_ <= 5.0f) 
    focallength = DEFAULT_FOCAL_LENGTH;

  if (leftImage_.dimx() < leftImage_.dimy()) // is left image in portrait mode?
    // increase focal length as the width of the image is a croped version of the full-frame
    return focallength * leftImage_.dimy() / leftImage_.dimx() ; 
  else
    return focallength;
}

void Source::rotateImages(float leftimageangle, float centerimageangle, 
                          float rightimageangle, float alphaangle)
{
  if (additionalImage_ != 0.0f) additionalImage_.rotate (leftimageangle  );

  if (centerimageangle != 0.0f) leftImage_      .rotate (centerimageangle);

  if (rightimageangle  != 0.0f) rightImage_     .rotate (rightimageangle );

  if (alphaangle       != 0.0f) alpha_          -> rotate (alphaangle );
}
