/*
 * Editor.cpp
 *
 *  Created on: 06 fev. 2011
  *      Author: Hippolyte Mounier, Copyright 2011 All rights reserved
 */
#include "PinEditor.h"
#include "Win32Tools.h"
#include "Frame.h"
#include "Square.h"

#define MAX_LOADSTRING 100

unsigned char COLOR_CREATE[] = { 0,  255,0   };
unsigned char COLOR_DELETE[] = { 255,0,  0   };
unsigned char COLOR_NORMAL[] = { 255,255,128 } ;

PinEditor::PinEditor(HINSTANCE hinstance, HWND parenthwnd, PSApplication *p):
  DialogWindow(hinstance,  parenthwnd, IDS_APP_TITLE_EDIT, 
                         IDC_EDITOR, IDI_SMALL, IDC_EDITOR)
{
  isOk_ = false;

  psinterface_    = p   ;

  initButtons();
  editMode_ = ACTION_EDIT_MODE_ADD_POINT;

  map_ = psinterface_->getMap();
  if (map_.isEmpty())
    THROW_INTERNAL_ERROR( "Sculpt map is empty, please init sculpt first (in initMap())");

  editCoordinates_ = psinterface_->getEditCoordinates();
}

void PinEditor::displayToWindow()
{
  HDC hdc = GetDC(paintZonehWnd_);

  renderSourceImages();
  buttons_.draw();

  float floatlx, floatly,floatrx, floatry;
  bool isInLeft = sources_.isCursorInImageL(mouseX_,mouseY_,floatlx,floatly);
  bool isInRight = sources_.isCursorInImageR(mouseX_,mouseY_,floatrx,floatry);

  if (editMode_ == ACTION_EDIT_MODE_ADD_POINT)
    if (isInLeft && mouseButton_==0)
    {
      float floatrx, floatry;

      sources_.correlateLtoR(floatlx,floatly, floatrx,floatry, map_);

      sources_.drawLine   (floatlx, floatly, floatrx, floatry, interfaceImage_, COLOR_CREATE, 1.0f);
    }

  drawConstraints();

  if (editMode_ == ACTION_EDIT_MODE_DELETE || editMode_ == ACTION_EDIT_MODE_MOVE_POINT || 
      editMode_ == ACTION_EDIT_MODE_ADD_POINT)
  {
    unsigned char* color = COLOR_CREATE;
    if (editMode_ == ACTION_EDIT_MODE_DELETE)
      color=COLOR_DELETE;

    if (isInLeft && (editMode_ == ACTION_EDIT_MODE_DELETE || editMode_ == ACTION_EDIT_MODE_MOVE_POINT ))
    {
      int indexl = searchConstraintL(floatlx, floatly, snapDistance_);
      if (indexl>=0)
      {
        float lx,ly,rx,ry;
        editCoordinates_.get(lx,ly,rx,ry,indexl);
        sources_.drawPointL (lx, ly, interfaceImage_, color);
        sources_.drawLine(lx,ly,rx,ry,interfaceImage_, color, 1.0f);
      }
    }
  
    if (isInRight)
    {
      int indexr = searchConstraintR(floatrx, floatry, snapDistance_);
      if (indexr>=0)
      {
        float lx,ly,rx,ry;
        editCoordinates_.get(lx,ly,rx,ry,indexr);
        sources_.drawPointR (rx, ry, interfaceImage_, color);
        sources_.drawLine(lx,ly,rx,ry,interfaceImage_, color, 1.0f);
      }
    }
  }

  bool isOk2 = Win32Tools::copyImageToDC(interfaceImage_, hdc);

  ReleaseDC(paintZonehWnd_, hdc);
}

