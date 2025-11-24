/*
 * ImageGeneration.cpp
 *
 *  Created on: 29 janv. 2009
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */

#include "ImageGeneration.h"
#include "Limit.h"
#include "ImageTools.h"
#include "BurstMap.h"
#include "PhotoSculptTools.h"
#include "Frame.h"
#include "View3D.h"
#include "Color.h"
#include "Square.h"

//////  Generate images from the displacement maps

Image ImageGeneration::generateDepth(const IntMap& map) 
{
  int sizex = map.getSizeX();
  int sizey = map.getSizeY();

  // get a value in the center of the screen
  int mean=-map(sizex/2, sizey/2);

  Image newimage(sizex,sizey,1,3);

  int minimum=mean, maximum=mean;

  // first scan for the min and max values of the table
  for (int j=0;j<sizey;j++)
    for (int i=0;i<sizex;i++)
    {
      int col= -map(i,j); 

      if (col < minimum)   minimum = col;
      if (col > maximum)   maximum = col;
    }

  // create picture using the dispmap information
  for (int j=0;j<sizey;j++)
    for (int i=0;i<sizex;i++)
    {
      int col;

      if ( maximum-minimum != 0 )
        col = (-map(i,j)-minimum) * 255 / (maximum-minimum);
      else
        col = 128 ; // medium gray

      limit( col , 0, 255 );

      //uses the col value to fill rgb pixel color : this makes a gray tone because r=g=b
      setPixel(newimage, i,j, Color(unsigned char(col),unsigned char(col),unsigned char(col)) );
    }

  return newimage;
}

Image16 ImageGeneration::generateDepth16(const IntMap& map) 
{
  int sizex=map.getSizeX();
  int sizey=map.getSizeY();

  // get a value in the center of the screen
  int mean = -map(sizex/2, sizey/2);

  Image16 newimage(sizex,sizey,1,3);

  int minimum = mean, maximum = mean;

  // first scan for the min and max values of the table
  for (int j=0;j<sizey;j++)
    for (int i=0;i<sizex;i++)
    {
      int col= -map(i,j); 

      if (col < minimum)   minimum = col;
      if (col > maximum)   maximum = col;
    } 

  // create picture using the dispmap information
  for (int j=0;j<sizey;j++)
    for (int i=0;i<sizex;i++)
    {
      int col;

      if ( maximum-minimum != 0 )
        col = round ( float ( -map(i,j) - minimum ) * 65535.0f / float(maximum - minimum) );
      else
        col = 32768 ; // medium gray

      limit( col , 0, 65535 );

      newimage(i,j,0,0) = newimage(i,j,0,1) = newimage(i,j,0,2) = col;
    }

  return newimage;
}

Image ImageGeneration::generateDepth(const FloatMap& map) 
{
  int sizex=map.getSizeX();
  int sizey=map.getSizeY();

  // get a value in the center of the screen
  float mean=-map(sizex/2, sizey/2);

  Image newimage(sizex,sizey,1,3);

  float minimum=mean, maximum=mean;

  // first scan for the min and max values of the table
  for (int j=0;j<sizey;j++)
    for (int i=0;i<sizex;i++)
    {
      float col= -map(i,j); 

      if (col < minimum)  minimum = col ;
      if (col > maximum)  maximum = col ;
    }

  // create picture using the dispmap information
  for (int j=0;j<sizey;j++)
    for (int i=0;i<sizex;i++)
    {
      float col;

      if ( maximum-minimum != 0 )
        col = (-map(i,j)-minimum) * 255 / (maximum-minimum);
      else
        col = 128 ; // medium gray

      limit( col , 0.0f, 255.0f );

      //uses the col value to fill rgb pixel color : this makes a gray tone because r=g=b
      setPixel(newimage, i,j, Color(unsigned char(col),unsigned char(col),unsigned char(col)) );
    }

  return newimage;
}

