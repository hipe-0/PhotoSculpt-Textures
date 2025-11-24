#include "POVFinder.h"
#include "Frame.h"
#include "PhotoSculptTools.h"
#include "Limit.h"
#include "Mesh.h"

extern float SCORE_FRAME_PERCENT ;// a frame around the image is not computed

Image POVFinder::computeScoreImage_(const POV& pointofview, int nbsamplesperline) const
{
  int sx   = map_.getSizeX(), sy = map_.getSizeY();
  float pixelstep = float (sx + sy) / 2.0f / float( nbsamplesperline );
  if (pixelstep<1.0f) pixelstep = 1.0f;

  int xmin = sx*round(SCORE_FRAME_PERCENT)/100;  
  int ymin = sy*round(SCORE_FRAME_PERCENT)/100;  
  int xmax = sx*(100-round(SCORE_FRAME_PERCENT))/100;
  int ymax = sy*(100-round(SCORE_FRAME_PERCENT))/100;

  FloatMap workdephts = depths_ * pointofview.getZScale();
  
  Image image(sx,sy,1,3);
  image.fill(255);

  bool isalpha = !alpha_.is_empty();

  for   (int y = ymin; y < ymax ; y += 1)
    for (int x = xmin; x < xmax ; x += 1)
   {
      bool isok = true;
      if (isalpha)
        isok = alpha_(x,y,0,0) >= 127 ; // white = ok

      if (isok)
      {
        bool ishotpoint = false;
        float dummy;
        if ( modf( float(x-xmin) / pixelstep, &dummy) < 1.0f && 
             modf( float(y-ymin) / pixelstep, &dummy) < 1.0f )
          ishotpoint = true;

        Vector3 vertexin3D = Mesh::computeDepthTo3D(workdephts, x, y, 
            pointofview.getFocalLength(),
            pointofview.getDistort()   );

        float pointin2Dx, pointin2Dy;
        if (pointofview.convert3DTo2D(vertexin3D, sx,sy, pointin2Dx,pointin2Dy))
        {
          int rx = round(pointin2Dx);
          int ry = round(pointin2Dy);

          if ( pointin2Dx > 0.0f        && pointin2Dy > 0.0f         && 
               pointin2Dx < float(sx-1) && pointin2Dy < float (sy-1) )
          {
            float xr2 = x + float(map_.x(x,y)) / MULTIPLY;
            float yr2 = y + float(map_.y(x,y)) / MULTIPLY;

            // distance between the displaced value and the perspectively correct value
            float sqx = xr2 - pointin2Dx, sqy = yr2 - pointin2Dy;
            float squaredistance = sqx * sqx + sqy * sqy;

            if (squaredistance >= 1.0f && squaredistance < 16.0f) // 16 = 4 pixels
            { // red
              image(rx,ry,0,0) = 255;
              image(rx,ry,0,1) = 0;
              image(rx,ry,0,2) = 0;
            }
            else if (squaredistance >= 16.0f)
            { // black
              image(rx,ry,0,0) = 0;
              image(rx,ry,0,1) = 0;
              image(rx,ry,0,2) = 0;
            }
            else
            { // light gray
              image(rx,ry,0,0) = 240;
              image(rx,ry,0,1) = 240;
              image(rx,ry,0,2) = 240;
            }

            if (ishotpoint)
            { // blue
              image(rx,ry,0,0) = 0;
              image(rx,ry,0,1) = 0;
              image(rx,ry,0,2) = 255;
            }
          }
        }
      }
    }

  return image;
}


Image POVFinder::generateImage(const POV& pointofview) const
{
  int sx   = map_.getSizeX() , sy  = map_.getSizeY() ,
      sx2  = sourceL_.dimx() , sy2 = sourceL_.dimy() ;
 
  if (sx!=sx2 || sy!=sy2)
    THROW_INTERNAL_ERROR("Wrong arguments, Sizes do not match (in POVFinder::generateImage())");

  FloatMap workdephts = depths_ * pointofview.getZScale() ;

  Image image(sx,sy,1,3);
  image.fill(255);

  for (int y = 0; y < sy ; y ++)
    for (int x = 0; x < sx ; x ++)
    {
      // Computes the 3d coordinates of the vertex
      Vector3 vertexin3D = Mesh::computeDepthTo3D(workdephts, x, y,
          pointofview.getFocalLength(),
          pointofview.getDistort()    );

      Color c = getPixel(sourceL_,x,y);

      float pointin2Dx, pointin2Dy;
      bool isdisplayed = pointofview.convert3DTo2D(vertexin3D, sx,sy, pointin2Dx,pointin2Dy);

      if (isdisplayed)
      {
        if (pointin2Dx>0.0f && pointin2Dy>0.0f && pointin2Dx< float(sx-1) && pointin2Dy< float (sy-1))
          {
            unsigned char col [3] = {c.r , c.g, c.b};
            image.draw_point( round(pointin2Dx)  ,   round (pointin2Dy)   , col ); 
            image.draw_point( round(pointin2Dx)  ,   round (pointin2Dy+1) , col ); 
            image.draw_point( round(pointin2Dx+1),   round (pointin2Dy)   , col ); 
          }
      }
    }

  return image;
}


