/* ManualMode.h
 *
 *  Created on: 15 avr. 2009
 *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */
#pragma once

#include "VectorMap.h"
#include "PhotoSculpt.h"
#include "ButtonManager.h"

class ManualEditInterface
{
public:
  ManualEditInterface(PhotoSculpt& mix);

  VectorMap launch();

  void displayGrid(Image& i,const VectorMap& D,int Skip, const unsigned char* linecolorleft,
                   int x=0,int y=0);

  void createOverlayImage(Image& i,const Image& L,const Image& R,const VectorMap& D,
                          int coordx, int coordy);

  void static initialize(HINSTANCE h);

  void close();

private:
  void draw_();

  VectorMap    editedMap_;
  PhotoSculpt* sculpt_;

  int   inputMapLevel_;
  int   inputMapSizeX_,inputMapSizeY_;
  int   editedMapLevel_;

  Image editedLeftImage_, editedRightImage_;
  int   editedMapSizeX_,  editedMapSizeY_;
  int   outputMapSizeX_,  outputMapSizeY_;

  int   skip_;
  unsigned char linecolorleft[3];

  Image background_;
  int   bandSize_;
  int   xoffset_, yoffset_;

  int   windowSizeX_, windowSizeY_;

  Image initialInterfaceImage_;
  Image imageBuffer_;

  CImgDisplay window_;

  static ButtonManager button_;
};

