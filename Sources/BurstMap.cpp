/*
 * BurstMap.cpp
 *
 *  Created on: 15 avr. 2009
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */

#include "BurstMap.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "Includes.h"

const int ACCELERATED_DISTANCE     = 4 ;
const int MAX_DENSITY     = 10 ;
const int MAX_DISTANCE    = 15 ;

// Memory Reservation table sizes
const int MAXBURST        = ( MAX_DISTANCE * MAX_DENSITY * 2 + 1 ) * 
                            ( MAX_DISTANCE * MAX_DENSITY * 2 + 1 ) ;

// size really used : 
int   BurstMap::size_     = -1 ;
int   BurstMap::density_  = -1 ;

int   BurstMap::x_     [MAXBURST];
int   BurstMap::y_     [MAXBURST];
float BurstMap::dist_  [MAXBURST];

const int MAX_REVERSE   = MAX_DISTANCE * MAX_DENSITY + 2 ;
int   BurstMap::reverseSearch_[MAX_REVERSE];


void BurstMap::init(int newsize, int newDensity) 
{
  DEBUG(BurstMa);
  if (size_ != -1)
    THROW_INTERNAL_ERROR("Burst Map created twice (in Init())");

  if (newsize<=0 || newsize > MAX_DISTANCE)
    THROW_INTERNAL_ERROR("wrong BurstMap argument size (in Init())");

  if (newDensity<=0 || newDensity > MAX_DENSITY)
    THROW_INTERNAL_ERROR("wrong BurstMap argument density (in Init())");

  density_ = newDensity ;
  size_    = newsize    ;

  const long int HUGE_VALUE = 100000 ;

  // initialize burst table
  int t[ MAX_DISTANCE * MAX_DENSITY * 2 + 1 ]
       [ MAX_DISTANCE * MAX_DENSITY * 2 + 1 ];

  int nb=( size_ * 2 + 1 )*( size_ * 2 + 1 );

  for (int j=-size_;j<size_+1;j++)
    for (int i=-size_;i<size_+1;i++)
	  {
      int distance = i * i + j * j ;
      // computes the squared distance for each i and j

	    if ( distance >= ACCELERATED_DISTANCE * ACCELERATED_DISTANCE )
		    if (rand() % 4 <= 1) // 50 % of values are canceled
			    distance = HUGE_VALUE;

      t[i + size_][ j + size_ ]= distance;
	  }

  for (int k = 0; k < nb; k ++)
  {
    // searches in the table for the shortest distance
    long int smallest = HUGE_VALUE ;

    for   (int j = - size_ ; j<size_+1; j++)
      for (int i = - size_ ; i<size_+1; i++)
      {
        int distance = t[i + size_][j + size_];

        if (distance <= smallest) 
        {
          smallest = distance;

          if ( k >= MAXBURST )
            THROW_INTERNAL_ERROR("BurstMap initialisation exceeds memory limits");

          x_   [k] = i ;
          y_   [k] = j ;
          dist_[k] = sqrt( float(distance) );
        }
      }

    t[x_[k]+size_][y_[k]+size_] = HUGE_VALUE; // "destroys"  value in the table so that it's not found again
  }

  // initialisation of the reverse table 
  reverseSearch_[0]=1;

  for   (int l=0; l<=size_ * density_; l++)
	  for (int k=nb-1 ; k>=0 ; k--)
	    if ( dist_[k] <= float(l) / float(density_) )
	    {
        if ( l >= MAX_REVERSE )
          THROW_INTERNAL_ERROR("Error in reverseSearch_ initialisation (in init())");

	      reverseSearch_[l] = k + 1;
        break;
	    }
}

int BurstMap::getMaxTableIndex(int scope)
{
  if (scope>size_*density_)
    scope = size_*density_ - 1 ;
  
  int nbElements = reverseSearch_ [scope];

  return nbElements;
}