IntMap ImageGeneration::improveDepthMapWithNormalMap(const Image& image, const IntMap& currentDepthmap)
{
  int       sizex=currentDepthmap.getSizeX(), sizey=currentDepthmap.getSizeY();
  int       isizex = image.dimx() , isizey = image.dimy();

  ASSERT (image.is_empty(), "No image in improveDepthMapWithNormalMap");
  ASSERT (!currentDepthmap.isEmpty() && (isizex != sizex || isizey != sizey),
          "image does not match the current map (in improveDepthMapWithNormalMap())");

  const int DEFAULT_MIN_MAX_DEPTH = 4 * (isizex+isizey);
  IntMap V(isizex,isizey);
  int    minimum=0, maximum=0;

  if (!currentDepthmap.isEmpty())
  {
    // get a value in the center of the screen
    minimum = maximum = -currentDepthmap(sizex/2, sizey/2);

    // first scan for the min and max values of the table
    for (int j=0;j<sizey;j++)
      for (int i=0;i<sizex;i++)
      {
        int col= -currentDepthmap(i,j); 

        if (col < minimum)   minimum = col;
        if (col > maximum)   maximum = col;
      }
  }

  if (minimum == maximum)
     // this can be either an empty current map or a map filled with same values
  {
    minimum = - DEFAULT_MIN_MAX_DEPTH;
    maximum = + DEFAULT_MIN_MAX_DEPTH;
  }

  // create picture using the dispmap information
  for   (int j=0 ; j<isizey ; j++)
    for (int i=0 ; i<isizex ; i++)
    {
      int col=128; // grey

      if ( maximum-minimum != 0 )
        col = (image(i,j,0,0) + image(i,j,0,1) + image(i,j,0,2)) / 3 ;

      int value = minimum - col * (maximum-minimum) / 255;
      V(i,j) = value;
    }

  V = PhotoSculptTools::blurMap(V, 20);

  return V;
}

IntMap ImageGeneration::improveDepthMapWithNormalMap(const Image& image)
{
  ASSERT (image.is_empty(), "No image in improveDepthMapWithNormalMap");

  int   isizex = image.dimx() , isizey = image.dimy();

  IntMap V(isizex,isizey);

  const int DEFAULT_MIN_MAX_DEPTH = (isizex+isizey)/2;
  const int minimum = - DEFAULT_MIN_MAX_DEPTH;
  const int maximum =   DEFAULT_MIN_MAX_DEPTH;

  // create picture using the dispmap information
  for   (int j=0 ; j<isizey ; j++)
    for (int i=0 ; i<isizex ; i++)
    {
      int col = (image(i,j,0,0) + image(i,j,0,1) + image(i,j,0,2)) / 3 ;

      int value = minimum - col * (maximum-minimum) / 255;
      V(i,j) = value;
    }

  V = PhotoSculptTools::blurMap(V, 20);

  return V;
}

IntMap ImageGeneration::loadDepthFromImage16(const Image16& image)
{
  ASSERT (image.is_empty(), "No image");

  int  sizex = image.dimx(), sizey = image.dimy(), nbchannels = image.dimv();

  bool is16bit=false;
  for     (int j = 0 ; j < sizey;      j ++)
    for   (int i = 0 ; i < sizex;      i ++)
      for (int k = 0 ; k < nbchannels; k ++)
        if (image(i,j,0,k) > 255 ) { is16bit = true; break;}
    
  IntMap newintmap(sizex, sizey);

  const int DEFAULT_MIN_MAX_DEPTH = (sizex + sizey)/2;
  const int minimum = - DEFAULT_MIN_MAX_DEPTH;
  const int maximum =   DEFAULT_MIN_MAX_DEPTH;

  // create picture using the dispmap information
  for   (int j=0 ; j<sizey ; j++)
    for (int i=0 ; i<sizex ; i++)
    {
      int col = 0;
      for (int k = 0 ; k < nbchannels; k ++)
        col += image(i,j,0,k);
      col /= nbchannels; // get the mean value of all chanels

      if (is16bit)
        newintmap(i,j) = minimum - col * (maximum-minimum) / 65535;
      else
        newintmap(i,j) = minimum - col * (maximum-minimum) / 255;
    }

  if (!is16bit)
    newintmap = PhotoSculptTools::blurMap(newintmap, 20);

  return newintmap;
}


Image ImageGeneration::generateBump(const IntMap& map) 
{
  const int MULT = 5 ; // pre multiplies the result to increase the final resolution

  IntMap newmap = map * MULT ;
  IntMap blured = PhotoSculptTools::blurMap(newmap, 30);
  newmap = newmap - blured ;

  blured = IntMap(); // free memory

  return generateDepth(newmap);
}

Image ImageGeneration::generateDisplace(const IntMap& map)
{
  IntMap blured = PhotoSculptTools::blurMap(map, 100);
  IntMap newmap = map - blured ;

  blured = IntMap(); // free memory

  return generateDepth(newmap);
}

void limit(IntMap &map, int elementlimit)
{
  int sizex = map.getSizeX();
  int sizey = map.getSizeY();
  int nbvertex = sizex * sizey;

  if (nbvertex > elementlimit)
  { // a limit to the number of points displayed is needed to avoid crash

    float ratio = float(elementlimit) / float(nbvertex); // ratio is < 1

    sizex=int(round(float(sizex)*sqrtf(ratio)));
    sizey=int(round(float(sizey)*sqrtf(ratio)));

    map = map.getResizeAndMultiply(sizex,sizey);
  }
}

