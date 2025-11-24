/*
 * PSApplication.cpp
 *
 *  Created on: 18 avr. 2009
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */
#define _WIN32_WINNT 0x0500

#include <fstream>

#include "PSApplication.h"
#include "resource.h"
#include "Win32Tools.h"
#include "ExportObj.h"
#include "Timer.h"
#include "Limit.h"
#include "Includes.h"
#include "DirectXControl.h"
#include "UserInterrupt.h"
#include "ImageGeneration.h"
#include "unzip.h"
#include "File.h"
#include "IntMap.h"
#include "Convert.h"
#include "Win32Tools.h"
#include "Options.h"
#include "MorphingEditor.h"

void displayTimeCompletedDialog(Timer & starttimer)
{
  char msg[200] = "";
  float minutes = starttimer.getSeconds()/60.0f;
  int m = (int) minutes;
  int s = int(round((minutes-(float)m)*60.0f));
      
  int mem = Win32Tools::getMaxMemUsedMb();

  sprintf(msg,"Extraction completed in %d minutes %d seconds, using %d mb", m, s, mem);
  MESSAGE(msg);
}

void PSApplication::runExtraction()
{
  DEBUG(runEx);
  NEVER_EXECUTE_ON_WINDOWS_THREAD();

  UserInterrupt::clear();
  if (isSculpting_)
    THROW_INTERNAL_ERROR("Already sculpting");

  isSculpting_ = true ;
  interface_.sleepIcon_.startSleepIcon(interface_.windowsHandle_);
  
  saveFileNameInRegistry(registry_,sources_.leftFileName_, sources_.rightFileName_);
  updateMenuRecentFiles_();
  requestRedraw();

  Win32Tools::clearMemUsed();

  limitImageSize();

  try
  {
    Timer starttimer;

    // Runs the Photosculpt extraction
    if (sources_.additionalImage_.is_empty())
      extractDisplaceMap();
    else
      extractDisplaceMap3Photos();

    interface_.sleepIcon_.stopSleepIcon(interface_.windowsHandle_);
    initializeProgressionBar();

    if (!commandLine_.isForcedExit_ && !commandLine_.isOutput_)
      displayTimeCompletedDialog(starttimer);
  }
  catch (UserInterrupt &)
  {
    interface_.sleepIcon_.stopSleepIcon(interface_.windowsHandle_);
    initializeProgressionBar();
    commandLine_.clear();
    requestRedraw();
  }
  INTERFACE_CATCH_BLOC(SCULPTING)

  isSculpting_ = false ;
  UserInterrupt::clear();

  updateMenuRecentFiles_();

  clearCurrentSculptInProgress();
  
  saveAsPerCommandLine(commandLine_);
   
  if (commandLine_.isForcedExit_)
    postActionOnFirstThread(ACTION_QUIT_AT_ONCE);
}

void PSApplication::initializeProgressionBar()
{
  if (sculpt_.isModel())
  {
    interface_.progressionBar_.setDisplay(interface_.image_);

    if (!interface_.isSculptBarMinimised_)
      interface_.progressionBar_.setGeometry(PROGRESSION_BAR_LEFT + interface_.bottomBar_.getPositionX(),  
                              PROGRESSION_BAR_UP   + interface_.bottomBar_.getPositionY() ,
                              PROGRESSION_BAR_RIGHT+ interface_.bottomBar_.getPositionX(),  
                              PROGRESSION_BAR_DOWN + interface_.bottomBar_.getPositionY() );
    else
      interface_.progressionBar_.setGeometry(PROGRESSION_BAR_LEFT + interface_.bottomMiniBar_.getPositionX(),  
                              PROGRESSION_BAR_UP   + interface_.bottomMiniBar_.getPositionY() ,
                              PROGRESSION_BAR_RIGHT-100+ interface_.bottomMiniBar_.getPositionX(),  
                              PROGRESSION_BAR_DOWN + interface_.bottomMiniBar_.getPositionY() );

    interface_.progressionBar_.setLineColor(MAIN_COLOR_R,MAIN_COLOR_G,MAIN_COLOR_B);

    interface_.progressionBar_.setPower(6.0f);
  }
}

void PSApplication::clearSculptAndResults()
{
  DEBUG(cleScuRe);
  interface_.changeTitle("");

  result_.clear();
  interface_.directX_.cleanD3D();

  if (sculpt_.isModel())
    sculpt_.clear();
  if (sculptAdditional_.isModel())
    sculptAdditional_.clear();

  sculpt_          .clear();
  sculptAdditional_.clear();
  isCurrentModelJustSculpted_ = false;

  doActionOnFirstThread(ACTION_MODE_SOURCES);
}

