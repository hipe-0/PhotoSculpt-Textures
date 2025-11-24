/*
 * ButtonManager.cpp
 *
 *  Created on: 22 april 2009
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */

#include "ButtonManager.h"
#include "Includes.h"
#include "Win32Tools.h"
#include "ImageTools.h"


// use setOutput to define the target image before any display is required by the class
ButtonManager::ButtonManager():background_(Image()),
  outputScreen_(0),nbButton_(0),
  pressed_(-1),highlighted_(-1),posX_(0), posY_(0), alphaValue_(255), sizex_(0), sizey_(0)
{
  lastButtonState_ = 0    ;
  lasthoover_      = -1   ;
  isHide_          = false;
  hWnd_            = 0    ;
}

// Checks if cursor enters any of the button.
// Stops searching at first positive result
// Returns button code if found
// Returns -1 if not found
int ButtonManager::detectCursorHoover(int mousex,int mousey)
{
  if (!isHide_)
    for (int i=0;i<nbButton_;i++)
    {
      bool isWithinBoundary = mousex>=buttons[i].x1 && mousey>=buttons[i].y1 &&
          mousex<=buttons[i].x2 && mousey<=buttons[i].y2;

      if (isWithinBoundary)
        // is this button active for clicking
        if (!buttons[i].isInactive_ && buttons[i].isClickable_)
          return buttons[i].actionCode_;
    }
  
  return -1;
}

bool ButtonManager::isInside(int mousex,int mousey, int action)
{
  if (!isHide_)
    for (int i=0;i<nbButton_;i++)
      if (buttons[i].actionCode_==action) 
      {
        bool isWithinBoundary = mousex>=buttons[i].x1 && mousey>=buttons[i].y1 &&
          mousex<=buttons[i].x2 && mousey<=buttons[i].y2;

        if (isWithinBoundary)
          // is this button active for clicking
          if (!buttons[i].isInactive_ && buttons[i].isClickable_)
            return true;
      }
  
  return false;
}

void ButtonManager::storeBackground(Image & backg)
{
  background_=backg;
  sizex_ = backg.dimx();
  sizey_ = backg.dimy();
}

void ButtonManager::drawBackground()
{
  if (!isHide_)
    if (outputScreen_ != NULL)
      outputScreen_->draw_image(background_, posX_, posY_);
}

void ButtonManager::setOutput(Image & out)
{
  outputScreen_ = &out;
}

void ButtonManager::declare_grayed_out(int action)
{
  for (int i=0;i<nbButton_;i++)
    if (buttons[i].actionCode_==action) 
      buttons[i].isInactive_=true;
}

void ButtonManager::declare_unsensible(int action)
{
  for (int i=0;i<nbButton_;i++)
    if (buttons[i].actionCode_==action) 
      buttons[i].isClickable_=false;
}

void ButtonManager::declare_sensible(int action)
{
  for (int i=0;i<nbButton_;i++)
    if (buttons[i].actionCode_==action) 
      buttons[i].isClickable_=true;
}

void ButtonManager::declare_active(int action)
{
  for (int i=0;i<nbButton_;i++)
    if (buttons[i].actionCode_==action) 
      buttons[i].isInactive_=false;
}

void ButtonManager::declareStatusOn(int action)
{
  for (int i=0;i<nbButton_;i++)
    if (buttons[i].actionCode_==action)
      buttons[i].isStatusOn=true;
}

void ButtonManager::declareStatusOff(int action)
{
  for (int i=0;i<nbButton_;i++)
    if (buttons[i].actionCode_==action)
      buttons[i].isStatusOn=false;
}

void ButtonManager::setPressed_(int action)
{
  pressed_ = action;
}

void ButtonManager::cancelPressed_()
{
  pressed_ = -1;
}

void ButtonManager::setHighlight_(int action)
{
  highlighted_ = action;
}

void ButtonManager::cancelHighlight_()
{
  highlighted_ = -1;
}

