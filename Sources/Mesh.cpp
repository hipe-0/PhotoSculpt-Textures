#include "Mesh.h"
#include "PhotoSculptTools.h"
#include "ImageGeneration.h"
#include "ImageTools.h"
#include "Limit.h"
#include "Options.h"

Mesh::Mesh(void)
{
}

Mesh::~Mesh(void)
{
}

int Mesh::getSizeX() const { return x_.getSizeX(); }
int Mesh::getSizeY() const { return x_.getSizeY(); }

void Mesh::create(const FloatMap& depths, float focallengthinmm)
{
  initXYZUV_    (depths, focallengthinmm);
}

Vector3 Mesh::computeDepthTo3D(const FloatMap& depths, int x, int y, float focallengthinmm, float distort)
{
  return computeDepthTo3D(depths(x,y),x,y,depths.getSizeX(),depths.getSizeY(), focallengthinmm, distort);
}

Vector3 Mesh::computeDepthTo3D(float depth, int x, int y, int sx, int sy, float focallengthinmm, float distort)
{
  if (  GlobalOptions.isHalfSpherePreview_ )
  {
    const float PI = 3.141592f;
    Vector3 v   ( (float(x) / float(sx-1) + 0.5f ) * PI ,
                  (float(y) / float(sy-1)        ) * PI ,
                  - depth   / float(sx-1)               );

    Vector3 pointSphere ( -sin (v.x) * sin (v.y) , -cos (v.y), -cos (v.x) * sin (v.y) ) ;
 
    const float RADIUS = 0.5f;
    const float fade   = 1.0f - fabs(1.0f - v.y/PI*2.0f );
    return pointSphere * (RADIUS + v.z*PI*fade) + Vector3 (0.5f, 0.5f * sy / sx, 0.0f );
  }

  if (GlobalOptions.isPerspectiveCorrected_)
  {
    float xcenter = 0.5f;
    float ycenter = 0.5f * float(sy-1) / float(sx-1);

    // convert from pixel
    float x3D = float(x) / float(sx - 1) - xcenter ;
    float y3D = float(y) / float(sx - 1) - ycenter ;

    // pincushion or barrel deformation
    if (distort != 0.0f)
    {
      float K = x3D * x3D + y3D * y3D ;
      x3D -= x3D * K * distort ;
      y3D -= y3D * K * distort ;
    }
    
    float z3D;
    if (x >= 0 && y >= 0 && x < sx && y < sy) 
      z3D = - depth / float(sx - 1) ;
    else 
      z3D = 0.0f;

    if (focallengthinmm <= 0.0f )
      focallengthinmm = 50.0f ; // 50 mm is a standard lens

    float focaldistance = focallengthinmm / 36.0f ; // 36mm = standard width of a photographic negative
    float zcamera = focaldistance - z3D;

    float zlimit = 0.2f;
    if (zcamera < zlimit)
    {
      z3D = zlimit - focaldistance;
      zcamera = zlimit;
    }

    // project in perspective
    // Computes the 3d coordinates of the vertex    
    x3D = x3D * zcamera / focaldistance + xcenter;
    y3D = y3D * zcamera / focaldistance + ycenter;

    return Vector3(x3D, y3D, z3D);
  }
  else
    // not focal perspective correction, witch is much simpler  
    return Vector3(float(x) / float(sx-1), 
                   float(y) / float(sx-1), 
                   - depth  / float(sx-1) );
}

void Mesh::initXYZUV_(const FloatMap& depths, float focallengthinmm)
{
  int sx = depths.getSizeX(), sy = depths.getSizeY();
  
  x_ = y_ = z_ = u_ = v_ = FloatMap(sx,sy);

  for (int y = 0; y < sy ; y ++)
    for (int x = 0; x < sx ; x ++)
    {
      // Computes the 3d coordinates of the vertex
      Vector3 v=computeDepthTo3D(depths, x,y, focallengthinmm);

      x_(x,y) = v.x ;
      y_(x,y) = v.y ;
      z_(x,y) = v.z ;
      u_(x,y) = float(x) / float(sx - 1);
      v_(x,y) = float(y) / float(sy - 1);
    }
}

Image Mesh::image()
{
  int sx = z_.getSizeX(), sy = z_.getSizeY();
  if ( sx <= 1 || sy <=1 ) 
    return Image();

  const int M = 5;

  Image i(sx * M ,sy * M, 1,3);
  i.fill(255);

  for (int y = 0; y < sy-1 ; y ++)
    for (int x = 0; x < sx-1 ; x ++)
    {
      int a = round ( float(x_(x,y)) * float(sx) * M);
      int b = round ( float(y_(x,y)) * float(sx) * M);
      limit(a, 0, sx * M);
      limit(b, 0, sy * M);

      int ax = round ( float(x_(x+1,y)) * float(sx) * M);
      int bx = round ( float(y_(x+1,y)) * float(sx) * M);
      limit(ax, 0, sx * M);
      limit(bx, 0, sy * M);

      int ay = round ( float(x_(x,y+1)) * float(sx) * M);
      int by = round ( float(y_(x,y+1)) * float(sx) * M);
      limit(ay, 0, sx * M);
      limit(by, 0, sy * M);

      unsigned char red[3]  = {100,0,0};
      unsigned char green[3] = {0,100,0};
      i.draw_line(a,b, ax, bx, red);
      i.draw_line(a,b, ay, by, green);
    }

  return i;
}

const FloatMap& Mesh::getAllVertexCoordsX() const {  return x_;  }
const FloatMap& Mesh::getAllVertexCoordsY() const {  return y_;  }
const FloatMap& Mesh::getAllVertexCoordsZ() const {  return z_;  }

float  Mesh::getVertexX(int x, int y) const 
{
  return x_.get(x,y);
}

float  Mesh::getVertexY(int x, int y) const 
{
  return y_.get(x,y);
}

float  Mesh::getVertexZ(int x, int y) const 
{
  return z_.get(x,y);
}

float  Mesh::getVertexU(int x, int y) const 
{
  return u_.get(x,y);
}

float  Mesh::getVertexV(int x, int y) const 
{
  return v_.get(x,y);
}

bool Mesh::isEmpty() const
{
  return x_.isEmpty() || y_.isEmpty() || z_.isEmpty();
}

Vector3 Mesh::getNorm(int x, int y) const
{
  int sizex = x_.getSizeX(), sizey = x_.getSizeY();

  int xmin = x-1;  int xmax = x+1;
  int ymin = y-1;  int ymax = y+1;

  limit( xmin, 0, sizex-1);  limit( xmax, 0, sizex-1);
  limit( ymin, 0, sizey-1);  limit( ymax, 0, sizey-1);

  Vector3 pos0(x_ (xmin, y   ) ,  y_ (xmin, y   ) ,  z_ (xmin, y   ));
  Vector3 pos1(x_ (xmax, y   ) ,  y_ (xmax, y   ) ,  z_ (xmax, y   ));
  Vector3 pos2(x_ (x,    ymin) ,  y_ (x,    ymin) ,  z_ (x,    ymin ));
  Vector3 pos3(x_ (x,    ymax) ,  y_ (x,    ymax) ,  z_ (x,    ymax ));

  return normalize((pos1-pos0)^(pos3-pos2));
}