void PSApplication::startAnimation()
{
  result_.isAnimation_=true;
  SetTimer(interface_.windowsHandle_, ID_ANIMATION_TIMER, NB_MILISECONDS_ANIMATION, NULL);
}

void PSApplication::stopAnimation()
{
  KillTimer(interface_.windowsHandle_,ID_ANIMATION_TIMER);
  result_.isAnimation_=false;
}

int PSApplication::getAnimationTimerId()
{
  return ID_ANIMATION_TIMER;
}

void PSApplication::animate()
{
  const float PI = 3.141592f;
  const float MOUSE_ROTATE_FACTOR = 4.0f / (PI / 180.0f) ;

  if (result_.isAnimation_)
    result_.roll_ -= 0.005f * MOUSE_ROTATE_FACTOR;
}

void limit(Image &d, int elementlimit)
{
  int sizex=d.dimx(), sizey=d.dimy();
  int nbpixel=sizex*sizey;

  if (nbpixel > elementlimit)
  { // a limit to the number of points displayed is needed to avoid crash

    float ratio = float(elementlimit) / float(nbpixel); // ratio is < 1

    sizex=round(float(sizex)*sqrtf(ratio));
    sizey=round(float(sizey)*sqrtf(ratio));

    d.resize(sizex,sizey);
  }
}


void PSApplication::limitImageSize()
{
  int sizex = sources_.leftImage_.dimx(), sizey = sources_.leftImage_.dimy();

  limit(sources_.leftImage_,  MAX_PIXEL_IN_IMAGE);
  limit(sources_.rightImage_, MAX_PIXEL_IN_IMAGE);
}



void PSApplication::runManualEdit()
{
  DEBUG(ruMan);
  NEVER_EXECUTE_ON_WINDOWS_THREAD();

  // it is no more possible to run edit mode without a map and a sculpt object
  if (!sculpt_.isModel())
    THROW_MESSAGE_TO_USER(USE_FUNCTION_1);

  requestRedraw();

  MorphingEditor editor(interface_.instanceHandle_, interface_.windowsHandle_, sculpt_);

  editor.launchDialog(true);

  VectorMap resultFromManualEdit = editor.getResultMap();
  
  if (!resultFromManualEdit.isEmpty())
  {
    sculpt_.setMap(resultFromManualEdit, false);
    runExtraction(); // true = the current level gets sculpted too
  }
}

void PSApplication::updateResult() 
{
  DEBUG(updare);
  if (result_.isResult())
  {
    result_.isInvertNormalX_ = GlobalOptions.isNormalInvertedX_;
    result_.isInvertNormalY_ = GlobalOptions.isNormalInvertedY_;

    result_.setAlpha(sources_.getAlpha());

    result_.updateAll(result_.getOriginalMap(),sources_.leftImage_,result_.getMode(),result_.getShader());
  }
}

POV GlobalPOV;

void PSApplication::createResultFromSculpt() 
{
  DEBUG(relo);
  if (sculpt_.isModel())
  {
    if (! sculptAdditional_.isModel())
    {
      // retreive the map from the current sculpt object
      PIntMap map = sculpt_.computeIntMap();
      //sculpt_.addLocalDetailToMap(map); // +400 mb memory peak
      //result_.setAlpha(sources_.getAlpha());

      result_.updateAll(*map, sources_.leftImage_, result_.getMode(), result_.getShader());
      //result_.bestPOV_ = sculpt_.getPOV();
      //GlobalPOV = result_.bestPOV_;
    }
    else
    {
      // retreive the map from the current sculpt object
      PIntMap map1 = sculpt_           .computeIntMap();
      PIntMap map2 = sculptAdditional_ .computeIntMap();

      // mean of both maps (map2 is negative as left and right are inverted in sculptAdditional_)
      *map1 = (*map1 - *map2) / 2 ;
      /*
      for (int i=0; i<map1->getSizeX(); i++)
        for (int j=map1->getSizeY()/2; j<map1->getSizeY(); j++)
          map1->set(i,j, -map2->get(i,j));*/
        
      map2 = new IntMap(); // free memory

      sculpt_.addLocalDetailToMap(map1);
      result_.setAlpha(sources_.getAlpha());

      result_.updateAll(*map1, sources_.leftImage_, result_.getMode(), result_.getShader());
      result_.bestPOV_ = sculpt_.getPOV(); // ?
    }
  }
  else
    result_.updateAll(IntMap(), Image(), result_.getMode(), result_.getShader());
}


