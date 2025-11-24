/*
 * View3D.cpp
 *
 *  Created on: 16 avr. 2009
 *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */

#include "View3D.h"
#include "Vector3.h"
#include "ImageGeneration.h"
#include <math.h>
#include "Mutex.h"
#include "Includes.h"

Vector3 View3D::rotationAlongYX(Vector3 v, Vector3 center, float azimuth, float zenith)
{
  float radAzimuth=azimuth/360.0f*3.141592f*2.0f;
  float radZenith=zenith/360.0f*3.141592f*2.0f;

  // Offset relative to center
  Vector3 newv=v-center;

  // double rotation in space
  newv= rotateX(newv, radZenith);
  newv= rotateY(newv, radAzimuth);

  // Offset back off center
  return newv+center;
}


Vector3 View3D::rotationAlongYX(Vector3 v,  float azimuth, float zenith)
{
  return rotationAlongYX(v,Vector3(),azimuth,zenith);
}


// Computes a float value between 0.0 (no light) and 1.0 (full light)
//
float View3D::shadow(Vector3 normalVector, Vector3 lightVector)
{
  float lenghtLight=  length (lightVector);
  float lengthNormal= length (normalVector);

  if (lenghtLight==0.0f || lengthNormal==0.0f)
    return 0.0f;

  // Lambert shader
  float shade=(normalVector*(-lightVector)) / lenghtLight / lengthNormal;

  if (shade<0.0f) 
	return 0.0f;
  if (shade>1.0f) 
	return 1.0f;

  return shade;
}


// Computes the normal vector to this location on the displacement map
Vector3 View3D::compute_normal(const IntMap& d,int x,int y, float zScale)
{
  float variationx,variationy;

  if (x==0)
  {
    int begin=    d(x,y);
    int after=    d(x+1,y);
    int wayafter= d(x+2,y);
    variationx=-0.5f*float(wayafter)+2.0f*float(after)-1.5f*float(begin);
  }
  else if (x==d.getSizeX()-1)
  {
    int waybefore=d(x-2,y);
    int before=   d(x-1,y);
    int end= 	  d(x,y);
    variationx=-0.5f*float(waybefore)+2.0f*float(before)-1.5f*float(end);
  }
  else 
  {
    int before= d(x-1,y);
    int after=  d(x+1,y);
    variationx=float(after-before) / 2.0f;
  }

  if (y==0)
  {
    int begin=   d(x,y);
    int after=   d(x,y+1);
    int wayafter=d(x,y+1);
    variationy=-0.5f*wayafter+2.0f*after-1.5f*begin;
  }
  else if (y==d.getSizeY()-1)
  {
    int waybefore=d(x,y-2);
    int before=   d(x,y-1);
    int end=      d(x,y);
    variationy=-0.5f*float(waybefore)+2.0f*float(before)-1.5f*float(end);
  }
  else 
  {
    int before= d(x,y-1);
    int after= 	d(x,y+1);
    variationy=float(after-before)/2.0f;
  }
  Vector3 vx(1.0f, 0.0f, variationx*zScale);
  Vector3 vy(0.0f, 1.0f, variationy*zScale);

  Vector3 normal=normalize(vx^vy);

  return normal;
}



// Computes the normal vector to this location on the displacement map
Vector3 View3D::computeNormal(const FloatMap& d,int x,int y)
{
  float variationx,variationy;

  if (x==0)
  {
    float begin=    d(x,y);
    float after=    d(x+1,y);
    float wayafter= d(x+2,y);
    variationx=-0.5f*wayafter+2.0f*after-1.5f*begin;
  }
  else if (x==d.getSizeX()-1)
  {
    float waybefore=d(x-2,y);
    float before=   d(x-1,y);
    float end= 	  d(x,y);
    variationx=-0.5f*waybefore+2.0f*before-1.5f*end;
  }
  else 
  {
    float before= d(x-1,y);
    float after=  d(x+1,y);
    variationx=(after-before) / 2.0f;
  }

  if (y==0)
  {
    float begin=   d(x,y);
    float after=   d(x,y+1);
    float wayafter=d(x,y+1);
    variationy=-0.5f*wayafter+2.0f*after-1.5f*begin;
  }
  else if (y==d.getSizeY()-1)
  {
    float waybefore=d(x,y-2);
    float before=   d(x,y-1);
    float end=      d(x,y);
    variationy=-0.5f*waybefore+2.0f*before-1.5f*end;
  }
  else 
  {
    float before= d(x,y-1);
    float after= 	d(x,y+1);
    variationy=(after-before)/2.0f;
  }
  Vector3 vx(1.0f, 0.0f, -variationx);
  Vector3 vy(0.0f, 1.0f, -variationy);

  Vector3 normal=normalize(vx^vy);

  return normal;
}

