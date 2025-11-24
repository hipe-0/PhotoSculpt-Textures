/* PhotoSculptTools.cpp
 *
 * Created on: 23 avril 2009     Author: Hippolyte Mounier, Copyright 2010 
 */
#include "PhotoSculptTools.h"
#include "UserInterrupt.h"
#include "Includes.h"
#include "Limit.h"
#include "MultiTasking.h"

// Global variables used by the multithreaded "non c++" routines
static int              GlobalSizeX, GlobalSizeY ;
static const VectorMap* GlobalMap                ;
static VectorMap*       GlobalReturned           ;
static int              GlobalBurstMapUsage      ;
static const Image*     GlobalImage              ;
static PFloatMap        GlobalResult             ;
static PVectorMap       GlobalBluredMap          ;
static PImage           GlobalLeftImage          ;
static float            GlobalThreshold          ;
static PVectorMap       GlobalMap2               ;

VectorMap PhotoSculptTools::applyDisplace(const Image& image, const VectorMap & map, 
                                          float strength, float midlevel)
{
  int sizex=image.dimx();
  int sizey=image.dimy();

  ASSERT (sizex!= map.getSizeX() || sizey!= map.getSizeY(),
    "Incorrect parameter, sizes of image and map are different (in applyDisplace())");

  VectorMap result(sizex, sizey);

  for (  int y=0;  y<sizey; y++)
    for (int x=0;  x<sizex; x++)
    {
      int Br = int(image (x,y, 0,0));
	    int Bg = int(image (x,y, 0,1));
	    int Bb = int(image (x,y, 0,2));

      // computes the mean of red green and blue, to get a medium gray
      int V  = (Br + Bg + Bb) / 3 ;

      // transforms the gray value to a displacement value
      float displace = - ( ( float(V) / 255.0f ) - midlevel ) * strength * ((sizex+sizey)/2);

      Vector v    = map.get(x,y) ;
      v.setX( v.x() + int(round(displace)));
      result.set(x,y, v);
	  }

  return result;
}

IntMap PhotoSculptTools::applyDisplace(const Image& image, const IntMap & map, 
                                       float strength, float midlevel)
{
  int sizex=image.dimx();
  int sizey=image.dimy();

  ASSERT (sizex!= map.getSizeX() || sizey!= map.getSizeY(),
    "Incorrect parameter, sizes of image and map are different (in applyDisplace())");

  IntMap result(sizex, sizey);

  for   (int y=0;  y<sizey; y++)
    for (int x=0;  x<sizex; x++)
    {
      UserInterrupt::stopIfOrdered();

      int Br = int(image (x,y, 0,0));
	    int Bg = int(image (x,y, 0,1));
	    int Bb = int(image (x,y, 0,2));

      // computes the mean of red green and blue, to get a medium gray
      int   V        = (Br + Bg + Bb) / 3 ;

      // transforms the gray value to a displacement value
      float displace = - ( ( float(V) / 255.0f ) - midlevel ) * strength * ((sizex+sizey)/2);

      result(x,y) = map(x,y) + int(round(displace));
	  }

  return result;
}

float edgeWeightFunction( float luminosity )
{
  return  1.0f + 50.0f * ( luminosity - 0.6f) * ( luminosity - 0.6f) ;
}

