#include "POV.h"
#include <stdlib.h>
#include "Limit.h"
#include "Error.h"

POV::POV()
{
  posX_    = 0.0f  ;
  posY_    = 0.0f  ;
  posZ_    = 0.0f  ;

  focal_   = 50.0f ; // 50 mm is a default focal length

  zenith_  = 0.0f  ;  // 0 degree
  azimuth_ = -15.0f ; // -15 degree
  roll_    = 0.0f  ;  // 0 degree

  zScale_  = 3.0f  ;
  distort_ = 0.0f  ;
}

bool POV::convert3DTo2D(Vector3 vertexin3D, int imagesizex, int imagesizey, float& pointin2Dx, float& pointin2Dy ) const
{
  float xcenter = 0.5f;
  float ycenter = 0.5f * float(imagesizey-1) / float(imagesizex-1);

  Vector3 eyeToTarget = vertexin3D - Vector3 ( xcenter, ycenter, 0.0f);

  // computes rotations
  float const PI = 3.141592653f;
  eyeToTarget = rotateX(eyeToTarget, zenith_  * (PI / 180));
  eyeToTarget = rotateY(eyeToTarget, azimuth_ * (PI / 180));
  eyeToTarget = rotateZ(eyeToTarget, roll_    * (PI / 180));

  eyeToTarget = eyeToTarget - Vector3(posX_, posY_, posZ_) ;

  // at this point eyeToTarget.z is inverted front to back
  float focaldistance = focal_ / 36.0f ;
  eyeToTarget.z = - eyeToTarget.z + focaldistance ;
  
  float zlimit = 0.2f ;
  if (eyeToTarget.z > zlimit)
  {
    // project in perspective
    float rx = eyeToTarget.x / eyeToTarget.z * focaldistance ;
    float ry = eyeToTarget.y / eyeToTarget.z * focaldistance ;

    // pincushion or barrel deformation
    if (distort_ != 0.0f)
    {
      float K = rx * rx + ry * ry ;
      rx += rx * K * distort_ ;
      ry += ry * K * distort_ ;
    }

    pointin2Dx = (rx + xcenter) * imagesizex ;
    pointin2Dy = (ry + ycenter) * imagesizex ;

    return true;
  }
  else
    return false;
}

Vector3 POV::computeCameraPositionRight(int imagesizex, int imagesizey) const
{
  float focaldistance = focal_ / 36.0f;

  Vector3 camerapos (0.0f , 0.0f , focaldistance);

  camerapos = camerapos + Vector3 (posX_, posY_, posZ_);
  
  float const PI = 3.141592653f;
  camerapos = rotateZ(camerapos, -roll_    * (PI / 180));
  camerapos = rotateY(camerapos, -azimuth_ * (PI / 180));
  camerapos = rotateX(camerapos, -zenith_  * (PI / 180));

  Vector3 initialpos  = Vector3 ( 0.5f, 0.5f * float(imagesizey-1) / float(imagesizex-1), 0.0f);

  camerapos = camerapos + initialpos ;
  
  return camerapos;
}

Vector3 POV::computeCameraPositionLeft(int imagesizex, int imagesizey) const
{
  DEBUG(computeCam);
  float focaldistance = focal_ / 36.0f;
  DEBUG_VALUE("  focaldistance", focaldistance);
  Vector3 camerapos = Vector3 ( 0.5f, 0.5f * float(imagesizey-1) / float(imagesizex-1), focaldistance);

  return camerapos;
}

float POV::getZScale() const
{
  return zScale_ ;
}

float POV::getFocalLength() const
{
  return focal_ ;
}

float POV::getDistort() const
{
  return distort_ ;
}

void POV::setZScale(float zscale)
{
  zScale_ = zscale ;
}

void POV::setFocalLength(float focallength)
{
  focal_ = focallength;
}

void POV::setDistort(float distort)
{
  distort_ = distort;
}

FloatVector POV::get() const
{
  return FloatVector (posX_ , posY_ , posZ_, focal_ , zenith_ , 
                      azimuth_ , roll_ , zScale_, distort_ );
}

void POV::set(FloatVector& floatvector)
{
  if (floatvector.getSize() != 9) 
    THROW_INTERNAL_ERROR( "Incorrect parameters (POV::set())" );

  posX_    = floatvector[0] ;
  posY_    = floatvector[1] ;
  posZ_    = floatvector[2] ;
  focal_   = floatvector[3] ;
  zenith_  = floatvector[4] ;
  azimuth_ = floatvector[5] ;
  roll_    = floatvector[6] ;
  zScale_  = floatvector[7] ;
  distort_ = floatvector[8] ;
}

#include "FloatMap.h"
#include "Mesh.h"

VectorMap POV::generateMapOfFlatObject  (int sx, int sy) const
{
  VectorMap returned(sx, sy);

  for   (int y = 0; y < sy ; y ++)
    for (int x = 0; x < sx ; x ++)
    {
      Vector3 vertexin3D = Mesh::computeDepthTo3D(0.0f, x, y, sx, sy,
                                                  getFocalLength(), 0.0f);
      float pointin2Dx, pointin2Dy;
      if (convert3DTo2D(vertexin3D, sx,sy, pointin2Dx,pointin2Dy))
      {
        float mapx = (pointin2Dx - x ) * MULTIPLY;
        float mapy = (pointin2Dy - y ) * MULTIPLY;

        returned.set(x,y, round(mapx), round(mapy));
      }
      else
        returned.set(x,y, 0,0); // ??
    }

  return returned;
}

VectorMap POV::generateMapOfCoordinates  (const FloatMap& depths) const
{
  int sx   = depths.getSizeX(), sy = depths.getSizeY();
  VectorMap returned(sx, sy);

  for   (int y = 0; y < sy ; y ++)
    for (int x = 0; x < sx ; x ++)
    {
      Vector3 vertexin3D = Mesh::computeDepthTo3D(depths(x,y) * getZScale(), 
                                      x,y, sx,sy, getFocalLength(), 0.0f);
      float pointin2Dx, pointin2Dy;
      if (convert3DTo2D(vertexin3D, sx,sy, pointin2Dx,pointin2Dy))
      {   
        float mapx = (pointin2Dx - x ) * MULTIPLY;
        float mapy = (pointin2Dy - y ) * MULTIPLY;

        returned.set(x,y, round(mapx), round(mapy));
      }
      else
        returned.set(x,y, 0,0); // ??
    }

  return returned;
}

void POV::trace() const
{
  DEBUG_VALUE("  PositionX", posX_     );
  DEBUG_VALUE("  PositionY", posY_     ); 
  DEBUG_VALUE("  PositionZ", posZ_     ); 

  DEBUG_VALUE("  Focal length", focal_ );

  DEBUG_VALUE("  zenith",    zenith_   );
  DEBUG_VALUE("  azimuth",   azimuth_  );
  DEBUG_VALUE("  roll",      roll_     );

  DEBUG_VALUE("  zScale",    zScale_   );
  DEBUG_VALUE("  distort",   distort_  );
}