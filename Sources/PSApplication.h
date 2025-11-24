/*
 * PSApplication.h
 *
 *  Created on: 25 févr. 2009
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */

#pragma once

#include <windows.h>
#include <shellapi.h>
#include "View3D.h"
#include "ExportObj.h"
#include "ExportStl.h"
#include "VectorMap.h"
#include "ButtonManager.h"

#include "DirectXControl.h"
#include "Result.h"
#include "Registry.h"
#include "EditCoordinates.h"
#include "Obfuscate.h"
#include "ProgressDialog.h"
#include "ToolTip.h"
#include "MouseTracker.h"
#include "MultiQuery.h"
#include "ImageFrame.h"
#include "Options.h"
#include "ThreadId.h"
#include "Includes.h"
#include "GraphicUserInterface.h"
#include "Sources.h"
#include "CommandLineOrder.h"

class PSApplication
{
public:
  PSApplication   ();
  void            init                      (HINSTANCE hInstance);
  void            createMainInterface       (HINSTANCE hInstance, int nCmdShow);

  bool            openFileDialog            (char* result,char* result2, HWND& hwnd, char* title, char* filter);
  bool            openFileDialog            (char* result, HWND& hwnd, char* title, char* filter);
  bool            saveAsFileDialog          (char* result, HWND& hwnd, char* txt_filter, char* txt_ext);
  void            savePath                  (const char* filename);
  bool            loadLastPath              (char* filename);
  bool            loadSamplePath            (char* filename);

  int             detectUserAction          ();

  void            clearSculptAndResults     ();

  void            runExtraction             ();
  void            runManualEdit             ();
  bool            executeCommandedExtraction(CommandLineOrder command);

  void            loadPhotos(char * filenameL, char * filenameR);
  void            loadDialog                (bool isLeft);
  void            loadDialogIdenticalLR     ();
  void            loadDepthMapDialog        ();
  void            loadSculptFileDialog      ();
  void            loadSculptFile            (char * FileName);
  void            loadRecent                (int index      );
  void            loadFromCommandLine       ();
  void            loadSculptFromCommandLine (char * lpCmdLine);
  void            loadAlphaDialog           ();
  void            closeAlpha                ();
  void            loadAdditionalPhotoDialog ();
  void            limitImageSize            ();

  bool            copyImageToClipboard      ();
  bool            pasteImageLFromClipboard  ();
  bool            pasteImageRFromClipboard  ();

  void            saveAsDialog              ();
  void            saveAsImageDialog         ();
  void            saveAsModelDialog         ();
  void            saveOneImage              (char * filename, int type, int subdivision);
  void            saveOneImageExt           (char * filename, int type, int subdivision);
  void            saveAsSculptFile          (char * filename, int subdivision);
  void            saveAsObjFile             (char * filename, int subdivmodel, int subdivimage);
  void            saveAsStlFile             (char * filename, int subdivision);
  void            saveAsPlyFile             (char * filename, int subdivmodel, int subdivimage);
  void            save16BitTiffDialog       ();
  void            saveAsPerMultiQueryDialog (const MultiQuery& query);
  void            saveAsPerMultiQuery       (const MultiQuery& query, const char * filename);
  void            saveAsPerCommandLine      (const CommandLineOrder& command);
  void            actionSaveMultiple        ();

  void            doAction                  (oint action);
  void            doActionOnNewThreadIfNecessary            (oint action);
  void            doActionOnFirstThread     (oint action);
  void            postActionOnFirstThread   (oint action);
  bool            askBeforeDelete  ();


  void            requestRedraw             ();

  void            initializeProgressionBar  ();

  void            extractDisplaceMap        ();
  void            extractDisplaceMap3Photos ();

  void            startAnimation            ();
  void            stopAnimation             ();
  int             getAnimationTimerId       ();
  void            animate                   ();

  bool            isLastStep                ();

  void            setButtonLogic            ();

  ////////////////DISPLAY
  void            reinit3DDisplay           ();
  void            stop3DDisplay             ();
  void            change3DShader            (int shadercode);
  void            displayToWindow           ();

  Image &         getLeftImage              ();
  Image &         getRightImage             ();
  VectorMap       getMap                    ();

  void            clearCurrentSculptInProgress();

  void            actionAbout               ();
  void            dropFiles                 (const HDROP& hDrop);
  void            setTrackMouseLeave        ();
  void            resetTrackMouseLeave      ();
  void            analyseAndWarnUser        ();
  void            setPage                   (int page)  ;

  void            resizeMainWindow          (int newSizeX, int newSizeY);
  Cropper         getCropper                ()          ;
  void            setCropper                (Cropper c) ;
  void            updateResult              ()          ;
  void            createResultFromSculpt    ()          ;
  void            finalReloadResult         ()          ;

  void            setEditCoordinates        ( const EditCoordinates& e );
  EditCoordinates getEditCoordinates        () const    ;
  void            warning                   (char * warningtxt, const char * warningressource);
  void            applyOptions              ();
  void            repaintStoredButtons      ( int hue, float saturation );
  void            searchCameraPosition      ();
  void            actionAnimateModel        ();
  void            clearAll                  ();
  void            actionCropEdit            ();
  void            actionEditPinning         ();
  void            actionAddDetail           ();
  void            actionSubstractDetail     ();
  void            actionSetFocalDialog      ();
  void            actionSetBlurRadiusDialog ();
  void            actionFlatOptionsDialog   ();
  void            actionEditSelection       ();
  void            actionMove3D              (int action);
  void            actionBatchSculpt         (int mode);
  void            actionSwitchOption        (bool &boolean, char* textiftrue, char* textiffalse);
  UserInfo        getUserInfo               ();
  void            openLoadDialogWithDoubleClick(int x, int y);


  // VARIABLES
  GraphicUserInterface interface_   ;

  volatile bool   isSculpting_ ;

private:

  void            saveInterfacePosition            ();

  static void     saveFileNameInRegistry           (const Registry& reg, 
                                                    char * filenameL, char * filenameR);
  static bool     loadfromRegistryFileName         (const Registry& reg,
                                                    char * returnLeftImage, 
                                                    char * returnRightImage, int index);

  void            setButtonLogicTopZone_           ();
  void            setButtonLogicMainArea_          ();
  void            setButtonLogicNotSculpting_      ();
  void            setButtonLogicCurrentlySculpting_();
  void            setButtonLogicShaders_           ();

  void            updateMenuRecentFiles_           ();
  void            rotateImages(float leftimageangle,  float centerimageangle, 
                               float rightimageangle, float alphaangle      );
  void            multiplyDepth                    (float value);
  void            actionDownSample                 ();
  void            actionShellExecute               (char* text);
  void            zeroDisplacementMap              ();


  // VARIABLES

  // Input image
  Source          sources_                    ;

  Result          result_                     ;

  PhotoSculpt     sculpt_                     ;
  PhotoSculpt     sculptAdditional_           ;

  Mutex           mutex_                      ;

  Registry        registry_ , registryAll_    ;

  MouseTracker    track_                      ;
  
  obool           isCurrentModelJustSculpted_ ;

  CommandLineOrder commandLine_               ;
};