void computeEdgeDetectionLine(int y, void* object_unused)
{
  ASSERT (y<0 || y>= GlobalSizeY, "Parameter y out of range in computeEdgeDetectionLine");
  ASSERT (GlobalResult->getSizeX() != GlobalSizeX || GlobalResult->getSizeY() != GlobalSizeY,
          "Result map wrong size in computeEdgeDetectionLine" );
  ASSERT (GlobalImage->dimx()      != GlobalSizeX || GlobalImage->dimy()      != GlobalSizeY,
          "Image wrong size in computeEdgeDetectionLine"      );

  for (int x = 0 ; x < GlobalSizeX ; x ++ )
  {
    UserInterrupt::stopIfOrdered();

    // Compute the blur map value
    ColorF center = getPixelF(*GlobalImage,x,y);

    float mostDifferentColorValue = 0.0f;

    // mean center is the luminosity value from 0.0 black to 1.0 white of the pixel at x,y
    float luminosity = (center.r + center.g + center.b ) / 3.0f ;
    float weight     = edgeWeightFunction ( luminosity ) ;

    // searches all neibouring pixels around x,y until it hits one that's too different
    // than center pixel. Difference Criteria is the square distance bigger than threshold
    for ( int b = 0 ; b < GlobalBurstMapUsage ; b++ )
    {
      int burstX = x + BurstMap::x(b);
      int burstY = y + BurstMap::y(b);

      if (0 <= burstX && burstX < GlobalSizeX && 0 <= burstY && burstY < GlobalSizeY)
      {
        ColorF side = getPixelF(*GlobalImage, burstX,burstY);

        float comparison = PhotoSculptTools::comparecolor(center,side) * weight ;

        if (comparison > mostDifferentColorValue) // we find an obstacle ?
          // store the value as it is the most different so far
          mostDifferentColorValue = comparison;
      } 
    }

    GlobalResult->set(x,y, 3.0f-mostDifferentColorValue );
  }
}

PFloatMap PhotoSculptTools::computeEdgeDetection(const Image & image, int range)
{
  DEBUG(computeEdg);
  DEBUG_VALUE("range", range);
  
  int sizex = image.dimx(), sizey = image.dimy();

  ASSERT (sizex <= 0 || sizey <= 0, "Empty input image (in computeEdgeDetection())");

  int       BurstMapUsage = BurstMap::getMaxTableIndex(range);
  PFloatMap result        = new FloatMap(sizex,sizey);
  DEBUG_VALUE("sizex", sizex);
  DEBUG_VALUE("sizey", sizey);

  // is the range so small that no loop is needed?
  if (BurstMapUsage <= 1)
  {
    result->fill(0.0f); // why?
	  return result;
  }

  GlobalImage          = &image        ;
  GlobalResult         = result        ;
  GlobalSizeX          = sizex         ;
  GlobalSizeY          = sizey         ;
  GlobalBurstMapUsage  = BurstMapUsage ;

  MultiTasking::launch(sizey, computeEdgeDetectionLine);

  GlobalResult         = new FloatMap();

  return result;
}

FloatMap PhotoSculptTools::computeEdgeDetectionX(const Image & image, int range)
{
  // Reads image size, to create a new one same size
  int sizex = image.dimx();
  int sizey = image.dimy();

  FloatMap result(sizex,sizey); // initiates a default empty mask

  for (int y=0;y<sizey;y++)
    for (int x=0;x<sizex;x++)
    {
      UserInterrupt::stopIfOrdered();

      // Compute the blur map value
      ColorF center=getPixelF(image,x,y);

      float mostDifferentColorValue=0.0f;

      // mean center is the luminosity value from 0.0 black to 1.0 white of the pixel
      // at x,y
      float luminosity = (center.r + center.g + center.b ) / 3.0f ;
      float weight = edgeWeightFunction ( luminosity ) ;

      // searches all neibouring pixels around x,y until it hits one that's too different
      // than center pixel. Difference Criteria is the square distance bigger than threshold
      for (int b=0;b<range;b++)
      {
        int burstX=x+b;
        int burstY=y;

        if (0<=burstX && burstX<sizex)
        {
          ColorF side=getPixelF(image,burstX,burstY);

          float comparison = PhotoSculptTools::comparecolor(center,side) * weight ;

          // if we find an obstacle
          //
          if (comparison>mostDifferentColorValue)
            // store the value as it is the most different so far
            mostDifferentColorValue=comparison;

        } // end if
      } // end for b

      result(x,y)=3.0f-mostDifferentColorValue;

    } // end for x,y

  return result;
}

