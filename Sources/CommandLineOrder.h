/*
 * CommandLineOrder.h
 *
 *  Created on: 23 févr. 2011
  *      Author: Hippolyte Mounier, Copyright 2011 All rights reserved
 */

#pragma once

class CommandLineOrder
{
public:
  CommandLineOrder();
  void clear();

public:
   bool isForcedSculpt_  ;
   bool isForcedSaveMaps_;
   bool isForcedExit_    ;

   bool isObj_           ;
   bool isPly_           ;
   bool isStl_           ;
   bool isSculpt_        ;

   bool isDisplacement_  ;
   bool isNormal_        ;
   bool isSpecular_      ;
   bool isDiffuse_       ;
   bool isOcclusion_     ;
   bool isBump_          ;
   bool isAlpha_         ;
   bool isLimDisp_       ;
   bool is3D_            ;

   bool isMinimum_       ;

   bool isOutput_        ;
   bool isSoftwareMode_  ;
   bool isBatchSculpt_   ;

   char outputPath_        [MAX_PATH];
   char frontPhotoFileName_[MAX_PATH];
   char rightPhotoFileName_[MAX_PATH];

   int  maxStep_         ;
   bool isSquareTile_    ;
   bool is2nSize_        ;
};