void ButtonManager::declare_all_grayed_out()
{
  for (int i=0; i<nbButton_;i++)
    declare_grayed_out(buttons[i].actionCode_);
}

void ButtonManager::declare_all_active()
{
  for (int i=0; i<nbButton_;i++)
    declare_active(buttons[i].actionCode_);
}

void ButtonManager::declareContinuous(int action)
{
  for (int i=0;i<nbButton_;i++)
    if (buttons[i].actionCode_==action)
      buttons[i].iscontinuous = true;
}
bool ButtonManager::isContinuous(int action)
{
  for (int i=0; i<nbButton_; i++)
    if (buttons[i].actionCode_==action)
      return buttons[i].iscontinuous;

  return false;
}

void ButtonManager::drawRectangle_(int i,unsigned char r,unsigned char g,unsigned char b)
{
  unsigned char linecolor[] = { r,g,b };
  if (outputScreen_ != NULL)
  {
    outputScreen_->draw_line(buttons[i].x1+posX_,buttons[i].y1+posY_,
                             buttons[i].x1+posX_,buttons[i].y2+posY_,linecolor);
    outputScreen_->draw_line(buttons[i].x1+posX_,buttons[i].y1+posY_,
                             buttons[i].x2+posX_,buttons[i].y1+posY_,linecolor);
    outputScreen_->draw_line(buttons[i].x1+posX_,buttons[i].y2+posY_,
                             buttons[i].x2+posX_,buttons[i].y2+posY_,linecolor);
    outputScreen_->draw_line(buttons[i].x2+posX_,buttons[i].y1+posY_,
                             buttons[i].x2+posX_,buttons[i].y2+posY_,linecolor);

    outputScreen_->draw_text(buttons[i].caption           ,
                             buttons[i].x1 + 2 + posX_ , 
                             buttons[i].y1 + 2 + posY_ ,
                             linecolor                    );
  }
}

void ButtonManager::draw()
{
  if (outputScreen_ == NULL) return;

  if (!isHide_)
  {
    MUTEX;

    for (int i=0; i<nbButton_; i++)
    {
      Image * selection = &buttons[i].grayedOut;
      Image * highlight = NULL;

      // NOT GRAYED OUT ?
      if (!buttons[i].isInactive_)
      {
        selection=&buttons[i].normal;

        // FULL PRESSED WITH THE MOUSE
        if (buttons[i].actionCode_ == pressed_)
          selection=&buttons[i].pressedDown;

        // HIGHLIGHTED BY DESIGNATION BY THE MOUSE
        else if (buttons[i].actionCode_ == highlighted_)
        {
          if (buttons[i].isStatusOn) // on + highlight
            highlight=&buttons[i].onAndHighlighted;
          else  // off + highlight
            highlight=&buttons[i].highlighted;
        }
        // NORMAL
        else  if (buttons[i].isStatusOn) // on + no highlight
          highlight=&buttons[i].on;
      }
      
      if (!selection->is_empty())
      {
        // draw the button image
        outputScreen_->draw_image(*selection, buttons[i].x1 + posX_,
                                              buttons[i].y1 + posY_);
        if (highlight != NULL)
          ImageTools::drawImageLinearDodge(*outputScreen_,*highlight, 
                                              buttons[i].x1 + posX_,
                                              buttons[i].y1 + posY_);
        // Draw the text over the image
        if (!buttons[i].isInactive_)
        {
          unsigned char color[] = { 50, 50, 75 };
          outputScreen_->draw_text(buttons[i].txt,
                                  buttons[i].x1 + posX_ + buttons[i].txtx,
                                  buttons[i].y1 + posY_ + buttons[i].txty,
                                  color);
        }
      
      }
      else if (!buttons[i].isInactive_)
        drawRectangle_(i, 128,128,160);

        // debug mode
  #ifdef BUTTON_DEBUG
      drawRectangle_(i,128,128,160);
  #endif

    }
    MUTEXRELEASE;
  }
}

