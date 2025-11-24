/*
 * PhotoSculptAnalysisDump.cpp
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

void addToImageOnDisk ( char * filename, Image insert, char * text)
{
  // load current image on disk
  Image i ( filename );
  int sizex = i.dimx();
  int sizey = i.dimy();

  if (insert.dimx() > sizex)
    sizex = insert.dimx();

  sizey += insert.dimy();

  Image resultimage (sizex, sizey, 1, 3);
  resultimage.fill(255);
  resultimage.draw_image(i, 0,0);
  resultimage.draw_image(insert, 0, i.dimy());

  unsigned char col[] = { 255,0,0 } ;
  resultimage.draw_text(text, 0, i.dimy()+5, col);

  resultimage.save(filename);
};

void SculptInProgress::dump(char * text)
{
#ifdef DEBUG_FILE_NAME_DUMP

  Image tex(getSizeX(),getSizeY(),1,3);
  tex.fill(255);
  Image i = View3D::generate3DImage(currentMap_, tex, 10,10, 
                                    2.0f/MULTIPLY);
  
  // image large enough so see text clearly
  if (getSizeX()>100)
    addToImageOnDisk(DEBUG_FILE_NAME_DUMP, i, text);
  else
    addToImageOnDisk(DEBUG_FILE_NAME_DUMP, i, "");

#endif
}

void SculptInProgress::initDump()
{
#ifdef DEBUG_FILE_NAME
  Image i(1,1,1,3);
  i.save(DEBUG_FILE_NAME);
#endif
}