#include "POVFinder.h"
#include "Frame.h"
#include "PhotoSculptTools.h"
#include "Limit.h"
#include "Convert.h"
#include "Perspective.h"
#include "Mesh.h"
#include "UserInterrupt.h"
#include "FunctionOptimizer.h"
#include "Square.h"
#include "Frame.h"

POVFinder::POVFinder(void)
{
  currentBestScore_ = -1.0f ; // undefined
}

void POVFinder::init(const Image& left, const Image& right, const Image& alpha,
                     const VectorMap& vectormap, const POV& pointofview, int maxsize)
{
  int sx = vectormap.getSizeX(), sy = vectormap.getSizeY();
  Frame::limit(sx,sy, maxsize, maxsize);

  sourceL_  = left .get_resize(sx,sy, ANY, ANY, CIMG_INTERPOLATION_MOVING_AVERAGE)  ;
  sourceR_  = right.get_resize(sx,sy, ANY, ANY, CIMG_INTERPOLATION_MOVING_AVERAGE) ;
  if (!alpha.is_empty())
    alpha_    = alpha.get_resize(sx,sy) ;

  currentBestPov_ = pointofview ;

  map_      = vectormap.getResizeAndMultiply(sx,sy) ;

  depths_   = convertToFloat ( getPerspCorrectedMap(map_, currentBestPov_) ) / MULTIPLY;
  //depths_   = convertToFloat ( getPerspCorrectedHMap(vectormap) ) / MULTIPLY;
}

POV POVFinder::getCurrentBestPOV()
{
  return currentBestPov_;
}
  
float POVFinder::getCurrentBestScore  ()
{
  return currentBestScore_;
}

float absolute(float a)  {  if (a > 0.0f) return a; else return -a; }

bool get2DPointFromXYDepth(const POV& pointofview, const FloatMap &workdephts, int x, int y, 
                           float& pointin2Dx, float& pointin2Dy, float focallength, float distort )
{
  Vector3 vertexin3D = Mesh::computeDepthTo3D(workdephts, x, y, focallength, distort );

  int sx   = workdephts.getSizeX(), sy = workdephts.getSizeY();
  bool is3DPointExistIn2D = pointofview.convert3DTo2D(vertexin3D, sx,sy, 
                                                      pointin2Dx, pointin2Dy);
  return is3DPointExistIn2D;
}

#define SCORE_MALUS_4_PIX   1000
#define SCORE_MALUS_1_PIX   1000
float SCORE_FRAME_PERCENT = 10.0f;   // a frame around the image is not computed

