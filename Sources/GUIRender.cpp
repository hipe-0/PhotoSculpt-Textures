#include "GraphicUserInterface.h"
#include "File.h"
#include "Frame.h"

extern int GlobalDisplayCounter;
const unsigned char MAXIMUM_OPACITY = 255;

int GlobalDisplayCounter=0;

void frameValuesByRectangle(int dimx, int dimy, int posx, int posy, int windowsizex, int windowsizey, 
                            int& retx, int& rety, int& retsizex, int& retsizey)
{
	int resizedx = dimx;
	int resizedy = dimy;
	Frame::limit(resizedx, resizedy, windowsizex, windowsizey);

	int x= posx + (windowsizex - resizedx) / 2;
	int y= posy + (windowsizey - resizedy) / 2;

  retx = x; rety = y; retsizex = resizedx ; retsizey = resizedy;
}

void GraphicUserInterface::renderBottomBar(const UserInfo& info)
{
  ALWAYS_EXECUTE_ON_WINDOWS_LOOP_THREAD();

  if (image_.is_empty())
    THROW_INTERNAL_ERROR( "interface image is not set (in renderBottomBar())" );

  // background color
  const unsigned char color[3] = {GlobalOptions.luminosity_,GlobalOptions.luminosity_,GlobalOptions.luminosity_} ;
  image_.draw_rectangle(0, getClientSizeY() - getBottomBarSizeY()-1, 
                                 getClientSizeX()-1, getClientSizeY()-1, color);
  
  frame_.draw(image_,  bottomBar_.getPositionX(), bottomBar_.getPositionY(), 
                                bottomBar_.getPositionX()+bottomBar_.getSizeX()-1, 
                                getBottomBarPosY()+getBottomBarSizeY()-1);

  // background alpha for opacity
  unsigned char opacity = GlobalOptions.interfaceOpacity_;
  if (!isAeroGlass_) opacity = 255;

  alpha_.draw_rectangle ( 0, getBottomBarPosY(),
                                      getClientSizeX()-1,
                                      getClientSizeY()-1, &opacity);

  const unsigned char bottombaropacity = 250 ;
  if (!isSculptBarMinimised_)
    bottomBar_.drawSolidRectangleOnImage(alpha_, &bottombaropacity);
  else
    bottomMiniBar_.drawSolidRectangleOnImage(alpha_, &bottombaropacity);

  if (isSculptBarMinimised_)
  {
    bottomMiniBar_.show(); bottomSolo_.hide();
    bottomMiniBar_.drawBackground();
    bottomMiniBar_.draw();
  }
  else if (!info.isSculpting && !info.isSculptModel && !info.isResult)
  {
    // show solo and hide full bottom bar so it doesn't detect user action later on
    bottomSolo_.show(); bottomBar_.hide();
    bottomSolo_.drawBackground();
    bottomSolo_.draw();
  }
  else 
  {
    bottomBar_.show(); bottomSolo_.hide();
    bottomBar_.drawBackground();
    bottomBar_.draw();
  }

  // draws the version number 
  drawText(VERSION_PHOTOSCULPT_SOFTWARE,
                               bottomBar_.getPositionX()+bottomBar_.getSizeX()+10,
                               bottomBar_.getPositionY()+57);
  
  int alignright  = bottomBar_.getPositionX() + bottomBar_.getSizeX() + 10 ;
  int alignbottom = getBottomBarPosY() ;

  if (!isSculptBarMinimised_)
  {
    // draws the memory usage
    char text[200];

    sprintf(text, STR(USING_MB), info.totalused);
    drawText(text, alignright, alignbottom+47);
    DEBUG_VALUE("    Using mb", info.totalused);

    if (info.isAutomaticMode)
    {
      sprintf(text, STR(AUTO));
      drawText(text, alignright, alignbottom+37);
    }

    if (GlobalDisplayCounter>0)
    {
      sprintf(text, "Debug: displayed %d times", GlobalDisplayCounter);
      drawText(text, alignright, alignbottom+27);
    }

    if (!info.isProtected)
    {
      sprintf(text, "Unprotected");
      drawText(text, alignright, alignbottom+37);
    }
  }

  // draws the progress bar if needed, on top of the result box
  if (info.isSculptModel)
    if (!info.isLastStep || info.isSculpting)
    {
      //sculpt_->drawProgressBar();
      progressionBar_.setStep ( info.maxlevel, info.step,
                                           NB_SUB_STEP , info.substep );
      progressionBar_.draw();
    }

  if ( info.isResult )
  {
    char txt[50]={'\0'};
  
    // Display model size
    sprintf(txt, "%d x %d",   info.resultSizeX, info.resultSizeY);
    DEBUG_VALUE("    Result_ Model Size", txt);

    int coordtextsizex = PROGRESSION_BAR_TEXT_X + bottomBar_.getPositionX();
    if (isSculptBarMinimised_) 
      coordtextsizex -= 100;

    if (info.resultSubdivition > 1)
    {
      char txt2[10]={'\0'};
      sprintf(txt2, " (/ %d)",   info.resultSubdivition);
      strcat(txt, txt2);
      
      drawTextOnOpaqueWhite(txt, coordtextsizex - 25,
        PROGRESSION_BAR_TEXT_Y + getBottomBarPosY());
    }
    else
      drawTextOnOpaqueWhite(txt, coordtextsizex,
        PROGRESSION_BAR_TEXT_Y + getBottomBarPosY());
  
    if (info.isSculptModel)
    {
      if (!isSculptBarMinimised_)
      {
        // Display step number
        sprintf(txt, STR(STEP), info.step, info.substep);
        DEBUG_VALUE("    step",txt);
     
        drawText(txt, alignright,
          alignbottom+PROGRESSION_BAR_TEXT_Y+10);
      }

      // Display time estimate

      if (info.timeToEnd == TASKTIMER_CODE_NO_DATA)
        sprintf(txt, "-");
      else if (info.timeToEnd == TASKTIMER_CODE_CURRENTLY_ESTIMATING)
        sprintf(txt, STR(ESTIMATING));
      else if (info.timeToEnd == 0.0f)
        sprintf(txt, STR(1SECOND));
      else
      {
        float MINUTE = 60.0f ;
        if (info.timeToEnd > MINUTE)
        {
          float minutes = floor (info.timeToEnd / MINUTE) ;
          float seconds = info.timeToEnd - minutes * MINUTE ;
          sprintf(txt, STR(MINUTES), minutes, seconds);
        }
        else
          sprintf(txt, STR(SECONDS), info.timeToEnd);
      }
      
      if (!isSculptBarMinimised_)
        drawTextOnOpaqueWhite(txt,
          320 + bottomBar_.getPositionX(),
          41  + bottomBar_.getPositionY());
    }

    if (!isSculptBarMinimised_)
    {
      // display number of triangle displayed
      if (info.nbTriangleDisplayed>0)
      {
        sprintf(txt, STR(TRIAS), info.nbTriangleDisplayed);
        drawText(txt,
          alignright , alignbottom + PROGRESSION_BAR_TEXT_Y );
      }
    }
  }

  // draws the sleep icon if needed
  sleepIcon_.drawSleepIcon(bottomBar_.getPositionX(),
                                      bottomBar_.getPositionY(), image_);
}

