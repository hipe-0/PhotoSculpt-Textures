/*
 * Exposure.cpp
 *
 *  Created on: 15 june 2009
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved

Goal : 
  These methods describes a way to adjust photographs that have a (slight)
  exposure difference
    - globally
    - locally adjusted using the EXPOSURE_BLUR_RATIO

 */

// 80 char per line standard :
/////////_/////////_/////////_/////////_/////////_/////////_/////////_/////////_

#include "PhotoSculpt.h"
#include "Limit.h"
#include "ImageTools.h"
#include "Frame.h"
#include "Exposure.h"
#include "PhotoSculptTools.h"
#include "UserInterrupt.h"

Image PhotoSculpt::generateMorphedRightImage_()
{
  return PhotoSculptTools::generateMorphed(getRightImageUNSTABLE(), *sculptInProgress_.getMap());
}

/// correct the left image so that it matches the (morphed) right image color
// and exposure
//
// parameter EXPOSURE_BLUR_RATIO = changes size of the blur for color change
// parameter EXPOSURE_FADE = increases or decreases intensity of color match
//                           0.0 should be no effect
//                           1.0 is full effect
//
void PhotoSculpt::adjustExposureLocal_()
{
  // get a read only left image
  const Image& left  = getLeftImageUNSTABLE();

  //DEBUG_SAVE_IMAGE(left, "Explocal-left.bmp");

  //  The morphed right image should looks like the left image more or less
  Image morphed = generateMorphedRightImage_() ;

  //DEBUG_SAVE_IMAGE(morphed, "Explocal-morphedright.bmp");

  float  blurradius= left.dimx() / EXPOSURE_BLUR_RATIO ;

  Image newMethod=ImageTools::adjustExposure(left, morphed, blurradius, 
    EXPOSURE_FADE);

  //DEBUG_SAVE_IMAGE(newMethod, "Explocal-leftadjusted.bmp");

  sculptInProgress_.setImageL( new Image(newMethod) );
}

void Exposure::getMinMax(const Image& image, int channel, unsigned char& min, unsigned char& max)
{
  int sizex = image.dimx();
  int sizey = image.dimy();

  unsigned char tempmin = image(0,0,0,channel);
  unsigned char tempmax = tempmin;

  // create picture using the dispmap information
  for   (int y=0;  y<sizey; y++)
    for (int x=0;  x<sizex; x++)
    {
      UserInterrupt::stopIfOrdered();
      unsigned char value = unsigned char ( image(x,y,0,channel) );
      if (value < tempmin ) 
        tempmin = value;

      if (value > tempmax ) 
        tempmax = value;
    }

  const int LIMIT = 50;

  if (tempmin > LIMIT) 
    tempmin = LIMIT;

  if (tempmax < 255 - LIMIT) 
    tempmax = 255 - LIMIT;

  min = unsigned char (tempmin);
  max = unsigned char (tempmax);
}


void Exposure::setMinMax(Image& image, int channel, unsigned char min, unsigned char max)
{
  int sizex = image.dimx();
  int sizey = image.dimy();

  if (min == max) return;

  // create picture using the dispmap information
  for   (int y=0;  y<sizey; y++)
    for (int x=0;  x<sizex; x++)
    {
      UserInterrupt::stopIfOrdered();
      int value = unsigned char ( image(x,y,0,channel) ); // 0...255
      int newvalue = (value - int (min) ) * 255 / ( int(max) - int (min) );
      limit(newvalue, 0, 255);

      image(x,y,0,channel) = (unsigned char) newvalue;
    }
}

void Exposure::autoLevel(Image& image)
{
  unsigned char min,max;
  Exposure::getMinMax(image,0,min,max);
  Exposure::setMinMax(image,0,min,max);
  Exposure::getMinMax(image,1,min,max);
  Exposure::setMinMax(image,1,min,max);
  Exposure::getMinMax(image,2,min,max);
  Exposure::setMinMax(image,2,min,max);
}

Image limit( const Image& image, int maxx, int maxy)
{
  int sizex = image.dimx();
  int sizey = image.dimy();

  Frame::limit(sizex,sizey, maxx, maxy);
  
  return image.get_resize(sizex, sizey);
}

inline int squ(int a) { return a*a; }

