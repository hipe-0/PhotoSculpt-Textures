/*
 * EditCoordinates.h
 *
 *  Created on: 30 sept. 2009
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */


#ifndef EDITCOORDINATES_H_
#define EDITCOORDINATES_H_

#define EDIT_COORD_POINT 0
#define EDIT_COORD_LINE  1

#include "Obfuscate.h"

class EditCoordinates
{
public:
  EditCoordinates();

  int   getNb() const;
  void  get(float& lx, float& ly, float& rx, float& ry, int item) const;
  void  set(float  lx, float  ly, float  rx, float  ry, int item);
  void  add(float  lx, float  ly, float  rx, float  ry);
  void  addAsLine(float  lx, float  ly, float  rx, float  ry);
  void  del(int item);
  void  clear();
  void  setType( int item, int type);
  int   getType( int item ) const;

private:

  oint nb_;

  static const int NB_MAX = 100;
  ofloat lx_[NB_MAX], ly_[NB_MAX],rx_[NB_MAX], ry_[NB_MAX];
  oint   type_[NB_MAX];
}; 


#endif