void PSApplication::setPage(int newpage) 
{
  DEBUG(setP);
  ALWAYS_EXECUTE_ON_WINDOWS_LOOP_THREAD();

  if (GlobalOptions.isSoftwareMode_) newpage = ACTION_MODE_OLD_3D;
  DEBUG_VALUE(  "Page", newpage);

  bool isshowprogress =    result_.getModelSizeX()+result_.getModelSizeY() > 600  // 600 pixels
                        && ! (newpage == ACTION_MODE_OLD_3D && interface_.mouseButton_ == 1); // drag;

  ProgressDialog progress(isshowprogress);

  if (result_.getMode() == ACTION_MODE_3D)
    stop3DDisplay(); // frees 180 mb
  
  result_.isInvertNormalX_ = GlobalOptions.isNormalInvertedX_;
  result_.isInvertNormalY_ = GlobalOptions.isNormalInvertedY_;

  if (result_.getMode() != newpage || newpage == ACTION_MODE_OLD_3D)
    result_.updateAll(result_.getOriginalMap(),sources_.leftImage_,newpage,result_.getShader());

  if (newpage == ACTION_MODE_3D) 
    reinit3DDisplay(); // +430 mb peak 300mb

  requestRedraw();
}

Image & PSApplication::getLeftImage()
{
  return sources_.leftImage_;
}

Image & PSApplication::getRightImage()
{
  return sources_.rightImage_;
}

VectorMap PSApplication::getMap()
{
  if (sculpt_.isModel())
    return sculpt_.getMap();
  else
    return VectorMap();
}

void PSApplication::setEditCoordinates(const EditCoordinates& e)
{
  sculpt_.setEditCoordinates(e);
}

EditCoordinates PSApplication::getEditCoordinates() const
{
  return   sculpt_.getEditCoordinates();
}

Cropper PSApplication::getCropper()
{
  return result_.getCropper();
}

void PSApplication::setCropper(Cropper c)
{
  result_.setCropper( c );
}

void PSApplication::clearCurrentSculptInProgress()
{
  if ( sculpt_.isModel() )
    sculpt_.clearCurrentSculptInProgress();

  if ( sculptAdditional_.isModel() )
    sculptAdditional_.clearCurrentSculptInProgress();
}

#include "POVFinder.h"
#include "ComputeCameraPositionDialog.h"

void PSApplication::searchCameraPosition()
{
  NEVER_EXECUTE_ON_WINDOWS_THREAD();

  if (sculpt_.isModel())
  {
    try
    {
      UserInterrupt::clear();

      POVFinder finder;
      finder.init(sculpt_.computeLeftImage(), sculpt_.computeRightImage(), 
                  *sources_.getAlpha(), sculpt_.getMap(), result_.bestPOV_, 200);

      // set the POV
      result_.bestPOV_.setZScale     ( result_.zScale_   );

      bool isOk = computeCameraDialog(interface_.instanceHandle_, interface_.windowsHandle_, &finder);

      if (isOk)
      {
        sculpt_.setPOV(finder.getCurrentBestPOV());
        createResultFromSculpt();

        // set the result from POV data
        result_.zScale_          = result_.bestPOV_.getZScale() ;
        sources_.leftFocal_      = result_.bestPOV_.getFocalLength();
        postActionOnFirstThread(ACTION_MODE_3D);
      }
    }
    catch (UserInterrupt &)
    {
      // copies the result into the current map of coordinates
      //*currentMap_ = finder.generateMapOfCoordinates(pov_);
      postActionOnFirstThread(ACTION_MODE_3D);
    }
    INTERFACE_CATCH_BLOC(COMPUTING)
  }
  else
    MESSAGEHWND("This function needs 2 photos and a model to work. So please 1. load 2 photos, 2. launch a sculpt, 3. stop the sculpt, then 4. use this function");
}

void PSApplication::actionAnimateModel()
{
  if (!result_.isAnimation_)
  {
    startAnimation();

    result_.isAnimation_ = true;

    MSG msg;
    while(result_.isAnimation_)
    {
      // Check to see if any messages are waiting in the queue
      while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
      {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }

      // If the message is WM_QUIT, exit the while loop
      if(msg.message == WM_QUIT)
        break;

      animate();
    }

    MESSAGEHWND("Turntable display mode now turned off");
  }
  else
    stopAnimation();
}