void PinEditor::detectAction()
{
  int action = buttons_.detectUserAction(mouseX_,mouseY_,mouseButton_);

  switch (action)
  {
    case ACTION_EDIT_MODE_ADD_POINT  :
    case ACTION_EDIT_MODE_ADD_LINE   :
    case ACTION_EDIT_MODE_DELETE     :
    case ACTION_EDIT_MODE_MOVE_POINT : setMode(action);   break;

    case ACTION_EDIT_OK:        finalise(); destroy();    break;
    case ACTION_EDIT_CANCEL:    destroy();                break;
    case ACTION_REPAINT_SCREEN: requestRedraw();          break;
  }

  float floatlx, floatly,floatrx, floatry;
  bool isInLeft = sources_.isCursorInImageL(mouseX_,mouseY_,floatlx,floatly);
  bool isInRight = sources_.isCursorInImageR(mouseX_,mouseY_,floatrx,floatry);

  static int oldButton = 0;
  bool released = (oldButton==1 && mouseButton_==0);
  oldButton=mouseButton_;

  if (released)
  {
    if (editMode_ == ACTION_EDIT_MODE_DELETE)
    {
      if (isInLeft)
      {
        int index = searchConstraintL(floatlx, floatly, snapDistance_);
        if (index>=0)
          // deleting this line
          editCoordinates_.del(index);
      }

      if (isInRight)
      {
        int index = searchConstraintR(floatrx, floatry, snapDistance_);
        if (index>=0)
          // deleting this line
          editCoordinates_.del(index);
      }
    }
  }

  if (mouseButton_)
  {
    if (isInLeft && editMode_ == ACTION_EDIT_MODE_MOVE_POINT)
    {
      int index = searchConstraintL(floatlx, floatly, snapDistance_);
      if (index>=0)
      { 
        // moving left dot
        float lx,ly,rx,ry;
        editCoordinates_.get(lx,ly,rx,ry,index);
        lx = floatlx ; ly = floatly;
        editCoordinates_.set(lx,ly,rx,ry,index);
      }
    }

    if (isInRight && (editMode_ == ACTION_EDIT_MODE_MOVE_POINT ||
                      editMode_ == ACTION_EDIT_MODE_ADD_POINT  )   )
    {
      int index = searchConstraintR(floatrx, floatry, snapDistance_);
      if (index>=0)
      { 
        // moving right dot
        float lx,ly,rx,ry;
        editCoordinates_.get(lx,ly,rx,ry,index);
        rx = floatrx ; ry = floatry;
        editCoordinates_.set(lx,ly,rx,ry,index);
      }
    }
  }
  
  if (released && isInLeft && editMode_ == ACTION_EDIT_MODE_ADD_POINT)
    dropConstraint();
}

void PinEditor::dropConstraint()
{
  float floatlx, floatly;

  if (sources_.isCursorInImageL(mouseX_,mouseY_,floatlx,floatly))
  {
    float floatrx, floatry;

    sources_.correlateLtoR(floatlx,floatly, floatrx,floatry, map_);

    editCoordinates_.add(floatlx,floatly, floatrx,floatry);
  }
}

void PinEditor::drawConstraints()
{
  int count = editCoordinates_.getNb();
  if (count>0)
  {
    for (int i=0; i<count; i++)
    {
      float lx,ly,rx,ry;
      editCoordinates_.get(lx,ly,rx,ry,i);

      sources_.drawLine(lx,ly,rx,ry,interfaceImage_, COLOR_NORMAL, 0.8f);
    }
  }
}

int PinEditor::searchConstraintL(float x, float y, float withindist)
{
  int count = editCoordinates_.getNb();
  
  float bestdistance = withindist;
  int returnindex = -1 ;

  if (count>0)
  {
    for (int i=0; i<count; i++)
    {
      float lx,ly,rx,ry;
      editCoordinates_.get(lx,ly,rx,ry,i);

      // computes the square distance of this constraint point to the cursor at x,y
      float currentdist = square(lx-x) + square(ly-y);
      if (currentdist < bestdistance)
      {
        bestdistance = currentdist;
        returnindex = i;
      }
    }
  }

  return returnindex;
}

