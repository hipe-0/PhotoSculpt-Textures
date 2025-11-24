/*
 * GetScore.cpp
 *
 *  Created on: 22 avr. 2009
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */

// 80 char per line standard :
/////////_/////////_/////////_/////////_/////////_/////////_/////////_/////////_

#include "PhotoSculpt.h"
#include "PhotoSculptTools.h"
#include "UserInterrupt.h"
#include "Includes.h"


/// Compute some sort of score of how good the displace map is,
// regarding to the stereo images
//
// note : Lower score is better
//
float PhotoSculpt::getScore()
{
  // Reads map size
  int sizex = currentMapStable_->getSizeX();
  int sizey = currentMapStable_->getSizeY();
  ASSERT (sizex == 0 || sizey == 0, "Internal error : map is empty (getScore())" );

  const Image& left  = computeLeftImage ();
  const Image& right = computeRightImage();

  ASSERT (sizex != left .dimx()  || sizey != left .dimy()  ||
          sizex != right.dimx()  || sizey != right.dimy(),
          "Internal error : map has incorrect size (getScore())" );

  // init quality score to 0.0, info : lower score is better
  float qualityscore = 0.0;
  long int nbpixel   = 0;

  // reads the picture using a central rectangular crop of 60% x 60%
  for (int y=sizey*2/10;y<sizey*8/10;y++)

    for (int x=sizex*2/10;x<sizex*8/10;x++)
    {
      nbpixel++;
      Vector D = currentMapStable_->get(x,y) / MULTIPLY;

      bool isInsideOfMap=(0<=x+D.x() && x+D.x()<sizex &&
                          0<=y+D.y() && y+D.y()<sizey);

      if (isInsideOfMap)
      {
        //get the color of the pixel from the real right hand map
        ColorF colorL=getPixelF(left,x,y);

        //get the color of the pixel from the diplaced left hand map
        ColorF colorR=getPixelF(right,x+D.x(),y+D.y());

        qualityscore+=PhotoSculptTools::comparecolor(colorL,colorR);
      }
      else
        qualityscore+=.5;
    }

  return qualityscore/nbpixel; // to get a "per pixel value"
}