void PSApplication::clearAll()
{
  if (askBeforeDelete())
  {
    sources_.leftImage_ = sources_.rightImage_ = sources_.additionalImage_ = Image();
    clearSculptAndResults();

    const Image empty;
    sources_.setAlpha(new Image(empty));

    reinit3DDisplay();
  }
}

#include "CropEditor.h"
void PSApplication::actionCropEdit()
{
  static CropEditor edit(interface_.instanceHandle_, interface_.windowsHandle_);

  if (!edit.isActive()) // editor window is not already opened?
  {
    edit.setCropperAndImage(result_.getCropperPointer(), sources_.leftImage_ );

    edit.launchDialog(false);

    //bringWindowToFront();
  }
}

#include "PinEditor.h"
void PSApplication::actionEditPinning()
{
  if (!isSculpting_)
    {
      if (sculpt_.isModel())
      {
        PinEditor edit(interface_.instanceHandle_, interface_.windowsHandle_, this);
        
        edit.launchDialog(true);

        if (edit.isOk())
        {
          sculpt_.resetMap();

          doActionOnNewThreadIfNecessary(ACTION_SCULPT_NOW);
        }
      }
      else THROW_MESSAGE_TO_USER(USE_FUNCTION_2);
    }
    else   THROW_MESSAGE_TO_USER(STOP_FIRST);
}

#include "PhotoSculptTools.h"

void PSApplication::actionAddDetail()
{
  if (result_.isResult())
  {
    ProgressDialog progress(STR(PROGRESS_ADDING_DETAIL));

    IntMap vmap = result_.getOriginalMap();
    Image tempFake = ImageGeneration::generateFakeDepth(
      sources_.leftImage_.get_resize(vmap.getSizeX(),vmap.getSizeY()));

    float strength   = 0.005f * MULTIPLY ;

    IntMap newmap = PhotoSculptTools::applyDisplace(tempFake, vmap, 
                        strength, 0.5f);

    result_.updateAll(newmap,sources_.leftImage_,result_.getMode(),result_.getShader());
    reinit3DDisplay();
  }
  else
    THROW_MESSAGE_TO_USER(3D_MODEL_FIRST);
}

void PSApplication::actionSubstractDetail()
{
  if (result_.isResult())
  {
    PROGRESS(SUBSTRACTING);

    IntMap vmap = result_.getOriginalMap();
    Image tempFake = ImageGeneration::generateFakeDepth(
      sources_.leftImage_.get_resize(vmap.getSizeX(),vmap.getSizeY()));

    float strength   = -0.005f * MULTIPLY ;

    IntMap newmap = PhotoSculptTools::applyDisplace(tempFake, result_.getOriginalMap(), 
                        strength, 0.5f);

    result_.updateAll(newmap,sources_.leftImage_,result_.getMode(),result_.getShader());
    reinit3DDisplay();
  }
  else
    MESSAGEHWND("Please load or create a 3D model before using that function");
} 

#include "FocalDialog.h"

void PSApplication::actionSetFocalDialog()
{
  askFocalDialog(interface_.instanceHandle_, interface_.windowsHandle_, &(sources_.leftFocal_) );

  updateResult();
  reinit3DDisplay();
}

#include "FilterDialog.h"

void PSApplication::actionSetBlurRadiusDialog()
{
  askBlurRadiusDialog(interface_.instanceHandle_, interface_.windowsHandle_, &(GlobalOptions.blur_) );
}

bool FlatModeOptionsDialog(HINSTANCE inst, HWND hwnd, int * numberofsamples);

void PSApplication::actionFlatOptionsDialog()
{
  int nbsamples = GlobalOptions.nbFlatModeSamples_;
  bool isok = FlatModeOptionsDialog(interface_.instanceHandle_, interface_.windowsHandle_, &(nbsamples) );
  
  if (isok)
  {
    GlobalOptions.nbFlatModeSamples_ = nbsamples;
    updateResult(); 
    reinit3DDisplay();
  }
}


#include "MultiTasking.h"
#include "ImageTools.h"
void PSApplication::applyOptions()
{
  interface_.directX_.setMaxTriangle ( GlobalOptions.nbTriangleDisplay_ );
  interface_.directX_.setLuminosity  ( GlobalOptions.luminosity_        );

  result_.isInvertNormalX_ = GlobalOptions.isNormalInvertedX_;
  result_.isInvertNormalY_ = GlobalOptions.isNormalInvertedY_;

  interface_.initButtonBarsIcons();
  repaintStoredButtons(GlobalOptions.hue_, GlobalOptions.saturation_);

  MultiTasking::setNbCores(GlobalOptions.nbThreadMax_);

  updateResult(); 

  reinit3DDisplay();
  requestRedraw();
}


