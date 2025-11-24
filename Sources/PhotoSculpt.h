/*
 * PhotoSculpt.h
 *
 *  Created on: 3 févr. 2009
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */

// 80 char per line standard :
/////////_/////////_/////////_/////////_/////////_/////////_/////////_/////////_

#pragma once

#include "ImageGeneration.h"
#include "ProgressionBar.h"
#include "BurstMap.h"
#include "Parameter.h"
#include "EditCoordinates.h"
#include "SculptInProgress.h"
#include "Obfuscate.h"
#include "Pyramid.h"
#include "Mutex.h"

class PhotoSculpt
{
public:  // PUBLIC METHODS
  PhotoSculpt     ();
  void            init(Image& LImage, Image& RImage);
 
  void            setEditCoordinates(const EditCoordinates& e);
  EditCoordinates getEditCoordinates() const;

  // PYRAMIDAL DOWNSAMPLED IMAGES MANAGEMENT
  /// current level number of the images of the pyramid being worked on
  int             getCurrentMapLevelUNSTABLE() const;
  int             computeLevel      ()   const;

  /// get the max level number of image in the pyramid,
  // this correspond to the highest value / smallest images of the pyramid
  oint            getMaxLevel       () const;

  // get the level number that matches this size (or immediate smaller size)
  int             computeLevelUsingSize(int sizex, int sizey) const;

  /// get the size of the current left image for this current pyramid level
  int             getStableSizeX    () const;
  int             getStableSizeY    () const;
  int             getUNSTABLESizeX  () const;
  int             getUNSTABLESizeY  () const;  
  void            clear            ();  /// clear the sculpt object

  /// test to check if at least one image is there on both pyramid
  bool            hasDownSamples   () const;
  bool            isModel          () const;

  const Image&    getLeftImageUNSTABLE () const;
  const Image&    getRightImageUNSTABLE() const;
  Image           getTexture           ()      ;
  Image           computeLeftImage ();
  Image           computeRightImage();
  Image           computeLeftImage (int level);
  Image           computeRightImage(int level);

  // DISPLACEMENT MAP MANAGEMENT

  /// get the matching score of the current displacement map relatively to the
  //  left and right image
  float           getScore            ();

  /// Compute the percentage of good pixels ( that match left to right image)
  // that's a way to compute quality
  float           getGoodPixelPercent (float qualityThreshold = 0.01f);

  void            setMap              (VectorMap& map, bool issculpted);
  void            resetMap            ();
  VectorMap       getMap              () const;
  PIntMap         computeIntMap       ();
  void            addLocalDetailToMap (PIntMap map);

  /// returns 0 for the first step of optimization, 1 for the next step, 2 for
  // the next step...
  oint            getStepNoUNSTABLE   () const;
  oint            computeStep         () const;

  /// says if this is the first step of the optimization
  bool            isFirstStep         () const;
  bool            isLastStep          () const;

  /// launches the full optimization or this level of displacement map
  //void            optimizeThisLevel   ();
  bool            growAndOptimizeThisLevel();
  bool            growAndOptimizeThisLevel(Parameter parameters);

  /// once the optimization step is done for the current level, this function
  //  will double the displacement map size and double all values.
  bool            interpolateToNextLevel   ();
  bool            downsampleToPreviousLevel();
  
  /// dig for better quality using random parameters over 7 sample images
  static void     searchForAllParameters();

  // apply current parameters over 7 sample images
  static float    testSamples();

  void            setSculptInProgress          ();
  void            retrieveSculptInProgress     ();
  void            clearCurrentSculptInProgress ();

  float           estimateTimeToEnd            ();
  int             getSubStep                   ();
  void            setAlpha                     (PImage alpha) ;
  POV             getPOV                       () const       ;
  void            setPOV                       (const POV& pointofview);
  void            setFocalLength               (float focallength);
  bool            isSculpted();

private :

  // IMAGE OPERATION
  void            prepareImages_               ();
  void            adjustExposureLocal_         ();
  void            adjustExposureGlobal_        ();
  Image           generateMorphedRightImage_   ();

  // PRIVATE MEMBERS
  Pyramid          pyramid_            ;

  SculptInProgress sculptInProgress_   ;

  // Current displacement map under work
  PVectorMap       currentMapStable_   ;
  POV              pointofview_        ;

  EditCoordinates  editCoordinates_    ;

  PImage           stableAlpha_        ;
  bool             isSculpted_         ;
  Mutex            mutex_              ;
};

typedef SmartPointer<PhotoSculpt, false> PPhotoSculpt;