void smartBlurLine(int y, void* object_unused)
{
    for (int x=0;x<GlobalSizeX;x++)
    {
      UserInterrupt::stopIfOrdered();

      // Compute the blur map value
      ColorF center=getPixelF(*GlobalLeftImage,x,y);

      // the luminosity value from 0.0 black to 1.0 white of the pixel
      // at x,y
      float luminosity = (center.r + center.g + center.b ) / 3.0f ;
      float weight     = 1.0f ;

      long  sumOfX = 0, sumOfY = 0;
      int   countValue = 0;

      // searches all neibouring pixels around x,y until it hits one that's too different
      // than center pixel. Difference Criteria is the square distance bigger than threshold
      for (int b=0;b<GlobalBurstMapUsage;b++)
      {
        int burstX=x+BurstMap::x(b);
        int burstY=y+BurstMap::y(b);

        if (0<=burstX && burstX<GlobalSizeX && 0<=burstY && burstY<GlobalSizeY)
        {
          Vector v=GlobalMap2->get(burstX,burstY);
          sumOfX+=v.x();
          sumOfY+=v.y();
          countValue+=1;

          ColorF side=getPixelF(*GlobalLeftImage,burstX,burstY);

          float comparison = PhotoSculptTools::comparecolor(center,side) * weight ;

          // if we find an obstacle
          //
          if (comparison > GlobalThreshold)
            // exit from loop and stop summing displacements
            break;

        } // end if
      } // end for b

      // Fills the blur map values
      if (countValue==0) // just in case
        // no blurring
        GlobalBluredMap->set(x,y, GlobalMap2->get(x,y));

      else
      {
        // compute the mean values of the summed elements
        int meanX=round( float(sumOfX) / float(countValue) );
        int meanY=round( float(sumOfY) / float(countValue) );
        GlobalBluredMap->set(x,y, meanX,meanY);
      }
    } // end for x,y

}

// Parameter : Threshold, if this parameter is small like 0.01 = small effect, it will stop blurring sooner
PVectorMap PhotoSculptTools::smartBlur(PVectorMap& map, const PImage& leftImage, float threshold, int range)
{
  DEBUG(smartBl);
  ASSERT (leftImage->dimx() != map->getSizeX() || leftImage->dimy() != map->getSizeY(), 
    "Wrong parameter, image size and map size are different (in smartBlur() )");

  // Reads image size, to create a new one same size
  int sizex = map->getSizeX();
  int sizey = map->getSizeY();

  int BurstMapUsage=BurstMap::getMaxTableIndex(range);

    // is the range so small that no loop is needed?
  if (BurstMapUsage<=1)
	  return map;

  PVectorMap bluredMap = new VectorMap(sizex,sizey); // initiates a map

  GlobalMap2              = map;
  GlobalSizeX             = sizex;
  GlobalSizeY             = sizey;
  GlobalBluredMap         = bluredMap;
  GlobalBurstMapUsage     = BurstMapUsage;
  GlobalLeftImage         = leftImage;
  GlobalThreshold         = threshold;

  MultiTasking::launch(sizey, smartBlurLine);

  GlobalBluredMap = new VectorMap();
  GlobalLeftImage = new Image    ();
  GlobalMap2      = new VectorMap();

  return bluredMap;
}

// defines the sum of 2 colors
ColorF operator+(ColorF a, ColorF b)
{
  return ColorF(a.r+b.r, a.g+b.g, a.b+b.b);
}

ColorF operator/(ColorF a, float b)
{
  return ColorF(a.r/b, a.g/b, a.b/b);
}

static VectorMap*       GlobalOptDispMap;
static const FloatMap*  GlobalMask;
static float            GlobalMaxColorMatchValue;
static int              GlobalSkip;

