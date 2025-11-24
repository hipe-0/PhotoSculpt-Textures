#pragma once

#include "FloatMap.h"
#include "Image.h"
#include "Vector3.h"

class Mesh
{
public:
  Mesh(void);
  ~Mesh(void);

  void            create       (const FloatMap& depths, float focallengthinmm);

  Image           image     ();
  const FloatMap& getAllVertexCoordsX()    const ;
  const FloatMap& getAllVertexCoordsY()    const ;
  const FloatMap& getAllVertexCoordsZ()    const ;
  int             getSizeX  ()             const ;
  int             getSizeY  ()             const ;
  float           getVertexX(int x, int y) const ;
  float           getVertexY(int x, int y) const ;
  float           getVertexZ(int x, int y) const ;
  float           getVertexU(int x, int y) const ;
  float           getVertexV(int x, int y) const ;

  Vector3         getNorm   (int x, int y) const ;

  bool            isEmpty   () const;

  static Vector3  computeDepthTo3D(const FloatMap& depths, int x, int y, 
    float focallengthinmm = 50.0f, float distort = 0.0f);

  static Vector3  computeDepthTo3D(float depth, int x, int y, int sx, int sy,
    float focallengthinmm = 50.0f, float distort = 0.0f);

private:
  void initXYZUV_ ( const FloatMap & depths, float focal );

  FloatMap x_, y_, z_, u_, v_;
};
