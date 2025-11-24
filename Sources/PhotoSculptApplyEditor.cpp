/*
 * PhotoSculptApplyEditor.cpp
 *
 *  Created on: 05 oct. 2009
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */
 
// 80 char per line standard :
/////////_/////////_/////////_/////////_/////////_/////////_/////////_/////////_


#include "RESOURCE.H"
#include "PhotoSculpt.h"
#include "Timer.h"
#include "Limit.h"
#include "ImageGeneration.h"
#include "View3d.h"

void SculptInProgress::applyVector_(int lx, int ly, Vector v, int radius, VectorMap &m)
{
  int sizex = m.getSizeX();
  int sizey = m.getSizeY();

  int squareradius = radius * radius ;
  // searches all neibouring pixels around x,y until it hits one that's too different
  // than center pixel. Difference Criteria is the square distance bigger than threshold
  
  for (int y=0; y<sizey; y++)
    for (int x=0; x<sizex; x++)
    {
      int distsq = (lx-x)*(lx-x) + (ly-y)*(ly-y) ;

      if (distsq < squareradius)
      {
        Vector current=m.get(x, y);

        int dist = int(round(sqrtf(float(distsq))));

        int falloff; const int MAXFALLOFF=1000;
        if (squareradius>0)
          falloff = MAXFALLOFF * dist / radius; // ranges from 0 to MAXFALLOFF
        else
          falloff = MAXFALLOFF;

        Vector target = ( v * (MAXFALLOFF-falloff) ) / MAXFALLOFF;

        int distancetarget  = target.x()*target.x()  + target.y()*target.y();
        int distancecurrent = current.x()*current.x()+ current.y()*current.y();

        if (distancetarget>distancecurrent)
          m.set(x, y, target);
      }
    }
}


int FtoI( float valuefloat , int size) 
{
  return int ( round (float ( size - 1 ) * valuefloat ) );
}


void SculptInProgress::stepApplyEdit(const EditCoordinates& e)
{
  int count = e.getNb();
  int sizex = currentMap_->getSizeX();
  int sizey = currentMap_->getSizeY();

  VectorMap m(sizex,sizey);
  m.fill(0);

  if (count>0)
  {
    for (int i=0; i<count; i++)
    {
      float lx,ly,rx,ry;
      e.get(lx,ly,rx,ry,i);

      // if the new coordinate is within reasonable bound
      if (lx>=0.0f && lx<=1.0f && ly>=0.0f && ly<=1.0f &&rx>=0.0f && rx<=1.0f && ry>=0.0f && ry<=1.0f)
      {
        int ilx=FtoI(lx, sizex);
        int ily=FtoI(ly, sizey);
        int irx=FtoI(rx, sizex);
        int iry=FtoI(ry, sizey);

        Vector objective(  (irx - ilx) * int(MULTIPLY)  , 
                           (iry - ily) * int(MULTIPLY)  );

        Vector current = currentMap_->get(ilx, ily);

        Vector v=objective-current;

        const float RADIUS_OF_EDIT_POINT = 0.2000f;

        int radius = int (round (float(sizex) * RADIUS_OF_EDIT_POINT));

        // apply vector and neighbourgs to current map in progress
        applyVector_(ilx, ily, v, radius, m );
      }
    }
  }

#ifdef DEBUG_EDIT_MAP
  Image i = ImageGeneration::generateDepth(m,1.0,127);
  i.save(DEBUG_EDIT_MAP);
#endif

  *currentMap_ = *currentMap_ + m ;
}
