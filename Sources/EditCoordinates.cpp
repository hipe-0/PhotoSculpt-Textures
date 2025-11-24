#include "EditCoordinates.h"
#include "Includes.h"

EditCoordinates::EditCoordinates()
{
  nb_ = 0;
}

void  EditCoordinates::get(float& lx, float& ly, float& rx, float& ry, int item) const
{
  if (item < 0 || item >= nb_)
    THROW_INTERNAL_ERROR("Wrong parameter (in EditCoordinates::get())");

  lx=lx_[item];  ly=ly_[item];  rx=rx_[item];  ry=ry_[item];
}

void  EditCoordinates::set(float lx, float ly, float rx, float ry, int item)
{
  if (item < 0 || item >= nb_)
    THROW_INTERNAL_ERROR("Wrong parameter (in EditCoordinates::set())");

  lx_[item] = lx;  ly_[item] = ly;  rx_[item] = rx;  ry_[item] = ry;
}

void  EditCoordinates::add(float lx, float ly, float rx, float ry)
{
  if (nb_ >= NB_MAX)
    return;

  nb_++;  

  // fill the newly created element
  set( lx, ly, rx, ry, nb_-1 );
  setType( nb_-1, EDIT_COORD_POINT);
}

void  EditCoordinates::del(int item)
{
  if (item < 0 || item >= nb_)
    THROW_INTERNAL_ERROR( "Wrong parameter (in EditCoordinates::getType())");

  set( -1.0, -1.0, -1.0, -1.0, item );
}

int   EditCoordinates::getNb() const
{
  return nb_;
}

void  EditCoordinates::setType( int item, int type)
{
  if (item < 0 || item >= nb_)
    THROW_INTERNAL_ERROR( "Wrong parameter (in EditCoordinates::setType())" );

  type_[item] = type;
}
int   EditCoordinates::getType( int item ) const
{
  if (item < 0 || item >= nb_)
    THROW_INTERNAL_ERROR( "Wrong parameter (in EditCoordinates::getType())");

  return type_[item] ;
}
