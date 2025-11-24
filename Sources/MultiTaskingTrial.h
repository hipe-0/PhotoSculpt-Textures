/*
 * MultiTaskingTrial.h
 *
 *  Created on: 2011/6/20
  *      Author: Hippolyte Mounier, Copyright 2011 All rights reserved
 */

#pragma once
#include "Image.h"
#include "Trace.h"
#include "Error.h"
#include "MultiTasking.h"

class MultiTaskingTrial: public Parallelable
{
private:
  Image image_;

public:
  void task  (int line);
  void launch();
};