void blurMaskedLine(int y, void* object_unused)
{
  for (int x=0;x<GlobalSizeX;x++)
  {
    if (x%GlobalSkip==0 && y%GlobalSkip==0)
    {
      if (GlobalMask->operator()(x,y) > GlobalMaxColorMatchValue)
      {
        //this disp map value at x,y is wrong and masked = it needs to be blurred

        Vector sumd(0,0);
        int count=0;
        //start pixel analysis
        for (int k=0; k<GlobalBurstMapUsage; k++)
        {
          UserInterrupt::stopIfOrdered();

          int dx = BurstMap::x(k);
          int dy = BurstMap::y(k);

          int xx = x + dx * GlobalSkip;
          int yy = y + dy * GlobalSkip;
 
          if (0<=xx && xx<GlobalSizeX && 0<=yy && yy<GlobalSizeY)
            
            if (GlobalMask->operator()(xx,yy) > GlobalMaxColorMatchValue) //found an bad value to be blurred too
            {
              sumd += GlobalMap->get(xx,yy);

              count += 1;
            }
            else // found an edge (a good value)
              break; // stop blurring
        }

        if (count==0)
          GlobalOptDispMap->set(x,y, GlobalMap->get(x,y));
        else
          GlobalOptDispMap->set(x,y,sumd / count); // mean value of the best neigbours
      }
      else //this disp map value is good and unmasked = it needs to be kept
        GlobalOptDispMap->set(x,y,GlobalMap->get(x,y));
    }
  } // end for
}


// blur only masked values
//
VectorMap PhotoSculptTools::blurMaskedMap(const VectorMap& map, const FloatMap& mask, 
                                   int Skip, int range,
								                   float MaxColorMatchValue)
{
  DEBUG(blurMas);
  int sizex=map.getSizeX();
  int sizey=map.getSizeY();

  VectorMap OptDispMap(sizex,sizey);
          
  int BurstMapUsage = BurstMap::getMaxTableIndex(range);

  GlobalSizeX         = sizex         ;
  GlobalSizeY         = sizey         ;
  GlobalBurstMapUsage = BurstMapUsage ;
  GlobalMap           = &map          ;
  GlobalMask          = &mask         ;
  GlobalSkip          = Skip          ;
  GlobalMaxColorMatchValue = MaxColorMatchValue;
  GlobalOptDispMap    = &OptDispMap   ;

  // launch n theads to do the task "blurMaskedLine" for all lines
  MultiTasking::launch(sizey, blurMaskedLine);

  return OptDispMap;
}

void blurLine(int y, void* object_unused)
{
  for (int x=0; x<GlobalSizeX; x++)
  {
    int nb=0;
    long int u=0,v=0;

    for (int b=0; b<GlobalBurstMapUsage; b++)
    {
      UserInterrupt::stopIfOrdered();

      int dx=BurstMap::x(b);
      int dy=BurstMap::y(b);

      int xx=x+dx;
      int yy=y+dy;

      // each point that is available contributes to the mean
      if (xx>=0 && xx<GlobalSizeX && yy>=0 && yy<GlobalSizeY)
      {
        Vector d=GlobalMap->get(xx,yy);
        nb+=1;
        u+=d.x();
        v+=d.y();
      }
    }

    int meanu=int(round(float(u)/nb));
    int meanv=int(round(float(v)/nb));

    GlobalReturned->set(x,y, meanu,meanv);
  }
}

VectorMap PhotoSculptTools::blurMap(const VectorMap& map, int range)
{
  DEBUG(blurMa);
  // if range is zero, then do not blur
  if (range<=0)
    return map;

  int sizex=map.getSizeX(), sizey=map.getSizeY();

  // creates the new map
  VectorMap returned(sizex,sizey);

  // gets the exact number of pixel to use for blurring 1 pixel
  int BurstMapUsage=BurstMap::getMaxTableIndex(range);

  GlobalSizeX         = sizex         ;
  GlobalSizeY         = sizey         ;
  GlobalBurstMapUsage = BurstMapUsage ;
  GlobalReturned      = &returned     ;
  GlobalMap           = &map          ;
    
  // launch 4 theads to do the task "optimizeLine" for all lines
  MultiTasking::launch(sizey, blurLine);

  return returned;
}

