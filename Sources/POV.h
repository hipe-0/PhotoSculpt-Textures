#pragma once

#include "Vector3.h"
#include "FloatVector.h"
#include "VectorMap.h"
#include "FloatMap.h"

/// POV stands for Point Of View. 
/// It contains all data needed to project a 3D model on a 2D screen using perspective
///
class POV
{
public:
  POV         ();
  bool        convert3DTo2D(Vector3 vertexin3D, int sx, int sy, float& pointin2Dx, float& pointin2Dy ) const;

  float       getZScale           () const;
  float       getFocalLength      () const;
  float       getDistort          () const;
  void        setZScale           (float zscale     ) ;
  void        setFocalLength      (float focallength) ;
  void        setDistort          (float distort    ) ;

  FloatVector get                 () const;
  void        set                 (FloatVector& floatvector);

  Vector3     computeCameraPositionLeft  (int imagesizex, int imagesizey) const;
  Vector3     computeCameraPositionRight (int imagesizex, int imagesizey) const;
  VectorMap   generateMapOfFlatObject    (int sx, int sy)                 const;
  VectorMap   generateMapOfCoordinates   (const FloatMap& depths)         const;

  void        trace() const;

private:
  float posX_    ; // position of object along camera horizontal axis
  float posY_    ; // position of object along camera vertical axis
  float posZ_    ; // position of object along camera axis that points to where the camera is looking at

  float focal_   ;

  float zenith_  ; // 3 angles of rotation
  float azimuth_ ;
  float roll_    ;

  float zScale_  ;

  float distort_ ;
};