void ButtonManager::declareTextOnButton(int action, char * text, int x, int y)
{
  for (int i=0;i<nbButton_;i++)
    if (buttons[i].actionCode_==action)
    {
      strcpy(buttons[i].txt, text);
      buttons[i].txtx=x;
      buttons[i].txty=y;
    }
}

void ButtonManager::defineNewButton(int action, int x1,int y1,int x2,int y2,char * tooltip, 
                           char * caption, int cx, int cy)
{
  defineNewButton(action, x1,y1,x2,y2, tooltip);
  declareTextOnButton(action, caption, cx, cy);
}

void ButtonManager::defineNewButton(int action)
{
  if (nbButton_ >= NB_BUTTON_MAX)
    THROW_INTERNAL_ERROR("Max number of button reached (in defineNewButton())");

  buttons[nbButton_].actionCode_ = action;
  buttons[nbButton_].x1 = 0;
  buttons[nbButton_].y1 = 0;
  buttons[nbButton_].x2 = 0;
  buttons[nbButton_].y2 = 0;
  buttons[nbButton_].isInactive_   = false;
  buttons[nbButton_].isStatusOn    = false;
  buttons[nbButton_].isClickable_  = true;
  buttons[nbButton_].iscontinuous  = false;
  nbButton_++;
  strcpy(buttons[nbButton_].caption, "");
  strcpy(buttons[nbButton_].txt, "");
  buttons[nbButton_].txtx = buttons[nbButton_].txty = 0 ;
}

void ButtonManager::changeButton(int action, int x1, int y1, int x2, int y2)
{
  for (int i=0;i<nbButton_;i++)
    if (buttons[i].actionCode_==action)
    {
      buttons[i].x1 = x1;
      buttons[i].y1 = y1;
      buttons[i].x2 = x2;
      buttons[i].y2 = y2;
    }
}

void ButtonManager::defineNewButton(int action_number, int x1, int y1, int x2, int y2, char* txt)
{
  if (nbButton_ >= NB_BUTTON_MAX)
    THROW_INTERNAL_ERROR("Max number of button reached (in defineNewButton())");
  if (strlen(txt)>=CAPTION_TEXT_SIZE)
    THROW_INTERNAL_ERROR("Text is too long for button (in defineNewButton())");

  int i=nbButton_;
  buttons[i].actionCode_=action_number;
  buttons[i].x1 = x1;
  buttons[i].y1 = y1;
  buttons[i].x2 = x2;
  buttons[i].y2 = y2;
  buttons[i].isInactive_  = false;
  buttons[i].isStatusOn   = false;
  buttons[i].isClickable_ = true ;
  buttons[i].iscontinuous = false;
  nbButton_++;
  strcpy(buttons[i].caption, "");
  strcpy(buttons[i].txt, "");
  buttons[i].txtx = buttons[i].txty = 0 ;
  strcpy(buttons[i].caption, txt);
}

void ButtonManager::loadAllImages(const Image& normal)
{
  for (int i=0; i<nbButton_;i++)
    buttons[i].normal=normal.get_crop(buttons[i].x1,buttons[i].y1,buttons[i].x2,buttons[i].y2);
}

void ButtonManager::loadAllImagesHoover(const Image& highlighted)
{
  for (int i=0; i<nbButton_;i++)
    buttons[i].highlighted=highlighted.get_crop(buttons[i].x1,buttons[i].y1,buttons[i].x2,buttons[i].y2);
}

void ButtonManager::loadAllImagesInactive(const Image& grayedOut)
{
  for (int i=0; i<nbButton_;i++)
    buttons[i].grayedOut=grayedOut.get_crop(buttons[i].x1,buttons[i].y1,buttons[i].x2,buttons[i].y2);
  }

void ButtonManager::loadAllImagesClicked(const Image& on)
{
  for (int i=0; i<nbButton_;i++)
    buttons[i].on=on.get_crop(buttons[i].x1,buttons[i].y1,buttons[i].x2,buttons[i].y2);
}