IntMap ImageGeneration::improveDepthMapWithNormalMap(const Image& depthimage, const Image& normalmapimage)
{
  ASSERT (depthimage.is_empty(), "No image in improveDepthMapWithNormalMap");
  ASSERT (normalmapimage.is_empty(), "No normal map in improveDepthMapWithNormalMap");
  ASSERT (depthimage.dimx() != normalmapimage.dimx() || depthimage.dimy() != normalmapimage.dimy(),
          "sizes of images don't match in improveDepthMapWithNormalMap");

  /// Search for the most correct zscale factor that fit both image and normal map
  int          sx = depthimage.dimx() , sy = depthimage.dimy();
  const int    MAX_SIZE = 100;
  const int    NB_TRIAL = 50;

  Frame::limit(sx,sy,MAX_SIZE,MAX_SIZE);
  float        subdiv = float(depthimage.dimx()) / float(sx);

  IntMap       depth      = improveDepthMapWithNormalMap        (depthimage) ;
  IntMap       thumbdepth = depth.getResizeAndMultiply(sx,sy)      ;

  Image        thumbnormalreference = normalmapimage.get_resize(sx,sy);

  float        zscale = 1.0f , bestzscale = 1.0f ;
  int          bestsquare = (255*255*3)*sx*sy; // worse value to start

  for (int i = 0; i<NB_TRIAL ; i++)
  {
    Image newthumbnormal = View3D::generateNormalMapImage(thumbdepth, zscale/MULTIPLY, false, false) ;

    int summeansquare = 0;
    for ( int y = 0 ; y < sy ; y+=4 )
      for ( int x = 0 ; x < sx ; x+=4 )
        summeansquare += square(newthumbnormal(x,y,0,0)-thumbnormalreference(x,y,0,0)) +
                         square(newthumbnormal(x,y,0,1)-thumbnormalreference(x,y,0,1)) ;

    if (summeansquare<bestsquare)
    {
      bestzscale = zscale;
      bestsquare = summeansquare;
    }

    zscale = bestzscale * (0.8f + 0.4f * (float(rand()) / float(RAND_MAX)));
  }

  IntMap BestDepth = depth * bestzscale;

  IntMap smallDepth = BestDepth ;
  const int    NB_TRIAL2 = 4;
  int scale[NB_TRIAL2] = {8,4,2,1};
  for (int i = 0; i<NB_TRIAL2 ; i++)
  {
    int sx = BestDepth.getSizeX() / scale[i];
    int sy = BestDepth.getSizeY() / scale[i];
    if (sx>10 && sy>10)
    {
      smallDepth = smallDepth . getResize (sx,sy);
      Image     smallNormalImage = normalmapimage.get_resize(sx,sy);
      IntMap a,b,c,d ;
      a=b=c=d=smallDepth;

      for ( int y = 1 ; y < sy-1 ; y++ )
        for ( int x = 1 ; x < sx-1 ; x++ )
        {
          unsigned char red   = smallNormalImage(x,y,0,0);
          unsigned char green = smallNormalImage(x,y,0,1);
          unsigned char blue  = smallNormalImage(x,y,0,2);

          Vector3 V(float (red)/255.0f - 0.5f, float (green)/255.0f - 0.5f, float (blue)/255.0f );
          V.setNormalize();
          int dx = int (round (V.x / V.z * MULTIPLY)) * 4 ;
          int dy = int (round (V.y / V.z * MULTIPLY)) * 4 ;

          int meandepthx = ( smallDepth(x-1,y) + smallDepth(x+1,y) ) / 2;
          a(x-1,y) = meandepthx-dx ;
          b(x+1,y) = meandepthx+dx ;

          int meandepthy = ( smallDepth(x,y-1) + smallDepth(x,y+1) ) / 2;
          c(x,y-1) = meandepthy-dy ;
          d(x,y+1) = meandepthy+dy ;
        }

      smallDepth = ( a + b + c + d ) / 4;
    }
  }

  return smallDepth ;
}


Image ImageGeneration::generateMask(const FloatMap & mask, float TresholdValue) 
{
	int sizex=mask.getSizeX();
	int sizey=mask.getSizeY();

	Image I(sizex,sizey,1,3);

	//red   is mask value > TresholdValue = bad matching color   
	//green is mask value < TresholdValue = good matching color

	// create picture using the dispmap information
	for (int y=0;y<sizey;y++)
		for (int x=0;x<sizex;x++) {
			// Displaced coordinates, so they are the corresponding coordinates on the right hand map
			float match;

			match=mask(x,y);

			float red=0.0f,green=0.0f,blue=0.0f;

			if ( match > TresholdValue )
				//no match = red
				red=0.25f+0.75f*match/3.0f;
			else
				//match = green
				green=1.0f-match/3.0f/TresholdValue;

			ColorF c=ColorF(red,green,blue);
			setPixelF(I,x,y,c);
		}

	return I;
	}

