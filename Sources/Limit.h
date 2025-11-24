/*
 * Limit.h
 *
 *  Created on: 16 avr. 2009
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */

#ifndef LIMIT_H_
#define LIMIT_H_

#include "Obfuscate.h"


inline void limit(int &value, int min, int max)
{
  if (value<min) value = min;
  if (value>max) value = max;
}
inline void limit(float &value, float min, float max)
{
  if (value<min) value = min;
  if (value>max) value = max;
}

#ifndef OBFUSCATED_VARIABLES_ARE_CLEAR
inline void limit(oint &value, int min, int max)
{
  if (value<min) value = min;
  if (value>max) value = max;
}
inline void limit(ofloat &value, float min, float max)
{
  if (value<min) value = min;
  if (value>max) value = max;
}
#endif

#endif /* LIMIT_H_ */