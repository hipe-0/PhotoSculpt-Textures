/*
 * ImageTools.cpp
 *
 *  Created on: 29 janv. 2009
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */
#include "ImageTools.h"
#include "ImageMath.h"
#include "Limit.h"

// parameter fadeValue : 0..1
Image ImageTools::fade(const Image& base, const Image& blend, float fadeValue)
{
  Image res=base;
  res.draw_image(blend, 0,0,0,0, fadeValue);

  return res;
}


Image ImageTools::mixOverlay(const Image& base, const Image& sprite)
{
  DEBUG(mixOve);
  int sizex=base.dimx();
  int sizey=base.dimy();

  Image res(sizex, sizey, 1,3);

  for   (int y=0;  y<sizey; y++)
    for (int x=0;  x<sizex; x++)
    {
	    res(x,y,0,0)=ChannelBlend_Overlay( sprite (x,y, 0,0), base (x,y, 0,0) );
	    res(x,y,0,1)=ChannelBlend_Overlay( sprite (x,y, 0,1), base (x,y, 0,1) );
	    res(x,y,0,2)=ChannelBlend_Overlay( sprite (x,y, 0,2), base (x,y, 0,2) );
	  }

  return res;
}

Image ImageTools::mixHardLight(const Image& base, const Image& sprite)
{
  DEBUG(mixHa);
  int sizex=base.dimx();
  int sizey=base.dimy();

  Image res(sizex, sizey, 1,3);

  for   (int y=0;  y<sizey; y++)
    for (int x=0;  x<sizex; x++)
    {
      res(x,y,0,0)=ChannelBlend_HardLight( sprite (x,y, 0,0), base (x,y, 0,0) );
	    res(x,y,0,1)=ChannelBlend_HardLight( sprite (x,y, 0,1), base (x,y, 0,1) );
	    res(x,y,0,2)=ChannelBlend_HardLight( sprite (x,y, 0,2), base (x,y, 0,2) );
	  }

  return res;
}

Image ImageTools::mixMultiply(const Image& base, const Image& sprite)
{
  int sizex=base.dimx();
  int sizey=base.dimy();

  Image res(sizex, sizey, 1,3);

  for (  int y=0;  y<sizey; y++)
    for (int x=0;  x<sizex; x++)
    {
      int Br = int(base  (x,y, 0,0));
	    int Bg = int(base  (x,y, 0,1));
	    int Bb = int(base  (x,y, 0,2));

      int Lr = int(sprite(x,y, 0,0));
	    int Lg = int(sprite(x,y, 0,1));
	    int Lb = int(sprite(x,y, 0,2));

      int Rr = Br * Lr / 255;
      int Rg = Bg * Lg / 255;
      int Rb = Bb * Lb / 255;

	    res(x,y,0,0)=unsigned char(Rr);
	    res(x,y,0,1)=unsigned char(Rg);
	    res(x,y,0,2)=unsigned char(Rb);
	  }

  return res;
}

Image ImageTools::mixMultiply(const Image& base, float value)
{
  int sizex=base.dimx();
  int sizey=base.dimy();

  Image res(sizex, sizey, 1,3);

  for (  int y=0;  y<sizey; y++)
    for (int x=0;  x<sizex; x++)
    {
      int Br = int(base  (x,y, 0,0));
	    int Bg = int(base  (x,y, 0,1));
	    int Bb = int(base  (x,y, 0,2));

      int Rr = int (round(float(Br) * value));
      int Rg = int (round(float(Bg) * value));
      int Rb = int (round(float(Bb) * value));

      limit(Rr,0,255);
      limit(Rg,0,255);
      limit(Rb,0,255);

	    res(x,y,0,0)=unsigned char(Rr);
	    res(x,y,0,1)=unsigned char(Rg);
	    res(x,y,0,2)=unsigned char(Rb);
	  }

  return res;
}