void GraphicUserInterface::renderContent2D(int mode, const Image& resultimage)
{
  ALWAYS_EXECUTE_ON_WINDOWS_LOOP_THREAD();

  if (mode == ACTION_MODE_DISPLACE   ||
      mode == ACTION_MODE_NORMAL     ||
      mode == ACTION_MODE_AMBOCC     ||
      mode == ACTION_MODE_SPECULAR   ||
      mode == ACTION_MODE_DIFFUSE    ||
      mode == ACTION_MODE_ALPHA      ||
      mode == ACTION_MODE_OLD_3D     ||
      mode == ACTION_MODE_BUMP       ||
      mode == ACTION_MODE_LIMDISP    ||
      mode == ACTION_MODE_TILE_MASK  )
  {
    const unsigned char color[3] = {GlobalOptions.luminosity_,GlobalOptions.luminosity_,GlobalOptions.luminosity_} ;
    image_.draw_rectangle(0, topTabBar_.getSizeY(), 
      getClientSizeX()-1, getBottomBarPosY() -1, color);

    unsigned char interfaceopacity = GlobalOptions.interfaceOpacity_;
    if (!isAeroGlass_) interfaceopacity = 255;

    alpha_.draw_rectangle(0, topTabBar_.getSizeY(), 
      getClientSizeX()-1, getBottomBarPosY() -1, &interfaceopacity);
 
    if (!resultimage.is_empty())
    {
      int resizex=getClientSizeX()-IMAGE_BLANK_FRAME;
      if (resizex<=0) 
        return;

      int resizey=resizex * resultimage.dimy() / resultimage.dimx() ;

      if (resizey>getContentSizeY()-IMAGE_BLANK_FRAME)
      {
        resizey=getContentSizeY()-IMAGE_BLANK_FRAME;
        if (resizey<=0) 
          return;
        resizex=resizey * resultimage.dimx()/ resultimage.dimy();
        if (resizex<=0) 
          return;
      }

      int posx = (getClientSizeX() -resizex)/2 ;
      int posy = (getContentSizeY()-resizey)/2 + topTabBar_.getSizeY();

      // load the image from the result object then resize it
      Image resize = resultimage.get_resize(resizex,resizey);

      // draw the 2D image to the interface
      image_.draw_image(resize,posx,posy);
      alpha_.draw_rectangle(posx,posy, posx+resizex-1, posy+resizey-1, &MAXIMUM_OPACITY);

      // draws a frame around it
      frame_.draw(image_, posx,posy, posx+resizex-1,posy+resizey-1);

      frontPage_.changeButton(ACTION_IMAGE_CENTER, 
                                     posx, posy, posx+resizex-1, posy+resizey-1);
      popupSave_.setPos(posx, posy);
       if (!popupSave_.isHidden())
      {
        popupSave_.drawBackground();
        popupSave_.draw();
        popupSave_.drawSolidRectangleOnImage(alpha_, &MAXIMUM_OPACITY);
        frame_.draw(image_,  popupSave_.getPositionX(), popupSave_.getPositionY(), 
                                      popupSave_.getPositionX()+popupSave_.getSizeX()-1, 
                                      popupSave_.getPositionY()+popupSave_.getSizeY()-1);
      }

      popup3d_.setPos(posx+resizex-popup3d_.getSizeX(),
                              posy+resizey-popup3d_.getSizeY());
      if (!popup3d_.isHidden())
      {
        popup3d_.drawBackground();
        popup3d_.draw();
        popup3d_.drawSolidRectangleOnImage(alpha_, &MAXIMUM_OPACITY);
        frame_.draw(image_,  popup3d_.getPositionX(), popup3d_.getPositionY(), 
                                      popup3d_.getPositionX()+popup3d_.getSizeX()-1, 
                                      popup3d_.getPositionY()+popup3d_.getSizeY()-1);
      }
    }
  }
}

