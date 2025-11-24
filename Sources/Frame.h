/*
 * Frame.h
 *
 *  Created on: 30 sept. 2009
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */

#ifndef FRAME_H_
#define FRAME_H_


#include "Frame.h"

class Frame
{
public:
  static void fit(int &sizex, int &sizey, int framex, int framey);
  static void limit(int &sizex, int &sizey, int framex, int framey);
};

inline void Frame::fit(int &sizex, int &sizey, int framex, int framey)
{
    int oldsizex=sizex;
    int oldsizey=sizey;

    sizex = framex;
    sizey = sizex * oldsizey / oldsizex;

    if (sizey>framey)
    {
       sizey = framey;
       sizex = sizey * oldsizex / oldsizey;
    }
}

inline void Frame::limit(int &sizex, int &sizey, int framex, int framey)
{
  if (sizex > framex || sizey > framey)
    fit(sizex, sizey, framex, framey);
}

#endif