Image ImageTools::mixTint(const Image& base, int hue)
{
  int sizex=base.dimx();
  int sizey=base.dimy();

  Image res(sizex, sizey, 1,3);

  for (  int y=0;  y<sizey; y++)
    for (int x=0;  x<sizex; x++)
    {
      uint8 r = base (x,y, 0,0);
	    uint8 g = base (x,y, 0,1);
	    uint8 b = base (x,y, 0,2);

      float64 h,l,s;
      Color_RgbToHls(r,g,b, &h,&l,&s);

      h = double(hue) / 360.0f ;

      Color_HlsToRgb(h,l,s, &r,&g,&b);

	    res(x,y,0,0)=r;
	    res(x,y,0,1)=g;
	    res(x,y,0,2)=b;
	  }

  return res;
}

void ImageTools::drawImageOverlay (Image& base, const Image& sprite, int x, int y)
{
  int bsizex=base.dimx();    int bsizey=base.dimy();
  int ssizex=sprite.dimx();  int ssizey=sprite.dimy();

  for   (int sy=0;  sy<ssizey; sy++)
    for (int sx=0;  sx<ssizex; sx++)
      if (x+sx>=0 && x+sx<bsizex && y+sy>=0 && y+sy<bsizey)
      {
        uint8 br = base (x+sx,y+sy, 0,0);
	      uint8 bg = base (x+sx,y+sy, 0,1);
	      uint8 bb = base (x+sx,y+sy, 0,2);

        uint8 sr = sprite (sx,sy, 0,0);
	      uint8 sg = sprite (sx,sy, 0,1);
	      uint8 sb = sprite (sx,sy, 0,2);

	      base(x+sx,y+sy,0,0)=ChannelBlend_Multiply(sr,br);
	      base(x+sx,y+sy,0,1)=ChannelBlend_Multiply(sg,bg);
	      base(x+sx,y+sy,0,2)=ChannelBlend_Multiply(sb,bb);
      }
}

void ImageTools::drawImageLinearDodge (Image& base, const Image& sprite, int x, int y)
{
  int bsizex=base.dimx();    int bsizey=base.dimy();
  int ssizex=sprite.dimx();  int ssizey=sprite.dimy();

  for   (int sy=0;  sy<ssizey; sy++)
    for (int sx=0;  sx<ssizex; sx++)
    
      if (x+sx>=0 && x+sx<bsizex && y+sy>=0 && y+sy<bsizey)
      {
        unsigned int br = base (x+sx,y+sy, 0,0);
	      unsigned int bg = base (x+sx,y+sy, 0,1);
	      unsigned int bb = base (x+sx,y+sy, 0,2);

        unsigned int sr = sprite (sx,sy, 0,0);
	      unsigned int sg = sprite (sx,sy, 0,1);
	      unsigned int sb = sprite (sx,sy, 0,2);

	      base(x+sx,y+sy,0,0)=ChannelBlend_Screen(sr,br);
	      base(x+sx,y+sy,0,1)=ChannelBlend_Screen(sg,bg);
        base(x+sx,y+sy,0,2)=ChannelBlend_Screen(sb,bb);
      }
}


Image ImageTools::mixDesaturateRedChannelOnly(const Image& base, float fade)
{
  int sizex=base.dimx();
  int sizey=base.dimy();

  Image res(sizex, sizey, 1,3);

  for (  int y=0;  y<sizey; y++)
    for (int x=0;  x<sizex; x++)
    {
      uint8 r = base (x,y, 0,0);
	    uint8 g = base (x,y, 0,1);
	    uint8 b = base (x,y, 0,2);
      uint8 gray = uint8((int(r)+int(g)+int(b))/3);

	    res(x,y,0,0)=ChannelBlend_Alpha(r,gray,fade);
	    res(x,y,0,1)=ChannelBlend_Alpha(g,gray,fade);
	    res(x,y,0,2)=ChannelBlend_Alpha(b,gray,fade);
	  }

  return res;
}