void GraphicUserInterface::renderSourceImages(Source& sources) // note: sources should be const but pointToFileNameOnly cannot allow this
{
  ALWAYS_EXECUTE_ON_WINDOWS_LOOP_THREAD();

  const unsigned char color[3] = {GlobalOptions.luminosity_,GlobalOptions.luminosity_,GlobalOptions.luminosity_} ;
  image_.draw_rectangle(0, topTabBar_.getSizeY(), 
    getClientSizeX()-1, getBottomBarPosY()-1, color);

  unsigned char interfaceopacity = GlobalOptions.interfaceOpacity_;
  if (!isAeroGlass_) interfaceopacity = 255;

  alpha_.draw_rectangle(0, topTabBar_.getSizeY(), 
    getClientSizeX()-1, getBottomBarPosY()-1, &interfaceopacity);
 
  bool is3images = !sources.additionalImage_.is_empty();

  const int FRAME = 10, CENTER_COLUMN = 14;
  int windowsizex = (getClientSizeX() - 2*FRAME - CENTER_COLUMN) / 2;
  int windowsizey = getContentSizeY() - 2*FRAME ;

  if (is3images) 
    windowsizex = (getClientSizeX() - 2*FRAME - 2* CENTER_COLUMN) / 3;

  if (sources.isAlpha()) windowsizey = (windowsizey - CENTER_COLUMN) * 2 / 3 ;

  Image imageL, imageR, imageA;
  if (!sources.leftImage_.is_empty())  imageL = sources.leftImage_        ;
  else                                 imageL = leftPhotoicon_  ; 
  if (!sources.rightImage_.is_empty()) imageR = sources.rightImage_       ;
  else                                 imageR = rightPhotoicon_ ; 
  imageA = sources.additionalImage_ ;

  // positions and sizes of left and right images
  int posxleftarea  = FRAME ;
  int posyleftarea  = FRAME + topTabBar_.getSizeY();
  int posxrightarea = FRAME + windowsizex + CENTER_COLUMN ;
  int posyrightarea = FRAME + topTabBar_.getSizeY();

  int posxaddarea = FRAME ;
  int posyaddarea = FRAME + topTabBar_.getSizeY();
  if (is3images)
  {
    posxleftarea  += CENTER_COLUMN + windowsizex ;
    posxrightarea += CENTER_COLUMN + windowsizex ;
  }

  int posxl, posyl, sizexl, sizeyl ;
  int posxr, posyr, sizexr, sizeyr ;
  int posxa, posya, sizexa, sizeya ;
  frameValuesByRectangle(imageL.dimx(), imageL.dimy(), posxleftarea, posyleftarea, windowsizex, windowsizey,
                         posxl,  posyl,  sizexl,  sizeyl);
  frameValuesByRectangle(imageR.dimx(), imageR.dimy(), posxrightarea, posyrightarea, windowsizex, windowsizey,
                         posxr,  posyr,  sizexr,  sizeyr);
  if (is3images)
    frameValuesByRectangle(imageA.dimx(), imageA.dimy(), posxaddarea, posyaddarea, windowsizex, windowsizey,
                           posxa,  posya,  sizexa,  sizeya);
  
  // drawing the source images
  if (sizexl>0 && sizeyl>0)
  {
    image_.draw_image    (imageL.get_resize(sizexl, sizeyl), posxl, posyl);
    alpha_.draw_rectangle(posxl, posyl, posxl+sizexl-1, posyl+sizeyl-1, &MAXIMUM_OPACITY);
    frame_.drawAtPosAndSize(image_, posxl, posyl, sizexl, sizeyl);
  }

  if (sizexr>0 && sizeyr>0)
  {  
    image_.draw_image    (imageR.get_resize(sizexr, sizeyr), posxr, posyr);
    alpha_.draw_rectangle(posxr, posyr, posxr+sizexr-1, posyr+sizeyr-1, &MAXIMUM_OPACITY);
    frame_.drawAtPosAndSize(image_, posxr, posyr, sizexr, sizeyr);
  }

  if (is3images && sizexa>0 && sizeya>0)
  {  
    image_.draw_image(imageA.get_resize(sizexa, sizeya), posxa, posya);
    alpha_.draw_rectangle(posxa, posya, posxa+sizexa-1, posya+sizeya-1, &MAXIMUM_OPACITY);
    frame_.drawAtPosAndSize(image_, posxa, posya, sizexa, sizeya);
  }

  // draw file names for sources images
	if (!sources.leftImage_.is_empty())
		drawText(pointToFileNameOnly(sources.leftFileName_), posxl, posyl-10);
	  
	if (!sources.rightImage_.is_empty())
	  drawText(pointToFileNameOnly(sources.rightFileName_), posxr, posyr-10);

  // updating the invisible but mouse sensitive area "frontpage"
  frontPage_.changeButton(ACTION_IMAGE_SOURCE_LEFT, posxl, posyl, posxl+sizexl-1,posyl+sizeyl-1);
  frontPage_.changeButton(ACTION_IMAGE_SOURCE_RIGHT, posxr, posyr, posxr+sizexr-1,posyr+sizeyr-1);

  // updates the popup menu positions
  popupLoadL_.setPos(posxl, posyl);
  if (!popupLoadL_.isHidden())
  {
    popupLoadL_.drawBackground();
    popupLoadL_.draw();
    popupLoadL_.drawSolidRectangleOnImage(alpha_, &MAXIMUM_OPACITY);
    frame_.drawAtPosAndSize(image_, popupLoadL_.getPositionX(), popupLoadL_.getPositionY(), 
                                               popupLoadL_.getSizeX(),     popupLoadL_.getSizeY());
  }

  popupLoadR_.setPos(posxr, posyr);
  if (!popupLoadR_.isHidden())
  {
    frame_.drawAtPosAndSize(image_, popupLoadR_.getPositionX(), popupLoadR_.getPositionY(), 
                                               popupLoadR_.getSizeX(),     popupLoadR_.getSizeY());
    popupLoadR_.drawBackground();
    popupLoadR_.draw();
    popupLoadR_.drawSolidRectangleOnImage(alpha_, &MAXIMUM_OPACITY);
  }

  // displays the focal length in mm on the left image
  if (!sources.leftImage_.is_empty())
  {
    char bufferfocal[20];
    if (sources.leftFocal_ > 0.0f ) sprintf(bufferfocal, STR(FOCAL), sources.leftFocal_ );
    else                            sprintf(bufferfocal, STR(FOCAL2), sources.leftFocal_ );

    drawText(bufferfocal, posxl + sizexl - 80, posyl + sizeyl);
  }

  // draws the Alpha image
  if (sources.isAlpha())
  {
    int posxalphaarea = FRAME ;

    if (is3images)
      posxalphaarea += CENTER_COLUMN + windowsizex;

    int posyalphaarea = FRAME + topTabBar_.getSizeY() + CENTER_COLUMN + windowsizey;
    int posxa, posya, sizexa, sizeya ;
    frameValuesByRectangle(sources.getAlpha()->dimx(), sources.getAlpha()->dimy(), 
                           posxalphaarea, posyalphaarea, windowsizex, windowsizey/2,
                           posxa,  posya,  sizexa,  sizeya);

    if (sizexa>0 && sizeya>0)
    {
      image_.draw_image(sources.getAlpha()->get_resize(sizexa, sizeya), posxa, posya);
      alpha_.draw_rectangle(posxa, posya, posxa+sizexa-1, posya+sizeya-1, &MAXIMUM_OPACITY);
      frame_.drawAtPosAndSize(image_, posxa, posya, sizexa, sizeya);

	    drawText(STR(ALPHA), posxa, posya-10);

      frontPage_.changeButton(ACTION_IMAGE_SOURCE_ALPHA, posxalphaarea, posyalphaarea, 
                                         posxalphaarea+windowsizex-1,posyalphaarea+windowsizey/2-1);
      
      popupLoadAlpha_.setPos(posxa, posya);
 
      if (!popupLoadAlpha_.isHidden())
      {
        popupLoadAlpha_.drawBackground();
        popupLoadAlpha_.draw();
        popupLoadAlpha_.drawSolidRectangleOnImage(alpha_, &MAXIMUM_OPACITY);
        frame_.drawAtPosAndSize(image_, popupLoadAlpha_.getPositionX(), popupLoadAlpha_.getPositionY(), 
                                                 popupLoadAlpha_.getSizeX(),     popupLoadAlpha_.getSizeY());
      }
    }
  }
}