void PSApplication::repaintStoredButtons( int hue, float saturation )
{
  interface_.bottomBar_             .repaintStoredButtons(hue, saturation);
  interface_.bottomMiniBar_         .repaintStoredButtons(hue, saturation);
  interface_.frontPage_             .repaintStoredButtons(hue, saturation);
  interface_.contentZone_           .repaintStoredButtons(hue, saturation);
  interface_.popupSave_             .repaintStoredButtons(hue, saturation);
  interface_.popup3d_               .repaintStoredButtons(hue, saturation);
  interface_.popupLoadL_            .repaintStoredButtons(hue, saturation);
  interface_.popupLoadR_            .repaintStoredButtons(hue, saturation);
  interface_.popupLoadAlpha_        .repaintStoredButtons(hue, saturation);
  interface_.topTabBar_             .repaintStoredButtons(hue, saturation);
  interface_.bottomSolo_            .repaintStoredButtons(hue, saturation);
  interface_.bottomZone_            .repaintStoredButtons(hue, saturation);
  interface_.manualInterfacebuttons_.repaintStoredButtons(hue, saturation);

  interface_.rightPhotoicon_ = ImageTools::mixTint(interface_.rightPhotoicon_, hue);
  interface_.leftPhotoicon_  = ImageTools::mixTint(interface_.leftPhotoicon_,  hue);

  interface_.rightPhotoicon_ = ImageTools::mixDesaturateRedChannelOnly(interface_.rightPhotoicon_, saturation);
  interface_.leftPhotoicon_  = ImageTools::mixDesaturateRedChannelOnly(interface_.leftPhotoicon_,  saturation);
}

#include "SelectionEditor.h"
void PSApplication::actionEditSelection()
{
  SelectionEditor editor(interface_.instanceHandle_, interface_.windowsHandle_,
                         sources_.getAlpha(), new Image(sources_.leftImage_) );
  
  editor.launchDialog(true);

  //bringWindowToFront();

  if (editor.isOk()) // updates the results with the latest info from the crop editor.
  {
    ProgressDialog progress;

    sources_.setAlpha( editor.getAlpha() );

    updateResult();
    reinit3DDisplay();
  }
}


void PSApplication::actionMove3D(int action)
{
  DEBUG(actionMo);
  NANOBEGIN;
  switch (action)
  {
    case ACTION_3D_ROTATE_LEFT :

      result_.azimuth_ += STEP_SPIN_3D_ANGLE;
      limit(result_.azimuth_, -90.0f, 90.0f);
      break;

    case ACTION_3D_ROTATE_RIGHT :

      result_.azimuth_ -= STEP_SPIN_3D_ANGLE; 
      limit(result_.azimuth_, -90.0f, 90.0f);
      break;

    case ACTION_3D_ROTATE_UP :

      result_.zenith_ -= STEP_SPIN_3D_ANGLE;
      limit(result_.zenith_, -90.0f, 90.0f);
      break;

    case ACTION_3D_ROTATE_DOWN :

      result_.zenith_ += STEP_SPIN_3D_ANGLE; 
      limit(result_.zenith_, -90.0f, 90.0f);
      break;

    case ACTION_3D_ZOOM_PLUS :

      result_.modelScale_ *= STEP_3D_SCALE ; 
      limit(result_.modelScale_, 0.1f , 10.0f);
      break;

    case ACTION_3D_ZOOM_MINUS :

      result_.modelScale_ /= STEP_3D_SCALE ; 
      limit(result_.modelScale_, 0.1f , 10.0f);
      break;
  }

  if (result_.getMode() == ACTION_MODE_OLD_3D)
    postActionOnFirstThread(ACTION_MODE_OLD_3D); // launches a redraw after key pressed

  NANOEND;
}

void PSApplication::rotateImages(float leftimageangle, float centerimageangle, float rightimageangle, float alphaangle)
{
   if (askBeforeDelete())
     sources_.rotateImages(leftimageangle, centerimageangle, rightimageangle, alphaangle);
}

void PSApplication::multiplyDepth(float value)
{
  result_.zScale_ *= value ;
  limit(result_.zScale_,0.2f,40.0f);

  updateResult();
  reinit3DDisplay();
}