void ButtonManager::loadAllImagesClickedAndSelected(const Image& onAndHighlighted)
{
  for (int i=0; i<nbButton_;i++)
    buttons[i].onAndHighlighted=onAndHighlighted.get_crop(buttons[i].x1,buttons[i].y1,buttons[i].x2,buttons[i].y2);
}

void ButtonManager::loadAllImagesSelected(const Image& pressedDown)
{
  for (int i=0; i<nbButton_;i++)
    buttons[i].pressedDown=pressedDown.get_crop(buttons[i].x1,buttons[i].y1,buttons[i].x2,buttons[i].y2);
}

void ButtonManager::repaintStoredButtons(int hue, float saturation)
{
  for (int i=0; i<nbButton_; i++)
  {
    buttons[i].grayedOut        = ImageTools::mixTint(buttons[i].grayedOut,        hue);
    buttons[i].normal           = ImageTools::mixTint(buttons[i].normal,           hue);
    buttons[i].pressedDown      = ImageTools::mixTint(buttons[i].pressedDown,      hue);

    buttons[i].grayedOut        = ImageTools::mixDesaturateRedChannelOnly(buttons[i].grayedOut,        saturation);
    buttons[i].normal           = ImageTools::mixDesaturateRedChannelOnly(buttons[i].normal,           saturation);
    buttons[i].pressedDown      = ImageTools::mixDesaturateRedChannelOnly(buttons[i].pressedDown,      saturation);
  }

  // repaint background image
  background_ = ImageTools::mixTint(background_, hue);
  background_ = ImageTools::mixDesaturateRedChannelOnly(background_, saturation);
}

void ButtonManager::clearAllButtons()
{
  for (int i=0; i<nbButton_; i++)
  {
    buttons[i].grayedOut        = Image();
    buttons[i].normal           = Image();
    buttons[i].highlighted      = Image();
    buttons[i].on               = Image();
    buttons[i].onAndHighlighted = Image();
    buttons[i].pressedDown      = Image();
  }

  nbButton_=0;
  background_ = Image();
}


int ButtonManager::getSizeX()
{
  return sizex_;
}

int ButtonManager::getSizeY()
{
  return sizey_;
}

int ButtonManager::getPositionX()
{
  return posX_;
}

int ButtonManager::getPositionY()
{
  return posY_;
}

void ButtonManager::setPos(int x, int y)
{
  posX_ = x;
  posY_ = y;
}

bool  ButtonManager::isInsideInterface(int mousex, int mousey)
{
  if (!isHide_)
    if (mousex - posX_ >= 0     && mousey - posY_ >=0       && 
        mousex - posX_ < sizex_ && mousey - posY_ < sizey_  )
        return true;

  return false;
}

int  ButtonManager::detectUserAction(int mousex, int mousey, bool mousebutton)
{
  int action = ACTION_CODE_NO_ACTION;

  if (!isHide_)
  {
    // initialises launch_action with a code that says "no action"
    int hoover = detectCursorHoover(mousex - posX_ , mousey - posY_ );

    // detects if hoover changed
    bool ischangedhoover = (hoover != lasthoover_);
    lasthoover_ = hoover;

    // detects if mouse button changed
    bool ischangedbutton = (mousebutton != lastButtonState_);
    bool isreleased = (lastButtonState_ != 0 && mousebutton == 0);

    if (ischangedhoover) 
      action = ACTION_CODE_REPAINT;
    if (ischangedbutton)
      action = ACTION_CODE_REPAINT;

    bool iscontinuous = isContinuous(hoover);

    // if button is not pressed
    if (!mousebutton)
    {
      cancelPressed_();
      setHighlight_(hoover);
    }

    // the mouse button is RELEASED over a button = it lauches the action
    if (isreleased && hoover>=0 && !iscontinuous)
      action=hoover;
    
    if (mousebutton)
    { // pressed
      setPressed_(hoover);

      if (iscontinuous)
        action = hoover;
    }
  }

  lastButtonState_ = mousebutton;

  return action;
}