IntMap PhotoSculptTools::blurMap(const IntMap& map, int range)
{
  // if range is zero, then do not blur
  if (range<=0)
    return map;

  int sizex=map.getSizeX(), sizey=map.getSizeY();

  // creates the new map
  IntMap returned(sizex,sizey);

  // gets the exact number of pixel to use for blurring 1 pixel
  int BurstMapUsage=BurstMap::getMaxTableIndex(range);

  // optimizes the dispmap using each pixel one by one
  for (int y=0; y<sizey; y++)
    for (int x=0; x<sizex; x++)
    {
      int nb=0;
      long int u=0;

      for (int b=0; b<BurstMapUsage; b++)
      {
        UserInterrupt::stopIfOrdered();

        int dx=BurstMap::x(b);
        int dy=BurstMap::y(b);

        int xx=x+dx;
        int yy=y+dy;

        // each point that is available contributes to the mean
        if (xx>=0 && xx<sizex && yy>=0 && yy<sizey)
        {
          int d=map(xx,yy);
          nb+=1;
          u+=d;
        }
      }

      returned(x,y) = round(float(u)/nb);
    }

  return returned;
}

PVectorMap PhotoSculptTools::blurMapYOnly(PVectorMap& map, int range)
{
  // if range is zero, then do not blur
  if (range<=0)
    return map;

  int sizex=map->getSizeX(), sizey=map->getSizeY();

  // creates the new map
  PVectorMap returned = new VectorMap(sizex,sizey);

  // gets the exact number of pixel to use for blurring 1 pixel
  int BurstMapUsage=BurstMap::getMaxTableIndex(range);

  // optimizes the dispmap using each pixel one by one
  for (int y=0; y<sizey; y++)
    for (int x=0; x<sizex; x++)
    {
      UserInterrupt::stopIfOrdered();

      int nb=0;
      long int v=0;

      for (int b=0; b<BurstMapUsage; b++)
      {
        int dx=BurstMap::x(b);
        int dy=BurstMap::y(b);

        int xx=x+dx;
        int yy=y+dy;

        // each point that is available contributes to the mean
        if (xx>=0 && xx<sizex && yy>=0 && yy<sizey)
        {
          nb+=1;
          v += map->y(xx,yy) ;
        }
      }

      int meanv=int(round(float(v)/nb));

      returned->set(x,y, map->x(x,y),meanv);
    }

  return returned;
}

/// Fills the map between every 'step' element using an interpolation
// note : Improvement for future revisions: interpolation of 2nd order instead of 1st order linear
VectorMap PhotoSculptTools::reInterpolateMap(const VectorMap & map, int step)
{
  int sizex=map.getSizeX();
  int sizey=map.getSizeY();

  VectorMap result(sizex, sizey);

  // artifact used to control the round-off of the interpolation (it was too low)
  int stepbis=step+1;

  for (int y=0;y<sizey;y++)
    for (int x=0;x<sizex;x++)
    {
      UserInterrupt::stopIfOrdered();

      //interpolation entre les points principaux
      float dx=float(x%step);
      float dy=float(y%step);

      if (dx!=0 || dy!=0) {
        float x0=x-dx;
        float y0=y-dy;
        float x1=x-dx+step;
        float y1=y-dy+step;

        if (x1>=sizex)
          x1=x0;
        if (y1>=sizey)
          y1=y0;

		    Vector disp00=map.get(int(::round(x0)),int(::round(y0)));
        Vector disp10=map.get(int(round(x1)),int(round(y0)));
        Vector disp01=map.get(int(round(x0)),int(round(y1)));
        Vector disp11=map.get(int(round(x1)),int(round(y1)));

        float interp_dx0=(disp00.x()*(step-dx)+disp10.x()*dx)/step;
        float interp_dx1=(disp01.x()*(step-dx)+disp11.x()*dx)/step;
        float interp_dx= (interp_dx0*(step-dy)+interp_dx1*dy);

        if (interp_dx>0.0)
          interp_dx=(interp_dx+stepbis)/step;
        else if (interp_dx<0.0)
           interp_dx=(interp_dx-stepbis)/step;

        float interp_dy0=(disp00.y()*(step-dx)+disp10.y()*dx)/step;
        float interp_dy1=(disp01.y()*(step-dx)+disp11.y()*dx)/step;
        float interp_dy= (interp_dy0*(step-dy)+interp_dy1*dy);

        if (interp_dy>0.0)
          interp_dy=(interp_dy+stepbis)/step;
        else if (interp_dy<0.0)
          interp_dy=(interp_dy-stepbis)/step;

        result.set(x,y, int(round(interp_dx)),int(round(interp_dy)));
      }
      else
        result.set(x,y,map.get(x,y));
    }

  return result;
}