float POVFinder::computeScore_(const POV& pointofview, int nbsamplesperline)
{
  float score      = 0.0f ; // start with best score ever
  int   countpixel = 0    ;

  int sx   = map_.getSizeX(), sy = map_.getSizeY();
  float xmin = sx*SCORE_FRAME_PERCENT/100;  float xmax = sx*(100-SCORE_FRAME_PERCENT)/100;
  float ymin = sy*SCORE_FRAME_PERCENT/100;  float ymax = sy*(100-SCORE_FRAME_PERCENT)/100;

  FloatMap workdephts = depths_ * pointofview.getZScale();

  bool isalpha = !alpha_.is_empty();
  float pixelstep = float (sx + sy) / 2.0f / float( nbsamplesperline );
  if (pixelstep<1.0f) pixelstep = 1.0f;

  for   (float yy = ymin; yy < ymax ; yy += pixelstep)
    for (float xx = xmin; xx < xmax ; xx += pixelstep)
    {
      int x = round(xx);
      int y = round(yy);

      bool isok = true;
      if (isalpha)  
        isok = alpha_(x,y,0,0) >= 127 ; // white = ok

      if (isok)
      {
        float xr = x + float(map_.x(x,y)) / MULTIPLY;
        float yr = y + float(map_.y(x,y)) / MULTIPLY;

        if (xr>=0 && yr>=0 && xr < sx && yr < sy)
        {
          float pointin2Dx, pointin2Dy;

          bool is3DPointExistIn2D = get2DPointFromXYDepth(pointofview, workdephts, 
            x, y, pointin2Dx, pointin2Dy, pointofview.getFocalLength() , pointofview.getDistort()    );

          if (is3DPointExistIn2D)
          {
            /* // centerweighted ?
            float factorx = 1.0f - absolute(xr - sx/2) / (sx/2) ;
            float factory = 1.0f - absolute(yr - sy/2) / (sy/2) ;
            float factor = min(factorx, factory);
            limit ( factor, 0.0f, 1.0f ); //? not tested
            */

            // distance between the displaced point: (xr,yr) and the 3d point
            // converted to 2D: (pointin2Dx, pointin2Dy)

            float sqx = xr - pointin2Dx, sqy = yr - pointin2Dy;
            float squaredistance = square(sqx) + square(sqy);

            if (squaredistance > 16.0f) // 16 = 4 pixels
              score += SCORE_MALUS_4_PIX;
            else if (squaredistance > 1.0f) // 16 = 1 pixels
              score += SCORE_MALUS_1_PIX;
            
            float sqrelativex = sqx/sx , sqrelativey = sqy/sy;

            const float FAVOR_Y_RELATIVELY_TO_X = 5.0f;
            const float MULTIPLICATOR_FOR_CONVENIENCE = 1000000.0f;
            score += (square(sqrelativex) + square(sqrelativey) * FAVOR_Y_RELATIVELY_TO_X) *
                     MULTIPLICATOR_FOR_CONVENIENCE ; // * factor;

            countpixel++;
          }
        }
      }
    }
  
  float score1 = score / float(countpixel);
  
  // defines 4 points at 25% and 75% of each side
  int L=sx/4, R=3*sx/4, U=sy/4, D=3*sy/4;

  float Y00map = float( map_.y(L,U) ) / MULTIPLY + U ;
  float Y10map = float( map_.y(R,U) ) / MULTIPLY + U ;
  float Y01map = float( map_.y(L,D) ) / MULTIPLY + D ;
  float Y11map = float( map_.y(R,D) ) / MULTIPLY + D ;

  float heightl_map = (Y01map - Y00map) / float ( sy ); // left
  float heightr_map = (Y11map - Y10map) / float ( sy ); // right

  float dummy, Y00_3D, Y10_3D, Y01_3D, Y11_3D ;
  bool is3DPointExistIn2D00 = get2DPointFromXYDepth(pointofview, workdephts, 
    L, U, dummy, Y00_3D, pointofview.getFocalLength() , pointofview.getDistort() );
  bool is3DPointExistIn2D10 = get2DPointFromXYDepth(pointofview, workdephts, 
    R, U, dummy, Y10_3D, pointofview.getFocalLength() , pointofview.getDistort() );
  bool is3DPointExistIn2D01 = get2DPointFromXYDepth(pointofview, workdephts, 
    L, D, dummy, Y01_3D, pointofview.getFocalLength() , pointofview.getDistort() );
  bool is3DPointExistIn2D11 = get2DPointFromXYDepth(pointofview, workdephts, 
    R, D, dummy, Y11_3D, pointofview.getFocalLength() , pointofview.getDistort() );

  float heightl_3D = (Y01_3D - Y00_3D) / float ( sy ) ; // left
  float heightr_3D = (Y11_3D - Y10_3D) / float ( sy ) ; // right

  float score2 = ( square(heightl_map-heightl_3D) + square(heightr_map-heightr_3D) ) * 1000.0f ;
  
  if (countpixel > 0)
    return score1 + score2 ;
  else
    return 100000.0f ; // bad value
}

POVFinder* tempPOVFinderPtr;

#define DEPTH_POV_UPDATE_FREQUENCY 100

float POV_function(FloatVector floatvector)
{
  POV tempPOV;
  tempPOV.set(floatvector);
  
  static int count = 0;
  if ((count++) % DEPTH_POV_UPDATE_FREQUENCY == 0)
    tempPOVFinderPtr->updateDepths(tempPOV);

  return tempPOVFinderPtr->computeScore_(tempPOV, tempPOVFinderPtr->getNbSamplePerLine());
}

void POVFinder::updateDepths(const POV& pointofview)
{
  depths_   = convertToFloat ( getPerspCorrectedMap(map_, pointofview) ) / MULTIPLY;
}

