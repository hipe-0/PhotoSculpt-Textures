/*
 * Seamless.cpp
 *
 *  Created on: 14 may 2009
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */

// 80 char per line standard :
/////////_/////////_/////////_/////////_/////////_/////////_/////////_/////////_

bool isMethodCrop=false;

#include "Seamless.h"
#include "PhotoSculpt.h"
#include "PhotoSculptTools.h"
#include "Includes.h"
#include "Bicubic.h"
#include "convert.h"
#include "Options.h"


IntMap Seamless::computeFlattenMap(const IntMap& intmap, int nbsamples)
{
  DEBUG(cmpteFlttenap);

  IntMap flatmap= intmap.getResize(nbsamples*2,nbsamples*2);
  flatmap		= PhotoSculptTools::blurMap(flatmap,10);

  // reduces the map to a very small size
  flatmap = flatmap.getResize(nbsamples,nbsamples);
  
  // increases it to 30x30
  flatmap = flatmap.getResize(30,30);
  flatmap = PhotoSculptTools::blurMap(flatmap,50);

  flatmap = flatmap.getResize(intmap.getSizeX(),intmap.getSizeY());

  return intmap - flatmap;
}

IntMap Seamless::computeBorderedMap(const IntMap& intmap, int singleBorderX, int singleBorderY, float zscale)
{
  DEBUG(coputBordedap);
  IntMap flatmap = computeFlattenMap(intmap, GlobalOptions.nbFlatModeSamples_);

  int sx = intmap.getSizeX();
  int sy = intmap.getSizeY();
  IntMap bordermap( sx, sy );
  bordermap.fill(0);

  int sqsingleBorderX = singleBorderX*singleBorderX,  sqsingleBorderY=singleBorderY*singleBorderY;

  float value = MULTIPLY * SEAMLESS_SCALE_FACTOR / zscale; //  * (sx+sy) 

  for   (int y = 0; y < sy; y++)
    for (int x = 0; x < sx; x++)
    {
      if (x < singleBorderX)
      {
        int i = singleBorderX - x;
        float factor = value * (singleBorderX - sqrtf(float((sqsingleBorderX - i*i))));
        bordermap(x,y) =  bordermap(x,y) -  int ( round ( factor ));
      }
      if (x >= sx-singleBorderX)
      {
        int i = x - sx + singleBorderX;
        float factor = value * (singleBorderX - sqrtf(float((sqsingleBorderX - i*i))));
        bordermap(x,y) =  bordermap(x,y) - int ( round ( factor ));
      }
      if (y < singleBorderY)
      {
        int i = singleBorderY - y;
        float factor = value * (singleBorderY - sqrtf(float((sqsingleBorderY - i*i))));
        bordermap(x,y) =  bordermap(x,y) - int ( round ( factor ));
      }
      if (y >= sy-singleBorderY)
      {
        int i = y - sy + singleBorderY;
        float factor = value * (singleBorderY - sqrtf(float((sqsingleBorderY - i*i))));
        bordermap(x,y) =  bordermap(x,y) - int ( round ( factor ));
      }
    }

  return flatmap - bordermap;
}