Image View3D::generateNormalMapImage(const IntMap& tablecorresp, float zScale, bool isInvertX, bool isInvertY) 
{
  int sizex=tablecorresp.getSizeX();
  int sizey=tablecorresp.getSizeY();

  Image I(sizex,sizey,1,3);

  // create picture using the dispmap information
  for (int j=0;j<sizey;j++)
    for (int i=0;i<sizex;i++) 
    {
      Vector3 normal=compute_normal(tablecorresp, i,j, zScale);

      // Working on X axis first (red channel)
      float colr;
      if (isInvertX)
        colr = float(round(127.0f + normal.x * 127.0f));
      else
        colr = float(round(127.0f - normal.x * 127.0f));

      if (colr<0.0f)
        colr = 0.0f;
      else if (colr>255.0f)
        colr = 255.0f;

      // working on Y axis second (green)
      float colg;
      if (isInvertY)
        colg = float(round(127.0f + normal.y * 127.0f));
      else
        colg = float(round(127.0f - normal.y * 127.0f));

      if (colg<0.0f)
        colg = 0.0f;
      else if (colg>255.0f)
        colg = 255.0f;

      // working on Z axis last (blue)
      float colb=float(round(normal.z*255.0f));
      if (colb<0.0f)
        colb=0.0f;
      else if (colb>255.0f)
        colb=255.0f;

      I(i,j,0,0)=(unsigned char)(colr);
      I(i,j,0,1)=(unsigned char)(colg);
      I(i,j,0,2)=(unsigned char)(colb);
    }

  return I;
}