float Exposure::percentok(const Image& image1, const Image& image2, int threshold)
{
  int sizex = image1.dimx();   int sizey = image1.dimy();
  int sizex2 = image2.dimx();  int sizey2 = image2.dimy();

  if (sizex!=sizex2 || sizey!=sizey2) 
    THROW_INTERNAL_ERROR("inconsistant image sizes (in distance())");
  if (image1.is_empty() || image2.is_empty())
    THROW_INTERNAL_ERROR("incorrect parameter : empty image (in distance())");

  int count=0;

  // create picture using the dispmap information
  for   (int y=0;  y<sizey; y++)
    for (int x=0;  x<sizex; x++)
    {
      UserInterrupt::stopIfOrdered();
      unsigned char r1 = image1(x,y,0,0);
      unsigned char g1 = image1(x,y,0,1);
      unsigned char b1 = image1(x,y,0,2);
      unsigned char r2 = image2(x,y,0,0);
      unsigned char g2 = image2(x,y,0,1);
      unsigned char b2 = image2(x,y,0,2);

      if (squ(r1-r2) + squ(g1-g2) + squ (b1-b2) < threshold)
        count ++;
    }

  return float(count) / float(sizex) / float(sizey);
}



void PhotoSculpt::adjustExposureGlobal_()
{
  Image left  = getLeftImageUNSTABLE ();
  //DEBUG_SAVE_IMAGE(left, "Exp1L.bmp");
  Exposure::autoLevel(left);
  //DEBUG_SAVE_IMAGE(left, "ExpLAutoadjust.bmp");
  sculptInProgress_.setImageL( new Image(left)  );

  Image right = getRightImageUNSTABLE ();
  //DEBUG_SAVE_IMAGE(right,"Exp2R.bmp");
  Exposure::autoLevel(right);

  //DEBUG_SAVE_IMAGE(right,"Exp3RAutoadjust.bmp");
  sculptInProgress_.setImageR( new Image(right) );
  right = Image(); // We're done with this, free memory

  Image morphed = generateMorphedRightImage_() ;
 //DEBUG_SAVE_IMAGE(morphed,"ExpLAdjustedCompareTo.bmp");

  Image tmorphed = limit(morphed, 50,50);
  //DEBUG_SAVE_IMAGE(tmorphed,"Exp5thumbmorphed.bmp");
  Image tleft= left.get_resize(tmorphed.dimx(), tmorphed.dimy());
  //DEBUG_SAVE_IMAGE(tleft,"Exp6tleft.bmp");

  // initialisation values
  unsigned char iminr, imaxr;
  unsigned char iming, imaxg;
  unsigned char iminb, imaxb;

  Exposure::getMinMax(tleft,0, iminr, imaxr);
  Exposure::getMinMax(tleft,1, iming, imaxg);
  Exposure::getMinMax(tleft,2, iminb, imaxb);

  // best values so far, to be improved
  int bminr=iminr, bmaxr=imaxr;
  int bming=iming, bmaxg=imaxg;
  int bminb=iminb, bmaxb=imaxb;
  const int threshold = 100;

  Image templeft = tleft;
  Exposure::setMinMax(templeft,0, bminr, bmaxr);
  Exposure::setMinMax(templeft,1, bming, bmaxg);
  Exposure::setMinMax(templeft,2, bminb, bmaxb);
  //DEBUG_SAVE_IMAGE(templeft,"Exp7templeft.bmp");

  float bestcurrent = Exposure::percentok(templeft, tmorphed, threshold);

  for (int i=0; i<50; i++)
  {
    templeft = tleft ;
    const int R=11, S=5;
    int minr = bminr + rand()%R-S, maxr=bmaxr + rand()%R-S;
    int ming = bming + rand()%R-S, maxg=bmaxg + rand()%R-S;
    int minb = bminb + rand()%R-S, maxb=bmaxb + rand()%R-S;

    Exposure::setMinMax(templeft,0 ,minr, maxr);
    Exposure::setMinMax(templeft,1, ming, maxg);
    Exposure::setMinMax(templeft,2, minb, maxb);

    float current = Exposure::percentok(templeft, tmorphed, threshold);

    if (current > bestcurrent)
    {
      //DEBUG_SAVE_IMAGE(templeft,"Exp8thumbleft.bmp");
      bestcurrent = current;
      bminr = minr; bmaxr = maxr;
      bming = ming; bmaxg = maxg;
      bminb = minb; bmaxb = maxb;
    }
  }

  Exposure::setMinMax(left,0, bminr, bmaxr);
  Exposure::setMinMax(left,1, bming, bmaxg);
  Exposure::setMinMax(left,2, bminb, bmaxb);

  //DEBUG_SAVE_IMAGE(left,"ExpLAdjusted.bmp");

  sculptInProgress_.setImageL( new Image(left) );
}