Image Seamless::computeSeamlessImageFromBordered(const IntMap & bordered, 
                                                 const Image & image, 
                                                 int singleBorderX, int singleBorderY)
{
  DEBUG(comteamlssIageFrmBrdred);
  int sx=bordered.getSizeX();
  int sy=bordered.getSizeY();

  if (sx != image.dimx() || sy != image.dimy())
    THROW_INTERNAL_ERROR( "Initialisation error (map and image don't match in computeSeamlessImageFromBordered())");

  Image result = image;

  for (int y=0;y<sy;y++)
    for (int x=0;x<sx;x++)
    {
      int currentHeight  = bordered(x,y) ;
      Color currentColor = getPixel(image,x,y);

      if (x<singleBorderX*2)
      {
        int newBorderHeight = bordered(x+sx-singleBorderX*2, y);
        if (newBorderHeight<currentHeight)
        {
          currentHeight = newBorderHeight;
          currentColor  = getPixel(image,x+sx-singleBorderX*2, y) ;
        }
      }

      if (x>sx-singleBorderX*2)
      {
        int newBorderHeight = bordered(x-sx+singleBorderX*2, y);

        if (newBorderHeight<currentHeight)
        {
          currentHeight = newBorderHeight;
          currentColor  = getPixel(image,x-sx+singleBorderX*2, y) ;
        }
      }
      if (y<singleBorderY*2)
      {
        int newBorderHeight = bordered(x, y+sy-singleBorderY*2);

        if (newBorderHeight<currentHeight)
        {
          currentHeight = newBorderHeight;
          currentColor  = getPixel(image,x, y+sy-singleBorderY*2) ;
        }
      }

      if (y>sy-singleBorderY*2)
      {
        int newBorderHeight = bordered(x, y-sy+singleBorderY*2);

        if (newBorderHeight<currentHeight)
        {
          currentHeight = newBorderHeight;
          currentColor  = getPixel(image,x, y-sy+singleBorderY*2) ;
        }
      }

      if (x<singleBorderX*2 && y<singleBorderY*2)
      {
        int newBorderHeight = bordered(x+sx-singleBorderX*2, y+sy-singleBorderY*2);

        if (newBorderHeight<currentHeight)
        {
          currentHeight = newBorderHeight;
          currentColor  = getPixel(image,x+sx-singleBorderX*2, y+sy-singleBorderY*2) ;
        }
      }
      if (x<singleBorderX*2 && y>sy-singleBorderY*2)
      {
        int newBorderHeight = bordered(x+sx-singleBorderX*2, y-sy+singleBorderY*2);

        if (newBorderHeight<currentHeight)
        {
          currentHeight = newBorderHeight;
          currentColor  = getPixel(image,x+sx-singleBorderX*2, y-sy+singleBorderY*2) ;
        }
      }
      if (x>sx-singleBorderX*2 && y<singleBorderY*2)
      {
        int newBorderHeight = bordered(x-sx+singleBorderX*2, y+sy-singleBorderY*2);

        if (newBorderHeight<currentHeight)
        {
          currentHeight = newBorderHeight;
          currentColor  = getPixel(image,x-sx+singleBorderX*2, y+sy-singleBorderY*2) ;
        }
      }
      if (x>sx-singleBorderX*2 && y>sy-singleBorderY*2)
      {
        int newBorderHeight = bordered(x-sx+singleBorderX*2, y-sy+singleBorderY*2);

        if (newBorderHeight<currentHeight)
        {
          currentHeight = newBorderHeight;
          currentColor  = getPixel(image,x-sx+singleBorderX*2, y-sy+singleBorderY*2) ;
        }
      }

      setPixel(result, x,y, currentColor);
    }
  
  const int s = 1, t=0;
  result = result.get_crop(singleBorderX+s, singleBorderY+s, sx-singleBorderX-t, sy-singleBorderY-t);
  
  return result;
}

Image Seamless::computeSeamlessMaskFromBordered(const IntMap & bordered,
                                                 int singleBorderX, int singleBorderY)
{
  DEBUG(comutSemlesMaskFomBodeed);
  int sx=bordered.getSizeX();
  int sy=bordered.getSizeY();

  Image result(sx, sy,1,1);
  //result.fill(255);

  for (int y=0;y<sy;y++)
    for (int x=0;x<sx;x++)
    {
      int currentHeight  = bordered(x,y) ;

      bool iscurrentwhite = true;

      if (x<singleBorderX*2)
      {
        int newBorderHeight = bordered(      x+sx-singleBorderX*2, y);
        if (newBorderHeight<currentHeight)
        {
          currentHeight = newBorderHeight;
          iscurrentwhite = false;
        }
      }

      if (x>sx-singleBorderX*2)
      {
        int newBorderHeight = bordered(      x-sx+singleBorderX*2, y);

        if (newBorderHeight<currentHeight)
        {
          currentHeight = newBorderHeight;
          iscurrentwhite = false;
        }
      }
      if (y<singleBorderY*2)
      {
        int newBorderHeight = bordered(      x, y+sy-singleBorderY*2);

        if (newBorderHeight<currentHeight)
        {
          currentHeight = newBorderHeight;
          iscurrentwhite = false;
       }
      }

      if (y>sy-singleBorderY*2)
      {
        int newBorderHeight = bordered(      x, y-sy+singleBorderY*2);

        if (newBorderHeight<currentHeight)
        {
          currentHeight = newBorderHeight;
          iscurrentwhite = false;
        }
      }

      if (x<singleBorderX*2 && y<singleBorderY*2)
      {
        int newBorderHeight = bordered(      x+sx-singleBorderX*2, y+sy-singleBorderY*2);

        if (newBorderHeight<currentHeight)
        {
          currentHeight = newBorderHeight;
          iscurrentwhite = true;
        }
      }
      if (x<singleBorderX*2 && y>sy-singleBorderY*2)
      {
        int newBorderHeight = bordered(      x+sx-singleBorderX*2, y-sy+singleBorderY*2);

        if (newBorderHeight<currentHeight)
        {
          currentHeight = newBorderHeight;
          iscurrentwhite = true;
       }
      }
      if (x>sx-singleBorderX*2 && y<singleBorderY*2)
      {
        int newBorderHeight = bordered(      x-sx+singleBorderX*2, y+sy-singleBorderY*2);

        if (newBorderHeight<currentHeight)
        {
          currentHeight = newBorderHeight;
          iscurrentwhite = true;
        }
      }
      if (x>sx-singleBorderX*2 && y>sy-singleBorderY*2)
      {
        int newBorderHeight = bordered(      x-sx+singleBorderX*2, y-sy+singleBorderY*2);

        if (newBorderHeight<currentHeight)
        {
          currentHeight = newBorderHeight;
          iscurrentwhite = true;
        }
      }

      if (iscurrentwhite)
        result(x,y,0,0) = 255; // white
      else
        result(x,y,0,0) = 0; // setPixel(result, x,y, Color(0,0,0)); //black
    }
  
  const int s = 1, t=0;
  result = result.get_crop(singleBorderX+s, singleBorderY+s, sx-singleBorderX-t, sy-singleBorderY-t);
  
  return result;
}