/// Generates a 3D view of the displacemap using texture image
//
// info : 	It uses Lambert shading and Phong+Blinn specular
//
// info : 	Color r0,v0,b0 is reserved for background element only
// info : 	Output image is set 20% bigger than displacemap
// Warning : 	"image" parameter should have same size as displaceMap
//
Image View3D::generate3DImage(const IntMap&  displaceMap, Image image, 
                              float azimuth, float zenith, float zScale)
{
  int sizex=displaceMap.getSizeX();
  int sizey=displaceMap.getSizeY();

  int imagesizex=image.dimx();
  int imagesizey=image.dimy();

  if (sizex != imagesizex || sizey != imagesizey )
    THROW_INTERNAL_ERROR( "Internal error : model and texture do not match (generate3DImage())");

  float const MATERIAL_BLINN_SPECULARITY=0.1f;
  float const MATERIAL_BLINN_POWER=30; // also called hardness

  // creates a new image
  // it is defined as 20% bigger than necessary to create a "black" frame around the subject
  Image outputImage(sizex*12/10,sizey*12/10,1,3);

  // 2D offset into image
  int planeOffsetx=sizex/10;
  int planeOffsety=sizey/10;

  outputImage.fill(255);

  //light is set to front top right corner by default
  Vector3 lightDirection     ( 0.5f, -0.5f, -1.0f );
  Vector3 viewPointDirection ( 0.0f, 0.0f,  -1.0f );
  Vector3 halfWay=(lightDirection + viewPointDirection) * 0.5f;

  for (int yy=sizey-2;yy>=0;yy--)
    for (int xx=sizex-2;xx>=0;xx--)
    {
      // Painter algorithm, backwards is first
      int x,y;
      Vector3 left(-1,0,0), up(0,1,0);
      bool isIncreasingX=rotationAlongYX (left, azimuth, zenith).z>=0.0f;
      bool isIncreasingY=rotationAlongYX (up,   azimuth, zenith).z>=0.0f;

      if (isIncreasingX)
        x=xx;
      else
        // reversing loop order
        x=sizex-2-xx;

      if (isIncreasingY)
        y=yy;
      else
        // reversing loop order
        y=sizey-2-yy;

      // computes the grid depth for this coordinate x,y
      int z = -displaceMap(x,y);
      int z2= -displaceMap(x+1,y);
      int z3= -displaceMap(x,y+1);
      int z4= -displaceMap(x+1,y+1);

      unsigned char col[3];
      col[0] = image(x,y,0,0);
      col[1] = image(x,y,0,1);
      col[2] = image(x,y,0,2);

      Vector3 normalDirection=-compute_normal(displaceMap,x,y,zScale);

      Vector3 spaceLoc1( float(x),   float(y),   -z*zScale);
      Vector3 spaceLoc2( float(x+1), float(y),   -z2*zScale);
      Vector3 spaceLoc3( float(x),   float(y+1), -z3*zScale);
      Vector3 spaceLoc4( float(x+1), float(y+1), -z4*zScale);

      Vector3 centerOfRotation(sizex/2,sizey/2,0);

      spaceLoc1 = rotationAlongYX (spaceLoc1, centerOfRotation, azimuth, zenith);
      spaceLoc2 = rotationAlongYX (spaceLoc2, centerOfRotation, azimuth, zenith);
      spaceLoc3 = rotationAlongYX (spaceLoc3, centerOfRotation, azimuth, zenith);
      spaceLoc4 = rotationAlongYX (spaceLoc4, centerOfRotation, azimuth, zenith);

      // Conversion 3D -> 2D to prepare display on screen
      // the conversion is simply made by removing z (= no perspective effect)
      int planeX1=int(round(spaceLoc1.x));
      int planeY1=int(round(spaceLoc1.y));
      int planeX2=int(round(spaceLoc2.x));
      int planeY2=int(round(spaceLoc2.y));
      int planeX3=int(round(spaceLoc3.x));
      int planeY3=int(round(spaceLoc3.y));
      int planeX4=int(round(spaceLoc4.x));
      int planeY4=int(round(spaceLoc4.y));

      // shadow calculation using normal data
      normalDirection=rotationAlongYX (normalDirection, azimuth, zenith);
      float shade=shadow(-normalDirection, lightDirection);

      shade=0.1f+shade*0.9f;
      //shade=0.0;

      // Apply shade to color
      col[0] = (unsigned char) (round(col[0] * shade));
      col[1] = (unsigned char) (round(col[1] * shade));
      col[2] = (unsigned char) (round(col[2] * shade));

      // specularity is defined by the Phong-Blinn shader
      float specular=normalDirection*halfWay;
      if (specular<0.0f)
        specular=0.0f;
      specular=pow(specular,MATERIAL_BLINN_POWER);
      if (specular>1.0f)
        specular=1.0f;

      specular*=MATERIAL_BLINN_SPECULARITY;

      col[0] = (unsigned char) (round(col[0] + (255-col[0]) * specular));
      col[1] = (unsigned char) (round(col[1] + (255-col[1]) * specular));
      col[2] = (unsigned char) (round(col[2] + (255-col[2]) * specular));

      outputImage.draw_triangle(planeX1+planeOffsetx,planeY1+planeOffsety,
          planeX2+planeOffsetx,planeY2+planeOffsety,
          planeX3+planeOffsetx,planeY3+planeOffsety,col);
      outputImage.draw_triangle(planeX2+planeOffsetx,planeY2+planeOffsety,
          planeX3+planeOffsetx,planeY3+planeOffsety,
          planeX4+planeOffsetx,planeY4+planeOffsety,col);
    }
  
  return outputImage;
}