/// Compute the percentage of good pixels ( that match left to right image)
//
// that's a way to compute quality
//
// note : high percent score is better
//
float PhotoSculpt::getGoodPixelPercent(float qualityThreshold)
{
  // Reads map size
  int sizex = currentMapStable_->getSizeX();
  int sizey = currentMapStable_->getSizeY();
  ASSERT (sizex == 0 || sizey == 0, "Internal error : map is empty (getGoodPixelPercent())" );

  const Image& left  = computeLeftImage  ();
  const Image& right = computeRightImage ();
    
  ASSERT (sizex != left .dimx()  || sizey != left .dimy()  ||
          sizex != right.dimx()  || sizey != right.dimy(), 
          "Internal error : map has incorrect size (getScore())" );

  long int goodPixel = 0;
  long int totalPixel= 0;

  // reads the picture using a rectangular crop of 60% x 60%
  for (  int y=sizey*2/10;   y<sizey*8/10;  y++)
    for (int x=sizex*2/10;   x<sizex*8/10;  x++)
    {
      totalPixel++;

      Vector D = currentMapStable_->get(x,y) / MULTIPLY;

      bool isInsideOfMap=(0<=x+D.x() && x+D.x()<sizex &&
                          0<=y+D.y() && y+D.y()<sizey);

      if (isInsideOfMap)
      {
        //get the color of the pixel from the right hand image
        ColorF colorL=getPixelF(left,x,y);

        //get the color of the pixel from the diplaced left hand map
        ColorF colorRdispl=getPixelF(right,x+D.x(),y+D.y());

        float qualityscore=PhotoSculptTools::comparecolor(colorL,colorRdispl);

        // if we found that the correspondance between the 2 pixels matches
        if (qualityscore < qualityThreshold)

          // then count this pixel
          goodPixel++;
      }
    }

  // get percentage value of good pixels over total number of tested pixels
  //
  return float(goodPixel)/float(totalPixel)*100.0f;
}
/*
/// Compute some sort of score of how good the displace map is,
// regarding to the stereo images
// note : Lower score is better
float SculptInProgress::getScore()
{
  // Reads map size
  int sizex = currentMap_->getSizeX();
  int sizey = currentMap_->getSizeY();

  ASSERT (sizex == 0 || sizey == 0, "Internal error : map in empty (getScore())" );

  const Image& left  = *currentLeft_  ;
  const Image& right = *currentRight_ ;

  ASSERT (sizex != left .dimx() ||  sizey != left .dimy()  ||
         sizex != right.dimx()  ||  sizey != right.dimy(),
         "Internal error : map has incorrect size (getScore())" );

  // init quality score to 0.0, info : lower score is better
  float qualityscore = 0.0;
  long int nbpixel   = 0;

  // reads the picture using a central rectangular crop of 60% x 60%
  for (int y=sizey*2/10;y<sizey*8/10;y++)

    for (int x=sizex*2/10;x<sizex*8/10;x++)
    {
      nbpixel++;
      Vector D = currentMap_->get(x,y) / MULTIPLY;

      bool isInsideOfMap=(0<=x+D.x() && x+D.x()<sizex &&
                          0<=y+D.y() && y+D.y()<sizey);

      if (isInsideOfMap)
      {
        //get the color of the pixel from the real right hand map
        ColorF colorL = getPixelF(left, x,y);

        //get the color of the pixel from the diplaced left hand map
        ColorF colorR = getPixelF(right, x+D.x(), y+D.y());

        qualityscore += PhotoSculptTools::comparecolor(colorL,colorR);
      }
      else
        qualityscore += .5;
    }

  return qualityscore/nbpixel; // to get a "per pixel value"
}


/// Compute the percentage of good pixels ( that match left to right image)
//
// that's a way to compute quality
//
// note : high percent score is better
//
float SculptInProgress::getGoodPixelPercent(float qualityThreshold)
{
  // Reads map size
  int sizex = currentMap_->getSizeX() ;
  int sizey = currentMap_->getSizeY() ;

  ASSERT (sizex == 0 || sizey == 0, "Internal error : map in empty (getGoodPixelPercent())" );

  const Image& left  = *currentLeft_  ;
  const Image& right = *currentRight_ ;

  long int goodPixel  = 0 ;
  long int totalPixel = 0 ;

  // reads the picture using a rectangular crop of 60% x 60%
  for (  int y=sizey*2/10;   y<sizey*8/10;  y++)
    for (int x=sizex*2/10;   x<sizex*8/10;  x++)
    {
      totalPixel++;

      Vector D = currentMap_->get(x,y) / MULTIPLY;

      bool isInsideOfMap=(0<=x+D.x() && x+D.x()<sizex &&
                          0<=y+D.y() && y+D.y()<sizey);

      if (isInsideOfMap)
      {
        //get the color of the pixel from the right hand image
        ColorF colorL = getPixelF(left,x,y);

        //get the color of the pixel from the diplaced left hand map
        ColorF colorRdispl = getPixelF(right, x+D.x(), y+D.y());

        float qualityscore = PhotoSculptTools::comparecolor(colorL,colorRdispl);

        // if we found that the correspondance between the 2 pixels matches
        if (qualityscore < qualityThreshold)
          goodPixel++; // then count this pixel
      }
    }

  // get percentage value of good pixels over total number of tested pixels
  return float(goodPixel)/float(totalPixel)*100.0f;
}

*/

void SculptInProgress::MixDispMap(PVectorMap DispMapOld,
                                  const PVectorMap DispMapNew,
                                  const PFloatMap  MaskNew)
{
  int sizex = DispMapNew -> getSizeX();
  int sizey = DispMapNew -> getSizeY();

  for (int y=0;y<sizey;y++)
    for (int x=0;x<sizex;x++)
    {
      UserInterrupt::stopIfOrdered();

      if (MaskNew->get(x,y) == -1)
        // Means that's an improvement
        DispMapOld -> set(x,y, DispMapNew->get(x,y));
    }
}

