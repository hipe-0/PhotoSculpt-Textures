/*
 * UserInterrupt.h
 *
 *  Created on: 5 oct. 2009
 *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */

// 80 char per line standard :
/////////_/////////_/////////_/////////_/////////_/////////_/////////_/////////_

#pragma once
 
class UserInterrupt 
{
public:
  static void clear();
  static void orderStop();
  static void stopIfOrdered();
  const char* what() const;

private:
  static bool isInterrupt_ ;
};
