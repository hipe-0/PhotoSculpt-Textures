/*
 * Round.h
 *
 *  Created on: 16 may 2009
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */

#ifndef ROUND_H_
#define ROUND_H_

inline int round(float x)
{
return int(x > 0.0 ? x + 0.5 : x - 0.5);
}


#endif /* ROUND_H_ */