void ButtonManager::createWindow(HWND parenthWnd, HINSTANCE hInstance)
{
  if (parenthWnd == 0)
    THROW_INTERNAL_ERROR( "ButtonManager::createWindow : Wrong parameter 1" );
  if (hInstance == 0)
    THROW_INTERNAL_ERROR( "ButtonManager::createWindow : Wrong parameter 2" );
  //if (sizex_ <= 0 || sizey_ <= 0)
    //THROW_INTERNAL_ERROR( "ButtonManager::createWindow : size is zero" );

  // create a static window
  hWnd_ = CreateWindow(TEXT("STATIC"), TEXT(""), 
              WS_CHILD | WS_VISIBLE | SS_OWNERDRAW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
              posX_, posY_, sizex_, sizey_,
              parenthWnd, NULL, hInstance, NULL);    

  if (hWnd_ == 0)
    THROW_INTERNAL_ERROR( "ButtonManager::createWindow : Could not create a static control" );
}

void ButtonManager::updateWindow(int x, int y, int w, int h)
{
  if (hWnd_ == 0)
    THROW_INTERNAL_ERROR( "ButtonManager::updateWindow : no window" );
  if (w <= 0 || h <= 0)
    THROW_INTERNAL_ERROR( "ButtonManager::updateWindow : size is zero" );

  sizex_ = w;
  sizey_ = h;

  MoveWindow(hWnd_, x , y , w, h , TRUE);
}

HWND ButtonManager::getWindowNr()
{
  return hWnd_;
}

void ButtonManager::hide()
{
  isHide_ = true;
}
void ButtonManager::show()
{
  isHide_ = false;
}
bool ButtonManager::isHidden()
{
  return isHide_ ;
}

bool ButtonManager::getButton(int action, int& x1, int& y1, int& x2, int& y2, char * text) const
{
  if (!isHide_)
    for (int i=0;i<nbButton_;i++)
      if (buttons[i].actionCode_==action)
      {
        x1 = buttons[i].x1 + posX_;
        y1 = buttons[i].y1 + posY_;
        x2 = buttons[i].x2 + posX_;
        y2 = buttons[i].y2 + posY_;
        strcpy( text, buttons[i].caption ) ;

        return true;
      }

  return false;
}

bool ButtonManager::isButton(int action) const
{
  if (!isHide_)
    for (int i=0;i<nbButton_;i++)
      if (buttons[i].actionCode_==action)
        return true;

  return false;
}

void ButtonManager::display()
{
  if (outputScreen_ != NULL && hWnd_ != 0)
  {
    HDC hdc = GetDC(hWnd_);
    int displayw = sizex_ , displayh = sizey_ ;

    if (outputScreen_->dimx() < displayw + posX_) displayw = outputScreen_->dimx() - posX_;
    if (outputScreen_->dimy() < displayh + posY_) displayh = outputScreen_->dimy() - posY_;

    if (displayw>0 && displayh>0)
    {
      Image view = outputScreen_->get_crop( posX_,  posY_,
                                            posX_ + displayw -1, posY_ + displayh -1);
      Image alpha;
      
      if (!view.is_empty())
      {
        bool isok;
        if (alphaImage_.is_empty())
          isok= Win32Tools::copyImageToDC(view, hdc, alphaValue_);
        else
          isok= Win32Tools::copyImageToDC(view, hdc, alphaImage_);

        ReleaseDC(hWnd_, hdc);

        if (!isok)
          THROW_INTERNAL_ERROR("Could not display interface (in displayCenterView())");
      }
    }
  }
}

void ButtonManager::setAlpha(unsigned char alpha)
{
  alphaValue_ = alpha;
}

void ButtonManager::setAlpha(const Image& alphaimage)
{
  alphaImage_ = alphaimage;
}

void ButtonManager::drawSolidRectangleOnImage(Image& image, const unsigned char* colorpointer)
{
  int width  = background_.dimx();
  int height = background_.dimy();
  image.draw_rectangle(posX_, posY_,
                       posX_+ width -1, posY_+height-1, colorpointer);
}