void PhotoSculptTools::reInterpolateMap(PVectorMap & map, int step)
{
  int sizex=map->getSizeX();
  int sizey=map->getSizeY();

  // artifact used to control the round-off of the interpolation (it was too low)
  int stepbis=step+1;

  for (int y=0;y<sizey;y++)
    for (int x=0;x<sizex;x++)
    {
      UserInterrupt::stopIfOrdered();

      //interpolation entre les points principaux
      float dx=float(x%step);
      float dy=float(y%step);

      if (dx!=0 || dy!=0) {
        float x0=x-dx;
        float y0=y-dy;
        float x1=x-dx+step;
        float y1=y-dy+step;

        if (x1>=sizex)
          x1=x0;
        if (y1>=sizey)
          y1=y0;

		    Vector disp00 = map->get( round(x0) , round(y0) );
        Vector disp10 = map->get( round(x1) , round(y0) );
        Vector disp01 = map->get( round(x0) , round(y1) );
        Vector disp11 = map->get( round(x1) , round(y1) );

        float interp_dx0=(disp00.x()*(step-dx)+disp10.x()*dx)/step;
        float interp_dx1=(disp01.x()*(step-dx)+disp11.x()*dx)/step;
        float interp_dx= (interp_dx0*(step-dy)+interp_dx1*dy);

        if (interp_dx>0.0)
          interp_dx=(interp_dx+stepbis)/step;
        else if (interp_dx<0.0)
           interp_dx=(interp_dx-stepbis)/step;

        float interp_dy0=(disp00.y()*(step-dx)+disp10.y()*dx)/step;
        float interp_dy1=(disp01.y()*(step-dx)+disp11.y()*dx)/step;
        float interp_dy= (interp_dy0*(step-dy)+interp_dy1*dy);

        if (interp_dy>0.0)
          interp_dy=(interp_dy+stepbis)/step;
        else if (interp_dy<0.0)
          interp_dy=(interp_dy-stepbis)/step;

        map->set(x,y, round(interp_dx),round(interp_dy));
      }
    }
}

Image PhotoSculptTools::areaFill(const Image& image)
{
  int sizex=image.dimx();
  int sizey=image.dimy();

  Image fill=image;

  for   ( int y = 0;  y < sizey; y ++ )
    for ( int x = 0;  x < sizex; x ++ )
    {
      Color currentcolor = getPixel(image, x,y);
      
      unsigned char destcolor[3] = { rand()%255, rand()%255, rand()%255 };

      UserInterrupt::stopIfOrdered();

      fill.draw_fill(x,y, destcolor,5.0f);
    }

  return fill;
}

