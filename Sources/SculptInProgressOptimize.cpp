/*
 * SculptOptimize.cpp
 *
 *  Created on: 22 avr. 2009
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */

// 80 char per line standard :
/////////_/////////_/////////_/////////_/////////_/////////_/////////_/////////_

#include "PhotoSculpt.h"
#include "Timer.h"
#include "Limit.h"
#include "ImageGeneration.h"
#include "PhotoSculptTools.h"
#include "UserInterrupt.h"
#include "SmartPointer.h"
#include "MultiTasking.h"

static SculptInProgress* GlobalSculpt;
static int        GlobalDisplacementRange; 
static int        GlobalPixelProbeSize;
static int        GlobalSkip; 
static float      GlobalMaxColorMatch;
static int        GlobalSizeX, GlobalSizeY;
static bool       GlobalIsEdgeDetect;
static float      GlobalLimit;
static PFloatMap  GlobalMask;
static PVectorMap GlobalResult, GlobalCurrentMap;
static PImage     GlobalAlpha;
static FloatMap*  GlobalEdge;

void optimizeLine(int y, void * unused)
{
  for (int x = 0; x < GlobalSizeX; x ++)
  {
    UserInterrupt::stopIfOrdered();

    if (x % GlobalSkip == 0 && y % GlobalSkip == 0)
    {
      bool iscompute = true;
      if ( ! GlobalAlpha -> is_empty() )
      { 
        // read the alpha map
        int asx = GlobalAlpha -> dimx(), asy = GlobalAlpha -> dimy();
        int ax = round (float(x) * float(asx-1) / float(GlobalSizeX-1));
        int ay = round (float(y) * float(asy-1) / float(GlobalSizeY-1));

        if (GlobalAlpha->operator()(ax,ay,0,0) < 127) // black = hide
          iscompute = false;
      }

      if (iscompute)
      {
        int d=GlobalDisplacementRange;

        if (GlobalIsEdgeDetect)
          // limit displacement range if area is almost empty
          if (GlobalEdge->operator()(x,y) > GlobalLimit) // red
            d = 10; // limit to 1 pixel max

        int vx,vy;

        GlobalMask->set(x,y, GlobalSculpt->findBestNeibour( x,y, vx,vy, d, 
                        GlobalPixelProbeSize,	GlobalMaxColorMatch));

        GlobalResult->set(x,y, vx,vy );
      }
      else // just copy the current value into the result table
      {
        Vector v = GlobalCurrentMap->get(x,y); // + Vector(1,0);
        GlobalResult->set(x,y, v);
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
// optimiseMap
////////////////////////////////////////////////////////////////////////////////
//
// note : values like skip==0 or displacementrange=0 are canceling the
//        optimisation
//        pixelProbeSize=0 is acceptable, only 1 pixel will be compared
//        (not recommended)
//
void SculptInProgress::optimiseMap(int displacementRange,int pixelProbeSize,
                                  int skip, float maxColorMatch)
{
  DEBUG(opiisMp);
  // values like skip==0 or displacementrange=0 are canceling the optimisation
  if (skip<=0 || displacementRange<=0)
    return;

  int sizex = currentMap_->getSizeX(), sizey = currentMap_->getSizeY();

  // Prepare the morphed right image (onto left image) if the morph method
  // is used

  PVectorMap result = new VectorMap(sizex,sizey);
  result->fill(0); // usefull?
  PFloatMap mask = new FloatMap(sizex,sizey);

  bool isEdgeDetect = (sizex>50 && sizey > 50);
  
  FloatMap edge ;
  const float LIMIT = 2.950f ;
  if (isEdgeDetect )
  {
    edge = PhotoSculptTools::computeEdgeDetectionX(*currentLeft_, 6);

    // red is above limit, green is below limit
    //DEBUG_SAVE_IMAGE(ImageGeneration::generateMask(edge, LIMIT ),"EdgeDetectionX.bmp");
  }

  GlobalSculpt            = this;
  GlobalDisplacementRange = displacementRange; 
  GlobalPixelProbeSize    = pixelProbeSize;
  GlobalSkip              = skip; 
  GlobalMaxColorMatch     = maxColorMatch;
  GlobalSizeX             = sizex;
  GlobalSizeY             = sizey;
  GlobalIsEdgeDetect      = isEdgeDetect;
  GlobalLimit             = LIMIT;
  GlobalMask              = mask;
  GlobalResult            = result;
  GlobalCurrentMap        = currentMap_;
  GlobalAlpha             = alpha_;
  GlobalEdge              = & edge;

  // launches n theads to do the task "optimizeLine" for all lines
  MultiTasking::launch(sizey, optimizeLine);

  GlobalMask       = new FloatMap  ();
  GlobalResult     = new VectorMap ();
  GlobalCurrentMap = new VectorMap ();
  GlobalAlpha      = new Image     ();

  currentMap_ = result ;
  result      = NULL   ; // free some memory

  currentMap_ = new VectorMap(PhotoSculptTools::blurMaskedMap(*currentMap_, 
                    *mask, skip, MASKED_BLUR_RANGE, MASKED_BLUR_THRESHOLD));
  mask = NULL; // free some memory

  if (skip>1)
    PhotoSculptTools::reInterpolateMap(currentMap_, skip);
}


float SculptInProgress::findBestNeibour(int xL,int yL, 
    int& retx, int& rety,
    int displacementRange, int scope, float maxColorMatchValue)
{
  Vector displacement = currentMap_ -> get(xL,yL);

  int xR = xL + round (float (displacement.x()) / MULTIPLY) ;
  int yR = yL + round (float (displacement.y()) / MULTIPLY) ;

  //reads the size of the left image
  int sizex = currentMap_->getSizeX();
  int sizey = currentMap_->getSizeY();

  int BurstMapUsage=BurstMap::getMaxTableIndex(displacementRange);

  //initialises the search for the closest color
  const float REALLY_BAD_DIFFERENCE = 9999999.0f;
  float bestdifference = REALLY_BAD_DIFFERENCE;
  int OptCoordRx = xR;
  int OptCoordRy = yR;

  bool isInsideRight = (0<=xR && xR<sizex && 0<=yR && yR<sizey);
  bool isInsideLeft  = (0<=xL && xL<sizex && 0<=yL && yL<sizey);

  // y limit strategy defined below
  int vlim = 10;
  if (step_ >= 7) vlim = 0;

  if ( isInsideRight && isInsideLeft )
  {
    //start pixel analysis
    for ( int b = 0; b < BurstMapUsage; b ++ )
    {
      int dx = BurstMap::x(b);
      int dy = BurstMap::y(b);

      // it gives better results to limit search in y direction
      if ( - vlim <= dy && dy <= vlim )
      {
        float weight = fallOffValue(BurstMap::distance(b), BurstMap::distance(BurstMapUsage-1), 
			                        parameter_.falloffDisplaceStepDown, 
                                    parameter_.displacementFalloff);
        float difference;
        if (weight > 0.0)
          difference=comparePixelPatch(xL,yL,	xR+dx,yR+dy,
                                       scope,maxColorMatchValue) / weight;
        else
          // weight=0, no need to compute difference
          difference = REALLY_BAD_DIFFERENCE;

		const float INTERRUPT_NEIBOUR_THRESHOLD = 0.20f;

        if (difference>INTERRUPT_NEIBOUR_THRESHOLD)
		  break;// if result is soo bad, then don't loose time searching

        if (difference < bestdifference)
        {
          OptCoordRx     = xR + dx;
          OptCoordRy     = yR + dy;
          bestdifference = difference;
        }
      }
    }
  }

  retx = round( float (OptCoordRx - xL) * MULTIPLY );
  rety = round( float (OptCoordRy - yL) * MULTIPLY );

  return bestdifference;
}


//	Checks if 2 pixels and their immediate neibour are identical
//
float SculptInProgress::comparePixelPatch(int xL,int yL, int xR,int yR, int scope,
                                     float maxColorMatchValue) const
{
  Vector displacement = currentMap_->get(xL,yL);

  //reads the size of the left image
  int sizex = currentMap_->getSizeX();
  int sizey = currentMap_->getSizeY();

  const Image& left  = *currentLeft_ ;
  const Image& right = *currentRight_;

  float sum = 0.0f ;
  int count = 0    ;

  // not zero because I fear that one day the division by zero crashes
  // the program (like if weight values = 0 for some reason)
  //
  float const BAD_COUNT_VALUE = 0.0001f;
  float nb  = BAD_COUNT_VALUE;

  int BurstMapUsage=BurstMap::getMaxTableIndex(scope);

  for (int b = 0; b < BurstMapUsage ; b++)
  {
    int bx=BurstMap::x(b);
    int by=BurstMap::y(b);

    int xLb = xL + bx, yLb = yL + by;
    int xRb = xR + bx, yRb = yR + by;

    bool isInsideLeftImage=  (0<=xLb && xLb<sizex && 0<=yLb && yLb<sizey);
    bool isInsideRightImage= (0<=xRb && xRb<sizex && 0<=yRb && yRb<sizey);

    if (isInsideLeftImage && isInsideRightImage)
    {
      ColorF leftColor  = getPixelF(left,  xLb,yLb);
      ColorF rightColor = getPixelF(right, xRb,yRb);

	  float weight=fallOffValue(BurstMap::distance(b), BurstMap::distance(BurstMapUsage-1), 
			                    parameter_.falloffRangeStepDown, 
                                parameter_.pixelFalloff);
      nb  += weight;
      sum += PhotoSculptTools::comparecolor(leftColor,rightColor) * weight;

      float mean = sum / nb ;

      // This interrupts the loop once really bad values are found
	  float interruptFactor;
	  if (count<INTERRUPT_NB)
		interruptFactor = INTERRUPT_FACTOR[count];
	  else 
		interruptFactor = INTERRUPT_FACTOR[INTERRUPT_NB-1];

      // this is a programmed search interrupt. No use to search further,
      // the result is bad
      if (mean > maxColorMatchValue * interruptFactor)
        break;

	  count ++;
    } 
  }

  float result = sum / nb;

  if (nb == BAD_COUNT_VALUE)
    result = 3.0; // bad value
  
  // result value is too good
  if (result < IGNORE_VALUE) 
	  result = IGNORE_VALUE;

  return result; // the weighted mean
}

void SculptInProgress::optimiseMapKeepBestOnly(int a,int b,int c, float maxv)
{
  DEBUG(opiseMapKepBtOly);
  if (a!=0 || b!=0 || c!=0)
  {
    if (parameter_.rangeOfKeepBest != 0)
    {
      PVectorMap bestmap = new VectorMap(*currentMap_);

      // warning slow : 36s at step 9
      PFloatMap  oldmask = computeValueMap(parameter_.rangeOfKeepBest,	maxv);

      // fills currentMap_:
      // warning slow
      optimiseMap(a,b,c,maxv); 

      // warning slow :
      improveMap(bestmap, oldmask, parameter_.rangeOfKeepBest,	 maxv);
      currentMap_ = bestmap;
    }
    else
      optimiseMap(a,b,c,maxv);
  }
}

void SculptInProgress::test_fall_off() const 
{
  // for debug only

  for (int u=1; u<8; u++)
  {
    printf("parameterPixelDisplaceFallOff_ = %.4f ",parameter_.displacementFalloff);
	  printf("falloffDisplaceStepDown = %.4f\n",parameter_.falloffDisplaceStepDown);
	  printf("displacementrange = %d >> ", u);

	  for (int b=0;b<u;b++)
	  {
	    float weight = fallOffValue(float(b),float(u),parameter_.falloffDisplaceStepDown, 
                                  parameter_.pixelFalloff);

	    printf("%.0f%% ", weight*100.0f);
	  }

	  printf("\n");
  }

}

float inline SculptInProgress::fallOffValue(float distance, float max, 
							  float falloffFirstStepdown, float falloffAtEnd) const
{
  float weight;

  if ( distance!=0 )
  {
    float distancePercent = distance / max;
    weight = (1.0f - falloffFirstStepdown) * (
              (1.0f - distancePercent) + (1.0f - falloffAtEnd ) * distancePercent );
  }
  else
    weight = 1.0f ;

  limit(weight, 0.0f, 1.0f);

  return weight;
}