int PinEditor::searchConstraintR(float x, float y, float withindist)
{
  int count = editCoordinates_.getNb();
  
  float bestdistance = withindist;
  int returnindex = -1 ;

  if (count>0)
  {
    for (int i=0; i<count; i++)
    {
      float lx,ly,rx,ry;
      editCoordinates_.get(lx,ly,rx,ry,i);

      // computes the square distance of this constraint point to the cursor at x,y
      float currentdist = square(rx-x) + square(ry-y);
      if (currentdist < bestdistance)
      {
        bestdistance = currentdist;
        returnindex = i;
      }
    }
  }

  return returnindex;
}

void PinEditor::renderSourceImages()
{
  Image& leftimage   = psinterface_->getLeftImage();
  Image& rightimage  = psinterface_->getRightImage();

  // white
  interfaceImage_.fill(255); 

  const int frame = 20;
  
  int windowsizex=getClientSizeX_() - frame;
  int windowsizey=getClientSizeY_() - frame - BAR_SIZE_Y;

  if (!leftimage.is_empty()) 
  {
    int resizedxl,resizedyl;

    Image leftreduced; 

    resizedxl = leftimage.dimx();
    resizedyl = leftimage.dimy();
    Frame::fit(resizedxl, resizedyl, windowsizex/2, windowsizey);

    if (resizedxl>0 && resizedyl>0)
      leftreduced  = leftimage .get_resize(resizedxl, resizedyl);

    int xl= getClientSizeX_() / 2 - resizedxl - 10;
    int yl= (getClientSizeY_() - BAR_SIZE_Y - resizedyl) / 2;

    if (!leftreduced.is_empty())
      interfaceImage_.draw_image(leftreduced, xl, yl);

    sources_.setL(xl,yl, resizedxl, resizedyl);
    snapDistance_= float (CURSOR_SNAP_DISTANCE) / float(resizedxl);
  }

  if (!rightimage.is_empty()) 
  {
    int resizedxr,resizedyr;
    Image rightreduced;

    resizedxr = rightimage.dimx();
    resizedyr = rightimage.dimy();
    Frame::fit(resizedxr, resizedyr, windowsizex/2, windowsizey);

    if (resizedxr>0 && resizedyr>0)
      rightreduced  = rightimage.get_resize(resizedxr, resizedyr);

    int xr = getClientSizeX_() / 2 + 10 ;
    int yr = (getClientSizeY_() - BAR_SIZE_Y - resizedyr) / 2 ;

    if (!rightreduced.is_empty())
      interfaceImage_.draw_image(rightreduced, xr, yr);

    sources_.setR(xr,yr, resizedxr, resizedyr);
    snapDistance_= float (CURSOR_SNAP_DISTANCE) / float(resizedxr);
  }
}

void PinEditor::resizeMainWindow(int newSizeX, int newSizeY)
{
  DialogWindow::resizeMainWindow(newSizeX, newSizeY);

  buttons_.setPos( (getClientSizeX_()-buttons_.getSizeX())/2 ,
                           getClientSizeY_()-buttons_.getSizeY()    );
}

bool PinEditor::isOk()
{
  return isOk_ ;
}

void PinEditor::finalise()
{
  psinterface_ -> setEditCoordinates(editCoordinates_);
  isOk_ = true ;
}

void PinEditor::setMode(int mode)
{
  editMode_ = mode;
}

void PinEditor::initButtons()
{
  buttons_ = ButtonManager();
  buttons_.setOutput(interfaceImage_);
  Image b(280,80,1,3);
  b.fill(200);
  buttons_.storeBackground(b);

  buttons_.defineNewButton (ACTION_EDIT_OK,              20,20,60,60,"ok");
  buttons_.defineNewButton (ACTION_EDIT_CANCEL,          60,20,100,60,"cancel");
  buttons_.defineNewButton (ACTION_EDIT_MODE_ADD_POINT,  100,20,140,60,"add point");
  buttons_.defineNewButton (ACTION_EDIT_MODE_DELETE,     180,20,220,60,"delete");
  buttons_.defineNewButton (ACTION_EDIT_MODE_MOVE_POINT, 220,20,260,60,"move point");

  buttons_.setPos( (getClientSizeX_()-buttons_.getSizeX())/2 ,
                           getClientSizeY_()-buttons_.getSizeY()     );
}