Image PhotoSculptTools::generateMorphed(const Image& right, const VectorMap & map, bool extend)
{
  //reads the size of the map
  int sizex = map.getSizeX();
  int sizey = map.getSizeY();

  ASSERT (right.dimx() != sizex || right.dimy() != sizey,
    "Wrong arguments, image and map are different (in generateMorphed())");

  Image returned(sizex, sizey, 1, 3);

  // create picture using the dispmap information
  for (  int y=0;  y<sizey; y++)
    for (int x=0;  x<sizex; x++)
    {
      UserInterrupt::stopIfOrdered();
      // Displaced coordinates, so they are the corresponding coordinates on the right hand map
      int xR=x+round(float( map.x(x,y) ) / MULTIPLY);
      int yR=y+round(float( map.y(x,y) ) / MULTIPLY);

      Color c;
      if (extend)
      {
        // in case the disp map shows outbound data
        // then chooses the pixel that's at the border of the image
	      limit (xR, 0, sizex-1);
	      limit (yR, 0, sizey-1);
        c=getPixel(right, xR, yR);
      }
      else
      { // outbound area is black
        if (xR<0 || xR>=sizex || yR<0 || yR>=sizey)
          c=Color(0,0,0); // = black
        else
          c=getPixel(right, xR, yR);
      }

      //get the color of the pixel from the diplaced left hand map
      //uses the col value to fill rgb pixel color
      setPixel(returned, x, y, c);
    }

  return returned;
}

Image PhotoSculptTools::generateMorphedLeft(const Image& left, const VectorMap & map, bool extend)
{
  //reads the size of the map
  int sizex = map.getSizeX();
  int sizey = map.getSizeY();

  ASSERT (left.dimx() != sizex || left.dimy() != sizey,
    "Wrong arguments, image and map are different (in generateMorphedLeft())");

  Image right(sizex, sizey, 1, 3);
  right.fill(255);

  // create picture using the dispmap information
  for (  int yl=0;  yl<sizey; yl++)
    for (int xl=0;  xl<sizex; xl++)
    {
      UserInterrupt::stopIfOrdered();
      // Displaced coordinates, so they are the corresponding coordinates on the right hand map
      int xr = xl + round(float( map.x(xl,yl) ) / MULTIPLY);
      int yr = yl + round(float( map.y(xl,yl) ) / MULTIPLY);

      Color c=getPixel(left, xl, yl);

      if ( xr >= 0 && xr < sizex-1 && yr >= 0 && yr < sizey-1 )
      {
        setPixel(right, xr, yr, c);      //uses the col value to fill rgb pixel color
        setPixel(right, xr+1, yr, c);    //uses the col value to fill rgb pixel color
        setPixel(right, xr, yr+1, c);    //uses the col value to fill rgb pixel color
        setPixel(right, xr+1, yr+1, c);  //uses the col value to fill rgb pixel color
      }
    }

  return right;
}

void PhotoSculptTools::mixByAlpha(PIntMap original, const PIntMap modification, const PImage alpha)
{
  // alpha and original can have different sizes

  // if no alpha then no filtering, the modification map is copied to the original 
  if ( alpha -> is_empty() )
  {
    *original = *modification;
    return;
  }

  int sizex = original->getSizeX(), sizey = original->getSizeY();
  int asx = alpha -> dimx(), asy = alpha -> dimy();
  
  ASSERT (modification->getSizeX() != sizex || modification->getSizeY() != sizey,
    "Wrong arguments, modification and original are different in size (in mixByAlpha())");
    
  for   (int y = 0; y < sizey; y ++)
    for (int x = 0; x < sizex; x ++)
    {
      // read the alpha map
      int ax = round (float(x) * float(asx-1) / float(sizex-1));
      int ay = round (float(y) * float(asy-1) / float(sizey-1));

      if (alpha->operator()(ax,ay,0,0) > 127) // white = show
      {
        int t = modification->get(x,y);
        original->set(x,y, t);
      }
    }
}
