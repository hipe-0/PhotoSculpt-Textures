/*
 * Result.h
 *
 *  Created on: 18 september 2009
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */
#pragma once

#include "Image.h"
#include "IntMap.h"
#include "FloatMap.h"
#include "Photosculpt.h"
#include "Cropper.h"
#include "Obfuscate.h"
#include "Mutex.h"
#include "POV.h"

class Result
{
public:
  Result();

  //////////////////////////
  // INPUT
  // updates the map, the image, and the mode all at once
  
  void          setAlpha        (const PImage alpha);
  void          updateAll       (const IntMap& depthmap,const Image& diffuse, int mode, int shader);
  void          updateShaderOnly(int shader);

  ////////////////////////
  // GETTERS
  const Image   getImage        ();
  const IntMap  getMap          ();
  PFloatMap     getFloatMap     ();

  ////////////////////////
  // MODE CHANGE AND QUERY
  void          setFlatMode     ();
  void          setSeamlessMode ();
  void          cancelMode      ();
  bool          isResult        ();
  bool          isFlat          () const;
  bool          isSeamless      () const;
  void          clear           ();

  IntMap        getOriginalMap  ();
  int           getOriginalSizeX();
  int           getOriginalSizeY();
  int           getModelSizeX   ();
  int           getModelSizeY   ();

  int           getMode         () const;
  int           getShader       () const;

  void          defineCropArea  (float x1, float y1, float x2, float y2);
  void          cancelCropArea  ();
  bool          isCrop          () const;

  bool          isSquare        () const;
  Cropper       getCropper      (); // cannot be "const" as the mutex won't compile
  Cropper*      getCropperPointer();
  void          setCropper      (Cropper c);

  void          startAnimation  ();


  /////////////////////
  // Public result data

  // 3d parameter
  ofloat             azimuth_ , zenith_ , roll_         ;
  ofloat             xTrans_ ,  yTrans_ , zTrans_       ;
  float              zScale_                            ;
  ofloat             modelScale_                        ;
  oint               subdivision_                       ;
  obool              isAnimation_                       ;
  obool              isInvertNormalX_, isInvertNormalY_ ;
  POV                bestPOV_                           ;

private:

  ////////////////////////
  // PRIVATE METHODS
  Image  computeBaseTexture_        (int mode)           ;
  Image  computeBaseTexture_        (int mode, const IntMap& workmap, const Image& image); // should be const, but the mutex won't compile
  IntMap computeSeamlessMap_        (const IntMap& map)  ;
  IntMap setMapAndComputeSeamless_  (const IntMap& map)  ;
  void   clear_                     ()                   ;
  Image  computeTexture_            ()                   ;

  ////////////////////////
  // PRIVATE MEMBERS

  // map variables :
  IntMap       originalMap_   ;
  IntMap       workMap_, borderedMap_, seamlessUncropedMap_;
  IntMap       finalMap_      ;

  // Images
  Image        frontPhoto_    ;
  Image        finalImage_    ;
  PImage       alpha_         ;
  int          shader_        ;
  obool        isFlatMap_     ;
  oint         modeNb_        ;
  Cropper      cropper_       ;
  Mutex        mutex_         ;
};

