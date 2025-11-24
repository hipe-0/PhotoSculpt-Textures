/*
 * ImageTools.h
 *  Created on: 15 juillet 2009
 *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */
#pragma once

#include "VectorMap.h"
#include "FloatMap.h"
#include "Image.h"

class ImageTools
{
public:
  static Image fade(const Image& main, const Image& use, float fadeValue);

  static Image mixOverlay    (const Image& base, const Image& sprite);
  static Image mixHardLight  (const Image& base, const Image& sprite);
  static Image mixMultiply   (const Image& base, const Image& sprite);
  static Image mixMultiply   (const Image& base, float value);
  static Image mixDifference (const Image& base, const Image& sprite);
  static Image mixTint       (const Image& base, int hue);
  static void  drawImageOverlay     (Image& base, const Image& sprite, int x, int y);
  static void  drawImageLinearDodge (Image& base, const Image& sprite, int x, int y);
  static Image mixDesaturateRedChannelOnly(const Image& base, float fade);
  static Image threshold     (const Image& base, int threshold);
  static Image mixOverlay    (const Image& base, const Image& sprite, float fadeValue);
  static Image adjustExposure(const Image& main, const Image& use, 
					    float blurRadius, float fadeValue);

  static Image computeDesaturate(const Image& image);
  static void  repeatN          (Image& texture, int nb, int framesizex, int framesizey);
};
