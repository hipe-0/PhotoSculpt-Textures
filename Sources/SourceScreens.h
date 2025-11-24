/*
 * Edit.h
 *
 *  Created on: 30 sept. 2009
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */

#pragma once
#include "VectorMap.h"
#include "Image.h"

class SourceScreens
{
public:
  SourceScreens();
  bool isCursorInImageL(int x, int y, float& coordx, float& coordy);
  bool isCursorInImageR(int x, int y, float& coordx, float& coordy);
  void convertCoordToImageL(float coordx, float coordy, int& x, int& y);
  void convertCoordToImageR(float coordx, float coordy, int& x, int& y);
  void correlateLtoR(float  coordlx, float  coordly, 
                     float& coordrx, float& coordry, const VectorMap& m);

  void drawPointL(float coordx, float coordy,   Image& i, unsigned char color[3]);
  void drawPointR(float coordx, float coordy,   Image& i, unsigned char color[3]);
  void drawLine  (float coordlx, float coordly, 
                  float coordrx, float coordry, Image& i, unsigned char color[3], float opacity);

  void setL(int xl, int yl, int sxl, int syl);
  void setR(int xr, int yr, int sxr, int syr);

  static int   FtoI_ ( float valuefloat , int size);
  static float ItoF_ ( int valueint , int size);

private:
  int xl_, yl_, xr_, yr_;
  int sxl_, syl_, sxr_, syr_;
};