POV POVFinder::findBestView()
{
  FloatVector mask (currentBestPov_.get().getSize());

  mask[0] = 0.001f;  // pos x
  mask[1] = 0.001f;  // pos y
  mask[2] = 0.01f;   // pos z
  mask[3] = 0.001f;  // 1.0f;  // focal
  mask[4] = 1.0f;    // azimuth angle
  mask[5] = 1.0f;    // zenith angle
  mask[6] = 0.1f;    // roll angle
  mask[7] = 1.0f;    // zscale
  mask[8] = 0.001f;  // distort

  return findBestView(mask, 20);
}

POV POVFinder::findBestView(FloatVector mask, int nbsamplesperline)
{
  const int NB_ITER = 1;

  FloatVector initparameters = currentBestPov_.get();

  tempPOVFinderPtr = this;

  limit (nbsamplesperline, 1, 100);
  nbSamplePerLine_ = nbsamplesperline ;

  FloatVector bestparameters = initparameters;

  FunctionOptimizer optimizer;
  bestparameters = optimizer.minimize( POV_function, 
                                       bestparameters, NB_ITER, mask);

  currentBestScore_ = optimizer.getBestScore();

  UserInterrupt::stopIfOrdered();

  currentBestPov_.set(bestparameters);
  //trace(); // debug only
  
  return currentBestPov_;
}

int POVFinder::getMapSizeX() const 
{
  return map_.getSizeX();
}

int POVFinder::getMapSizeY() const 
{
  return map_.getSizeY();
}

VectorMap POVFinder::generateMapOfCoordinates  () const
{
  return currentBestPov_.generateMapOfCoordinates(depths_);
}

void POVFinder::trace() const
{
  Image scoreimage = computeScoreImage_ (currentBestPov_, nbSamplePerLine_);
  
  unsigned char color[3] = {0,255,0};
  char txt[100]="";
  sprintf(txt, "score %.3f ", currentBestScore_);
  scoreimage.draw_text(txt, 0,0, color);

  SAVE_IMAGE(scoreimage , "POVFinderScore.jpg"    );
  SAVE_IMAGE(sourceR_   , "POVFinderTarget.jpg"   );

  Image bestmatchimage = generateImage(currentBestPov_);
  sprintf(txt, "focal %.1f ", currentBestPov_.getFocalLength() );
  bestmatchimage.draw_text(txt, 0,0, color);

  SAVE_IMAGE(bestmatchimage, "POVFinderTargetBestMatch.jpg");

  FloatVector bestparameters = currentBestPov_.get();
  Image parameters(200,120,1,3);
  parameters.fill(255);
  for (int i = 0; i < bestparameters.getSize() ; i ++ )
  {
    unsigned char color[3] = {0,255,0};
    char txt[100]="";
    sprintf(txt, "parameter %d : %.5f ", i, bestparameters[i]);
    parameters.draw_text(txt, 0,i*10, color);
  }
  SAVE_IMAGE(parameters, "POVFinderParameters.jpg"    );

  Image camera(300,30,1,3);
  Vector3 cameraposR = currentBestPov_.computeCameraPositionRight(sourceL_.dimx(), sourceR_.dimy());
  Vector3 cameraposL = currentBestPov_.computeCameraPositionLeft (sourceL_.dimx(), sourceR_.dimy());
  float camxR=cameraposR.x, camyR=cameraposR.y, camzR=cameraposR.z;
  float camxL=cameraposL.x, camyL=cameraposL.y, camzL=cameraposL.z;
  camera.fill(255);
  sprintf(txt, "camera Right X Y Z %.5f %.5f %.5f", camxR, camyR, camzR );
  camera.draw_text(txt, 0,0, color);

  // verif
  float rx,ry;
  bool isok = currentBestPov_.convert3DTo2D(cameraposR, sourceL_.dimx(), sourceR_.dimy(), rx,ry);
  if (isok) camera.draw_text("displayable", 0,10, color);
  else      camera.draw_text("not displayable (ok)", 0,10, color);

  sprintf(txt, "camera Left X Y Z %.5f %.5f %.5f", camxL, camyL, camzL );
  camera.draw_text(txt, 0,20, color);

  SAVE_IMAGE(camera, "POVFinderCamera.jpg"    );
}

int POVFinder::getNbSamplePerLine() const
{
  return nbSamplePerLine_;
}