// generate a black and white image with all values > threshold as black, the rest is white
Image ImageGeneration::generateThreshold(const FloatMap & mask, float TresholdValue) 
{
	int sizex=mask.getSizeX(), sizey=mask.getSizeY();

	Image I(sizex,sizey,1,3);

	// create picture using the FloatMap information
	for (int y=0;y<sizey;y++)
		for (int x=0;x<sizex;x++) 
			if ( mask(x,y) > TresholdValue )
				//bigger than threshold = black
			  setPixel(I, x,y, Color(0, 0, 0));
      else
			  setPixel(I, x,y, Color(255, 255, 255));

	return I;
	}

#define PI 3.14159265f

Image ImageGeneration::generateOcclusion(const IntMap& dispmap, int radius, float contrast)
{
  // Reads image size, to create a new one same size
  int sizex = dispmap.getSizeX();
  int sizey = dispmap.getSizeY();

  Image image(sizex,sizey,1,3);

  // create picture using the dispmap information
  for (int y=0;y<sizey;y++)
    for (int x=0;x<sizex;x++) 
    {
      float origin = float( -dispmap ( x , y ) ) / MULTIPLY * contrast;
      
      const float MINIMUM_OCCLUSION = -45.0f;
      float currentAngle = MINIMUM_OCCLUSION;
      float sumAngle=0;
      int count=0;

      const float RAD2DEGREE=(180.0f/PI);    

      int BurstMapUsage=BurstMap::getMaxTableIndex(radius);

      for (int b=0;b<BurstMapUsage;b++)
      {
        int   bx = BurstMap::x(b);
        int   by = BurstMap::y(b);
        float d  = BurstMap::distance(b);

        if (d>0.0f) 
        {
          float m = d + 0.03f * d * d * d ; // defines a rampup : 1 2 6 9 12 17 23 31 40

          int nx = x + int (round (float (bx) * m));
          int ny = y + int (round (float (by) * m));

          bool isInsideLeftImage= (0<=nx && nx<sizex && 0<=ny && ny<sizey);

          if (isInsideLeftImage)
          {
            float nd = m*d ;
            float distant = float( -dispmap ( nx , ny ) ) / MULTIPLY * contrast;
            float newAngle = atan2( distant-origin, float(nd) ) * RAD2DEGREE;        
            currentAngle=max(currentAngle,newAngle);
            sumAngle+=newAngle;
            count++;
          }
        }
      }

      float value = (90.0f-currentAngle)/90.0f;
      limit    (value, 0.0f, 1.0f);

      setPixelF(image, x,y, ColorF(value,value,value));
    }

  return image;
}



Image ImageGeneration::generateSpecular(const IntMap& dispmap, const Image& ambient, int radius, float contrast)
{
  Image occlusion = generateOcclusion(dispmap,radius, contrast);

  // occlusion is too dark, let's boost up the values by 2
  //occlusion = ImageTools::mixMultiply(occlusion, 2.0f);

  // generate a 50% desaturated ambient map
  Image resizedAmbient=ambient.get_resize(occlusion.dimx(), occlusion.dimy());

  Image desaturateAmbient = ImageTools::computeDesaturate (resizedAmbient);
  desaturateAmbient = ImageTools::fade (desaturateAmbient, resizedAmbient, 0.5f);

  Image ret = ImageTools::mixMultiply(occlusion, desaturateAmbient);

  return ret;
}

Image ImageGeneration::generateFakeDepth(const Image & image)
{
  Image bw, newlayer, r;

  bw=ImageTools::computeDesaturate(image);

  // First layer : normal mode blur 20
  r   = bw.get_blur(FAKE_BLUR_RANGES[0]);  

  // 2nd layer : overlay blur 10
  newlayer = bw.get_blur(FAKE_BLUR_RANGES[1]) ;  
  r   = ImageTools::mixOverlay( r, newlayer, FAKE_BLUR_FADES [1]);

  // 3rd layer : overlay blur 5
  newlayer = bw.get_blur(FAKE_BLUR_RANGES[2]) ;
  r   = ImageTools::mixOverlay( r, newlayer, FAKE_BLUR_FADES [2]);

  // 4th layer : overlay blur 2.5
  newlayer = bw.get_blur(FAKE_BLUR_RANGES[3]) ;
  r   = ImageTools::mixOverlay( r, newlayer, FAKE_BLUR_FADES [3]);

  // 5th layer : overlay 
  newlayer = bw;
  r   = ImageTools::mixOverlay( r, newlayer, FAKE_BLUR_FADES [4]);

  return r;
}