void PSApplication::actionDownSample()
{
  if (sculpt_.isModel())
  {
    bool isSuccess = sculpt_.downsampleToPreviousLevel();
    if (!isSuccess)
      THROW_MESSAGE_TO_USER(SIZE_LIMIT);

    createResultFromSculpt();

    reinit3DDisplay();
  }
  else
  {
    IntMap newmap = result_.getMap();
    int newsizex = newmap.getSizeX() / 2 ;
    int newsizey = newmap.getSizeY() / 2 ;
    if (newsizex<2) newsizex = 2 ;
    if (newsizey<2) newsizey = 2 ;
    newmap = newmap.getResizeAndMultiply(newsizex, newsizey);

    result_.updateAll(newmap, sources_.leftImage_, result_.getMode(), result_.getShader());
    reinit3DDisplay();
  }
}

void PSApplication::actionShellExecute(char* text)
{
  ShellExecute(interface_.windowsHandle_, "open", text, NULL, NULL, SW_SHOWNORMAL);
}

void PSApplication::zeroDisplacementMap()
{
  if (sculpt_.isModel())
  {
    VectorMap map = sculpt_.getMap();
    map.fill( Vector(0,0) );
    sculpt_.setMap(map, false);

    createResultFromSculpt();
    reinit3DDisplay();
  }
}

bool PSApplication::copyImageToClipboard()
{ 
  bool isok = Win32Tools::copyImageToClipboard(result_.getImage(), interface_.windowsHandle_);
  if (!isok)
    THROW_MESSAGE_TO_USER(COPY_CLIPBOARD);

  return isok;
}

bool PSApplication::pasteImageLFromClipboard()
{ 
  Image image = Win32Tools::loadImageFromClipboard(interface_.windowsHandle_);

  if (image.is_empty()) return false;
  
  sources_.leftImage_ = image;
  return true;
}

bool PSApplication::pasteImageRFromClipboard()
{ 
  Image image = Win32Tools::loadImageFromClipboard(interface_.windowsHandle_);

  if (image.is_empty()) return false;
  
  sources_.rightImage_ = image;
  return true;
}

UserInfo PSApplication::getUserInfo ()
{
  UserInfo info;

  info.isResult        = result_.isResult();
  info.totalused       = Win32Tools::getMemUsedMb();
  info.isSculptModel   = sculpt_.isModel();
  info.isSculpting     = isSculpting_;
  info.isAutomaticMode = commandLine_.isOutput_; 
  info.isProtected     = checkArmadilloProtection();

  if (info.isSculptModel)
    info.isLastStep = sculpt_.isLastStep();
    
  if ( info.isResult )
  {
    info.resultSizeX       = result_.getModelSizeX();
    info.resultSizeY       = result_.getModelSizeY();
    info.resultSubdivition = result_.subdivision_ ;
    info.nbTriangleDisplayed = interface_.directX_.getTriangleDisplayed()/1000 ;
  }

  if (info.isSculptModel)
  {
    info.step              = sculpt_.computeStep();
    info.substep           = sculpt_.getSubStep();
    info.maxlevel          = sculpt_.getMaxLevel();

    info.timeToEnd         = sculpt_.estimateTimeToEnd();
  }

  return info;
}

void PSApplication::openLoadDialogWithDoubleClick(int x, int y)
{
  if (isSculpting_) return;
  
  switch (result_.getMode())
  {
  case ACTION_MODE_SOURCES:
    if (interface_.frontPage_.isInside(x, y, ACTION_IMAGE_SOURCE_LEFT))
      loadDialog(true);
    else if (interface_.frontPage_.isInside(x, y, ACTION_IMAGE_SOURCE_RIGHT))
      loadDialog(false);
    else if (interface_.frontPage_.isInside(x, y, ACTION_IMAGE_SOURCE_ALPHA))
      actionEditSelection();
    break;
  case ACTION_MODE_DISPLACE: case ACTION_MODE_NORMAL:   case ACTION_MODE_AMBOCC:
  case ACTION_MODE_SPECULAR: case ACTION_MODE_DIFFUSE:  case ACTION_MODE_ALPHA:
  case ACTION_MODE_OLD_3D:   case ACTION_MODE_BUMP:     case ACTION_MODE_LIMDISP:
  case ACTION_MODE_TILE_MASK:
    if (interface_.frontPage_.isInside(x, y, ACTION_IMAGE_CENTER))
      saveAsImageDialog();
    break;
  };
}