IntMap Seamless::computeSeamlessMapFormBorderedUncroped(const IntMap & bordered,
                                                        int singleBorderX, int singleBorderY)
{
  DEBUG(comteSemlesMapFrmBodred);
  int sx=bordered.getSizeX();
  int sy=bordered.getSizeY();

  IntMap res(sx, sy);

  for (int y=0;y<sy;y++)
    for (int x=0;x<sx;x++)
    {
      int currentHeight   = bordered(x,y) ;

      if (x<singleBorderX*2)
      {
        int newBorderHeight = bordered(           x+sx-singleBorderX*2, y);

        if (newBorderHeight<currentHeight)
          // this pixel wins as it's higher. It gets stored into currentHeight
          currentHeight = newBorderHeight;
      }

      if (x>sx-singleBorderX*2)
      {
        int newBorderHeight = bordered(           x-sx+singleBorderX*2, y);

        if (newBorderHeight<currentHeight)
          currentHeight = newBorderHeight;
      }
      if (y<singleBorderY*2)
      {
        int newBorderHeight = bordered(           x, y+sy-singleBorderY*2);

        if (newBorderHeight<currentHeight)
          currentHeight = newBorderHeight;
      }

      if (y>sy-singleBorderY*2)
      {
        int newBorderHeight = bordered(           x, y-sy+singleBorderY*2);

        if (newBorderHeight<currentHeight)
          currentHeight = newBorderHeight;
      }

      if (x<singleBorderX*2 && y<singleBorderY*2)
      {
        int newBorderHeight = bordered(  x+sx-singleBorderX*2, y+sy-singleBorderY*2);
        if (newBorderHeight<currentHeight)
          currentHeight = newBorderHeight;
      }
      if (x<singleBorderX*2 && y>sy-singleBorderY*2)
      {
        int newBorderHeight = bordered(  x+sx-singleBorderX*2, y-sy+singleBorderY*2);
        if (newBorderHeight<currentHeight)
          currentHeight = newBorderHeight;
      }
      if (x>sx-singleBorderX*2 && y<singleBorderY*2)
      {
        int newBorderHeight = bordered(  x-sx+singleBorderX*2, y+sy-singleBorderY*2);
        if (newBorderHeight<currentHeight)
          currentHeight = newBorderHeight;
      }
      if (x>sx-singleBorderX*2 && y>sy-singleBorderY*2)
      {
        int newBorderHeight = bordered(  x-sx+singleBorderX*2, y-sy+singleBorderY*2);
        if (newBorderHeight<currentHeight)
          currentHeight = newBorderHeight;
      }

      res(x,y)=currentHeight;
    }
    
  return res;
}

IntMap Seamless::cropTextureMap(const IntMap & intmap, int cropx, int cropy)
{
  return intmap.getCrop(cropx+1, cropy+1, intmap.getSizeX()-cropx, intmap.getSizeY()-cropy);
}