void GraphicUserInterface::displayToWindow(const UserInfo& info, Source& sources, int mode, Result & result) // triggered by WM_PAINT message at any time!
{
  //DEBUG(GUIdisplayToWindow);
  alpha_.fill(255);

  topTabBar_.drawBackground();
  topTabBar_.draw();
  topTabBar_.display();

  renderBottomBar(info);

  Image alphabottomzone = alpha_.get_crop(0, getBottomBarPosY(),
                                 getClientSizeX()-1, getClientSizeY()-1);

  bottomZone_.setAlpha(alphabottomzone);
  alpha_.draw_image(alphabottomzone,0,bottomBar_.getPositionY());
  bottomZone_.display();

  switch (mode)
  {
  case ACTION_MODE_SOURCES:
    {
      renderSourceImages(sources);
      contentZone_.setAlpha(alpha_.get_crop(0,topTabBar_.getSizeY(), 
                                   getClientSizeX()-1, getBottomBarPosY()-1));
      contentZone_.display();
      break;
    }

  case ACTION_MODE_DISPLACE: case ACTION_MODE_NORMAL:   case ACTION_MODE_AMBOCC:
  case ACTION_MODE_SPECULAR: case ACTION_MODE_DIFFUSE:  case ACTION_MODE_ALPHA:
  case ACTION_MODE_OLD_3D:   case ACTION_MODE_BUMP:     case ACTION_MODE_LIMDISP:
  case ACTION_MODE_TILE_MASK:
    {
      renderContent2D(mode, result.getImage());
      contentZone_.setAlpha(alpha_.get_crop(0,topTabBar_.getSizeY(), 
                                   getClientSizeX()-1, getBottomBarPosY()-1));
      contentZone_.display();
      break;
    }
   
  case ACTION_MODE_3D:
    // render 3D model with DirectX
    {
      const float PI = 3.141592f;

      directX_.setCamera( -result.azimuth_ * (PI / 180.0f) + PI,
                           result.zenith_  * (PI / 180.0f) + PI,
                           result.roll_    * (PI / 180.0f) + PI,
                           result.xTrans_, result.yTrans_, 0,
                           result.modelScale_ );

      if (directX_.isOn())
        directX_.render3DImageFrame();
      else
        contentZone_.display();

      // Draws the popup3D
      if (!popup3d_.isHidden())
      {
        popup3d_.setPos(getClientSizeX() - popup3d_.getSizeX(), 
                getBottomBarPosY() - popup3d_.getSizeY() );
        popup3d_.drawBackground();
        popup3d_.draw();
        popup3d_.updateWindow(getClientSizeX() - popup3d_.getSizeX(), 
                              getBottomBarPosY() - popup3d_.getSizeY() - topTabBar_.getSizeY(),
                              popup3d_.getSizeX(), popup3d_.getSizeY());
        ShowWindow(popup3d_.getWindowNr(), SW_SHOW);
        popup3d_.display();
      }
      else
        ShowWindow(popup3d_.getWindowNr(), SW_HIDE);

      // Draws the popupSave
      if (!popupSave_.isHidden())
      {
        popupSave_.setPos(0, topTabBar_.getSizeY());
        popupSave_.drawBackground();
        popupSave_.draw();
        popupSave_.updateWindow(0, 0,
                                popupSave_.getSizeX(), popupSave_.getSizeY());
        ShowWindow(popupSave_.getWindowNr(), SW_SHOW);
        popupSave_.display();
      }
      else
        ShowWindow(popupSave_.getWindowNr(), SW_HIDE);

      break;
    }
  }

  /*
  HDC hdc = GetDC(windowsHandle_);

  bool isok= Win32Tools::copyImageToDC(image_, hdc, alpha_);

  ReleaseDC(windowsHandle_, hdc);
  
  if (!isok)
        THROW_INTERNAL_ERROR("Could not display interface (in displayCenterView())");
  */
}

