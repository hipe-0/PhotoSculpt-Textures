/*
 * Cropper.h
 *
 *  Created on: 29 sept. 2009
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */

// 80 char per line standard :
/////////_/////////_/////////_/////////_/////////_/////////_/////////_/////////_

#ifndef CROPPER_H_
#define CROPPER_H_

#include "IntMap.h"
#include "ImageGeneration.h"
#include "ProgressionBar.h"
#include "BurstMap.h"
#include "Parameter.h"

#define HANDLE_CORNER_NOT_DETECTED 0
#define HANDLE_CORNER_TOP_LEFT     1
#define HANDLE_CORNER_TOP_RIGHT    2
#define HANDLE_CORNER_BOTTOM_LEFT  3
#define HANDLE_CORNER_BOTTOM_RIGHT 4
#define HANDLE_CORNER_ALL          5

class Cropper
{
public:
  Cropper();

  void      defineArea      ( float x1, float y1, float x2, float y2 );
  IntMap    crop            ( const IntMap &m );
  Image     crop            ( const Image &i     );
  void      cancel          ();
  bool      isCrop          () const;

  void      setSquareRectify( bool set, int realSizeOfAreaX, int realSizeOfAreaY );
  bool      isSquare        () const;

  void      set2N           ( bool set );
  bool      is2N            () const;

  void      setSeamless     ( bool set );
  bool      isSeamless      () const;

  void      setPerspective  ( bool set );
  bool      isPerspective   () const;

  void      correct         ();
  void      makeSquare      (float realSizeOfAreaX, float realSizeOfAreaY, int method);

  void      setCropDimensions(float  x1, float y1 , float  x2, float  y2 );
  void      getCropDimensions(float& x1, float& y1, float& x2, float& y2 ) const;
  void      setCropDimensions(float  x1, float y1 , float  x2, float  y2,
                              float  x3, float y3 , float  x4, float  y4);
  void      getCropDimensions(float& x1, float& y1, float& x2, float& y2,
                              float& x3, float& y3, float& x4, float& y4) const;
  float     getWidthRatio    () const;
  
  static Image  reduce2N     (const Image&  i);
  static IntMap reduce2N     (const IntMap& d);

  void      setFrameSize     (float);
  float     getFrameSize     () const;

private:

  // 1 is top left
  // 2 is bottom right
  // 3 is top right
  // 4 is bottom left
  static IntMap crop_(const IntMap &m, int x1, int y1, int x2, int y2);
  static Image  crop_(const Image &i,  int x1, int y1, int x2, int y2);
  static IntMap crop_(const IntMap &m, int x1, int y1, int x2, int y2,
                                       int x3, int y3, int x4, int y4, bool issquare);
  static Image  crop_(const Image &i,  int x1, int y1, int x2, int y2,
                                       int x3, int y3, int x4, int y4, bool issquare);

  // Private members:
  ofloat x1_, y1_, x2_, y2_ ;
  ofloat x3_, y3_, x4_, y4_ ;

  obool  isSquare_          ;
  obool  isMode2n_          ;
  ofloat frameSize_         ;
  obool  isSeamless_        ;
  obool  isPerspective_     ;
};

#endif