Image ImageTools::mixDifference(const Image& base, const Image& sprite)
{
  int sizex=base.dimx();
  int sizey=base.dimy();

  Image res(sizex, sizey, 1,3);

  for   (int y=0;  y<sizey; y++)
    for (int x=0;  x<sizex; x++)
    {
      int Br = int(base  (x,y, 0,0));
	    int Bg = int(base  (x,y, 0,1));
	    int Bb = int(base  (x,y, 0,2));

      int Lr = int(sprite(x,y, 0,0));
	    int Lg = int(sprite(x,y, 0,1));
	    int Lb = int(sprite(x,y, 0,2));

      int Rr = Br - Lr ;
      int Rg = Bg - Lg ;
      int Rb = Bb - Lb ;

      limit(Rr,0,255);
      limit(Rg,0,255);
      limit(Rb,0,255);

	    res(x,y,0,0) = unsigned char(Rr);
	    res(x,y,0,1) = unsigned char(Rg);
	    res(x,y,0,2) = unsigned char(Rb);
	  }

  return res;
}

Image ImageTools::threshold(const Image& base, int threshold)
{
  int sizex=base.dimx();
  int sizey=base.dimy();

  Image res(sizex, sizey, 1,3);

  for   (int y=0;  y<sizey; y++)
    for (int x=0;  x<sizex; x++)
    {
      int Br = int(base  (x,y, 0,0));
      int Bg = int(base  (x,y, 0,1));
	    int Bb = int(base  (x,y, 0,2));

      int mean = (Br+Bg+Bb)/3;

      unsigned char r;
      if (mean > threshold) r = 255;
      else                  r = 0  ;

	    res(x,y,0,0) = r;	    res(x,y,0,1) = r;	    res(x,y,0,2) = r;
	  }

  return res;
}

Image ImageTools::mixOverlay(const Image& base, const Image& sprite, float fadeValue)
{
  const Image i = mixOverlay(base, sprite);
  return fade(base, i, fadeValue);
}


Image ImageTools::adjustExposure(const Image& main, const Image& use, 
					  float blurRadius, float fadeValue)
{
  const Image blurMain  = main.get_blur(blurRadius); // left
  const Image blurUsing = use. get_blur(blurRadius); // morphed

  Image exposureChange = blurUsing;
  exposureChange.draw_image( - blurMain, 0,0,0,0, 0.5f);

  const Image res = mixOverlay(main, exposureChange, fadeValue);

  return res;
}



Image ImageTools::computeDesaturate(const Image& image)
{
  int sizex=image.dimx();
  int sizey=image.dimy();

  Image r(sizex, sizey, 1,3);

  for (  int y=0;  y<sizey; y++)
    for (int x=0;  x<sizex; x++)
    {
      int Br = int(image (x,y, 0,0));
	    int Bg = int(image (x,y, 0,1));
	    int Bb = int(image (x,y, 0,2));

      // computes the mean of red green and blue, to get a medium gray
      int R=(Br + Bg + Bb) / 3 ;

	    r(x,y,0,0)=unsigned char(R);
	    r(x,y,0,1)=unsigned char(R);
	    r(x,y,0,2)=unsigned char(R);
	  }

  return r;
}

void ImageTools::repeatN(Image& texture, int nb, int framesizex, int framesizey)
{
  DEBUG(rep);
  if (nb<=0) return;

  Image res (texture.dimx()*nb+framesizex*2, texture.dimy()*nb+framesizey*2, 1, 3);
  int startx = 0, starty=0;
  if (framesizex>0) startx = 1;
  if (framesizey>0) starty = 1;

  for   (int y=-starty; y<nb+starty; y++)
    for (int x=-startx; x<nb+startx; x++)
      res.draw_image(texture,texture.dimx()*x+framesizex, texture.dimy()*y+framesizey);
  
  texture = res;
}