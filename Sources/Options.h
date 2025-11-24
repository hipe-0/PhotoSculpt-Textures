/*
 * Options.h
 *
 * Created on: 01 jan 2011
 *      Author: Hippolyte Mounier, Copyright 2011 All rights reserved
 */

#pragma once

#include "Registry.h"

class Options
{
public:  
  Options();

  static const int NB_TRIANGLE_LIMIT=10;
  static const int TRIANGLE_LIMITS[10];

  int   nbTriangleDisplay_, nbTriangleIndex_   ;
  int   hue_, jpgPercent_                      ;
  float saturation_                            ;
  bool  isNormalInvertedX_, isNormalInvertedY_ ;
  int   luminosity_                            ;
  float blur_                                  ;
  bool  isExport3DClosed_                      ;
  bool  isPerspectiveCorrected_      ;
  bool  isSoftwareMode_                        ;
  int   nbThreadMax_                           ;
  bool  isSeamCorrectedDiffuse_                ;
  bool  isHalfSpherePreview_                   ;
  int   nbFlatModeSamples_                     ;
  int   interfaceOpacity_                      ;
  bool  isAutoLocalDetail_                     ;
};

extern Options GlobalOptions;

class OptionStore
{
public:
  static void saveToRegistry   (const Options& options, Registry* r);
  static void loadFromRegistry (Options& options, Registry* r);
  static bool launchDialog     (Registry * registry, HINSTANCE inst, HWND hwnd);
};