void SculptInProgress::isImprovedValueMap(PFloatMap& mask, 
                                          int pixRange, float maxv)
{
  int sizex = mask -> getSizeX();
  int sizey = mask -> getSizeY();

  for (int y=0;y<sizey;y++)
    for (int x=0;x<sizex;x++)
    {
      UserInterrupt::stopIfOrdered();

      Vector D = currentMap_->get(x,y) / MULTIPLY;

      float newvalue = comparePixelPatch(x,y, x + D.x(), y + D.y() ,
                                         pixRange, maxv) ;

      float BETTER = -1;
      if (newvalue < mask->get(x,y) - MIX_QUALITY_TOLERANCE)
        mask -> set(x,y, BETTER);
        // Means that's an improvement, new is better
    }
}

#include "MultiTasking.h"

static int        GlobalSizeX, GlobalSizeY;
static int        GlobalPixRange;
static float      GlobalMaxV;
static PVectorMap GlobalMap;
static PFloatMap  GlobalOldMask;
static SculptInProgress* GlobalSculpt;
static PVectorMap GlobalCurrentMap;
static PVectorMap GlobalNewMap;

void improveLine(int y, void * unused)
{
  for (int x=0;x<GlobalSizeX;x++)
  {
    UserInterrupt::stopIfOrdered();

    Vector D = GlobalCurrentMap->get(x,y)/MULTIPLY;

    float newvalue = GlobalSculpt->comparePixelPatch(x,y, x + D.x(), y + D.y() ,
                                        GlobalPixRange, GlobalMaxV) ;

    float BETTER = -1;
    if (newvalue < GlobalOldMask->get(x,y) - MIX_QUALITY_TOLERANCE)
      GlobalMap -> set(x,y, GlobalNewMap->get(x,y));
      // Means that's an improvement, new is better
  }
}

void SculptInProgress::improveMap(PVectorMap& map,
                                  PFloatMap& oldmask, 
                                  int pixRange, float maxv)
{
  DEBUG(impveap);
  const PVectorMap newmap  = new VectorMap(*currentMap_);

  int sizex = map -> getSizeX();
  int sizey = map -> getSizeY();

  GlobalSizeX       = sizex       ;
  GlobalSizeY       = sizey       ;
  GlobalPixRange    = pixRange    ;
  GlobalMaxV        = maxv        ;
  GlobalSculpt      = this        ;
  GlobalCurrentMap  = currentMap_ ;
  GlobalNewMap      = newmap      ;
  GlobalOldMask     = oldmask     ;
  GlobalMap         = map         ;

  MultiTasking::launch(sizey, improveLine);

  GlobalCurrentMap  = new VectorMap ();
  GlobalMap         = new VectorMap ();
  GlobalOldMask     = new FloatMap  ();
  GlobalNewMap      = new VectorMap ();
}

static PFloatMap  GlobalValueMap;

void computeValueLine(int y, void * unused)
{
  for (int x = 0 ; x < GlobalSizeX ; x++)
  {
    UserInterrupt::stopIfOrdered();

    Vector D = GlobalCurrentMap->get(x,y) / MULTIPLY;

    GlobalValueMap -> set(x,y, GlobalSculpt->comparePixelPatch(x,y, x + D.x(), y + D.y() ,
                                            GlobalPixRange, GlobalMaxV) );
  }
}

/// Get a 2D table of floats (a "FloatMap") that stores all the pixel comparison
//  values
//
PFloatMap SculptInProgress::computeValueMap(int pixRange, float maxv)
{
  DEBUG(copueVluMp);

  // Reads image size, to create a new one same size
  int sizex = currentMap_->getSizeX();
  int sizey = currentMap_->getSizeY();

  PFloatMap valuemap = new FloatMap(sizex,sizey); // initiates a default empty mask map

  GlobalSizeX       = sizex       ;
  GlobalSizeY       = sizey       ;
  GlobalValueMap    = valuemap    ;
  GlobalCurrentMap  = currentMap_ ;
  GlobalPixRange    = pixRange    ;
  GlobalMaxV        = maxv        ;
  GlobalSculpt      = this        ;

  // launch 4 theads to do the task "optimizeLine" for all lines
  MultiTasking::launch(sizey, computeValueLine);

  GlobalValueMap    = new FloatMap();
  GlobalCurrentMap  = new VectorMap();

  return valuemap;
}

