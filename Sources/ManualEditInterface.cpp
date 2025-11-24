/*
 * ManualEditInterface.cpp
 *
 *  Created on: 19 avr. 2009
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */


#include "ManualEditInterface.h"
#include "RESOURCE.H"
#include "Win32Tools.h"
#include "PhotoSculptTools.h"
#include "Limit.h"
#include "Includes.h"
#include "Color.h"

ButtonManager ManualEditInterface::button_;

// Constructor of the interface
ManualEditInterface::ManualEditInterface(PhotoSculpt &sculpt)
{
  sculpt_ = &sculpt;

  inputMapLevel_    = sculpt_ -> computeLevel();
  inputMapSizeX_    = sculpt_ -> computeLeftImage(inputMapLevel_).dimx();
  inputMapSizeY_    = sculpt_ -> computeLeftImage(inputMapLevel_).dimy();

  editedMapLevel_   = sculpt_ -> computeLevelUsingSize(1200,800);

  int outputLevel   = sculpt_ -> computeLevelUsingSize(100,100);
  Image tempImage   = sculpt_ -> computeLeftImage(outputLevel);

  outputMapSizeX_   = tempImage.dimx();
  outputMapSizeY_   = tempImage.dimy();


  editedLeftImage_  = sculpt_ -> computeLeftImage(editedMapLevel_);
  editedRightImage_ = sculpt_ -> computeRightImage(editedMapLevel_);
  editedMapSizeX_   = editedLeftImage_.dimx();
  editedMapSizeY_   = editedLeftImage_.dimy();

  // sets the working displacement map to this size
  editedMap_        = sculpt_ -> getMap().getResizeAndMultiply(editedMapSizeX_,editedMapSizeY_);


  // returns an unmultiplied map
  //editedMap_        = editedMap_ * (1.0f / MULTIPLY); // * (multiply_ / multiply) ;

  skip_             = 128  ;

  linecolorleft[0]  = 218 ; // orange
  linecolorleft[1]  = 120 ;
  linecolorleft[2]  = 50  ;

  // reload the background image for the buttons
  background_       = Image(60,600,1,3).fill(255);

  bandSize_         = 100;
  xoffset_          = background_.dimx() + bandSize_;
  yoffset_          = bandSize_;

  windowSizeX_      = editedMapSizeX_ + xoffset_ + bandSize_;
  windowSizeY_      = editedMapSizeY_ + bandSize_ * 2;

  if (windowSizeY_ < background_.dimy()) 
  {
    windowSizeY_    = background_.dimy();
    yoffset_        = (background_.dimy() - editedMapSizeY_) / 2;
  }

  initialInterfaceImage_=Image(windowSizeX_, windowSizeY_,1,3);
  initialInterfaceImage_.fill(255);
  initialInterfaceImage_.draw_image(background_,0,0);
  initialInterfaceImage_.draw_rectangle(xoffset_-1,yoffset_-1,
    editedMapSizeX_ + xoffset_, editedMapSizeY_ + yoffset_, linecolorleft);

  createOverlayImage(initialInterfaceImage_,editedLeftImage_,editedRightImage_,
    editedMap_, xoffset_,  yoffset_);

  button_.set_output(initialInterfaceImage_);

  imageBuffer_ = initialInterfaceImage_;
  displayGrid (imageBuffer_, editedMap_,skip_, linecolorleft,  xoffset_,  yoffset_);
  window_ = CImgDisplay(imageBuffer_, "Manual Adjustment");
}


void ManualEditInterface::displayGrid(Image& i, const VectorMap& D, int skip,
                                      const unsigned char* linecolorleft, int x,int y)
{
  int sizex = D.getSizeX();
  int sizey = D.getSizeY();

  for   (int gx = 0; gx < sizex; gx += skip)
    for (int gy = 0; gy < sizey; gy += skip)
    {
      Vector R = D.get(gx,gy) / MULTIPLY;
      R += Vector(gx,gy);

      //horizontal lines
      if (gx + skip < sizex) {
        Vector Rx = D.get(gx + skip, gy) / MULTIPLY;
        Rx += Vector(gx + skip, gy);

        i.draw_line(Rx.x()+x, Rx.y()+y, R.x()+x, R.y()+y, linecolorleft, 1.0);
      }

      //vertical lines
      if (gy + skip < sizey) {
        Vector Ry = D.get(gx, gy + skip) / MULTIPLY;
        Ry += Vector(gx, gy + skip);

        i.draw_line(Ry.x()+x, Ry.y()+y, R.x()+x, R.y()+y, linecolorleft, 1.0);
      }
    }
}

// show either the left image or the displaced right image
void ManualEditInterface::createOverlayImage(Image& dest,const Image& left,const Image& right,
                                             const VectorMap& map, int coordx, int coordy)
{
  int sizex = left.dimx();
  int sizey = left.dimy();

  //Image displacedRightToLeft = PhotoSculptTools::generateMorphed(right, map, MULTIPLY, false);

  Image displacedRightToLeft = PhotoSculptTools::generateMorphedLeft(left, map, false);
  for (int x=0;x<sizex;x++)
    for (int y=0;y<sizey;y++)
    {
      bool odd = bool( (x + y) % 2 );
      Color c;
      if (odd) c=getPixel(displacedRightToLeft,x,y);
      else     c=getPixel(right,x,y);

      setPixel(dest,x+coordx,y+coordy,c);
    }
}

void ManualEditInterface::initialize(HINSTANCE h)
{
  // Warning this function does not define the button 'output' field. it will lead to a program crash
  // if member function set_output is not used

  button_.define_new_button(ACTION_MANUAL_EDIT_PLUS,   10,87,	  40,125, "+"      );
  button_.define_new_button(ACTION_MANUAL_EDIT_MINUS,  10,127,	40,168, "-"      ); 
  button_.define_new_button(ACTION_MANUAL_EDIT_LEFT,   10,285,	40,323, "front"   );
  button_.define_new_button(ACTION_MANUAL_EDIT_RIGHT,  10,326,	40,368, "right"  );
  button_.define_new_button(ACTION_MANUAL_EDIT_CLEAR,  10,450,	40,490, "clear"  );
  button_.define_new_button(ACTION_MANUAL_EDIT_OK,     10,507,	40,547, "OK"     );
  button_.define_new_button(ACTION_MANUAL_EDIT_CANCEL, 10,549,	40,591, "Cancel" );
}


void ManualEditInterface::draw_()
{
  createOverlayImage(imageBuffer_,editedLeftImage_,editedRightImage_,editedMap_, xoffset_,yoffset_);
  displayGrid(imageBuffer_,editedMap_,skip_, linecolorleft, xoffset_,yoffset_);
}


#include "POV.h"

VectorMap ManualEditInterface::launch()
{
  int x = -1, y = -1;
  bool isValidated = false, isEscape = false;

  while (!window_.is_closed && !isEscape && !isValidated)
  {
    if (window_.is_resized)
      window_.resize(initialInterfaceImage_);

    window_.wait();

    int mx= window_.mouse_x, my = window_.mouse_y;
    x = mx - xoffset_;	y = my - yoffset_;

    int hoover= button_.detectCursorHoover(mx,my);

    int pressed=-1;
    if (window_.button==1)
      pressed=hoover;

    // read keyboard and lauch action
    if (window_.is_keyPADADD)     pressed = ACTION_MANUAL_EDIT_PLUS  ;
    if (window_.is_keyPADSUB)     pressed = ACTION_MANUAL_EDIT_MINUS ;
    if (window_.is_keyENTER)      pressed = ACTION_MANUAL_EDIT_OK ;
    if (window_.is_keyESC)        pressed = ACTION_MANUAL_EDIT_CANCEL ;
    if (window_.is_keyARROWLEFT)  pressed = ACTION_MANUAL_EDIT_LEFT ;
    if (window_.is_keyARROWRIGHT) pressed = ACTION_MANUAL_EDIT_RIGHT ;

    button_.draw();

    imageBuffer_=initialInterfaceImage_;
    draw_();

    switch (pressed)
    {
      case ACTION_MANUAL_EDIT_PLUS:
      case ACTION_MANUAL_EDIT_MINUS:
        {
          if (pressed == ACTION_MANUAL_EDIT_PLUS)  skip_ /= 2;
          if (pressed == ACTION_MANUAL_EDIT_MINUS) skip_ *= 2;
          limit(skip_,8,512);

          draw_();

          while (window_.is_keyPADADD || window_.is_keyPADSUB || window_.button) ;
          break;
        }
      case ACTION_MANUAL_EDIT_OK:       isValidated = true;     break;
      case ACTION_MANUAL_EDIT_CANCEL:   isEscape    = true;     break;
      case ACTION_MANUAL_EDIT_RESET: 
        editedMap_ = sculpt_ -> getMap().getResizeAndMultiply(editedMapSizeX_,editedMapSizeY_);
        break;

      case ACTION_MANUAL_EDIT_CLEAR:    
        {
          int sizex = editedMap_.getSizeX();
          int sizey = editedMap_.getSizeY();

          POV pov; // creates a default point of view
          editedMap_ = pov.generateMapOfFlatObject(sizex,sizey) ;

          
          //editedMap_.fill(0);     
          break;
        }

      case ACTION_MANUAL_EDIT_LEFT:
        {
          Image displaced = PhotoSculptTools::generateMorphedLeft(editedLeftImage_, editedMap_, 
            false);

          imageBuffer_.draw_image(displaced, xoffset_,  yoffset_);
          break;
        }
      case ACTION_MANUAL_EDIT_RIGHT:
        imageBuffer_.draw_image(editedRightImage_, xoffset_,  yoffset_);
        break;
    }

    int bestmapx, bestmapy;
    bool isDotDetected = false;

    if (mx>=0 && my>=0)
    {
      displayGrid(imageBuffer_,editedMap_,skip_, linecolorleft,  xoffset_,  yoffset_);

      const int DETECTION_PIXEL_SIZE = 20;
      long bestdistance = 100000;
      for   ( int gx = 0; gx < editedMapSizeX_ ; gx += skip_ )
        for ( int gy = 0; gy < editedMapSizeY_ ; gy += skip_ )
        {
          Vector R = editedMap_.get(gx,gy) / MULTIPLY;
          int distance = (x-gx-R.x())*(x-gx-R.x())+(y-gy-R.y())*(y-gy-R.y());

          if (distance < bestdistance && distance < DETECTION_PIXEL_SIZE * DETECTION_PIXEL_SIZE)
          {
            bestdistance = distance;
            bestmapx = gx; bestmapy = gy;
            isDotDetected = true;
          }
        }

      if ( isDotDetected )
      {
        // displays a 5x5 pixel dot
        Vector d = editedMap_.get(bestmapx,bestmapy) / MULTIPLY;
        int screenCoordDotx = xoffset_ + bestmapx + d.x();
        int screenCoordDoty = yoffset_ + bestmapy + d.y();
        imageBuffer_.draw_rectangle(screenCoordDotx-2, screenCoordDoty-2,
                                    screenCoordDotx+2, screenCoordDoty+2, linecolorleft, 1.0);

        if (window_.button)
        {
          int nmx = window_.mouse_x - xoffset_, nmy = window_.mouse_y - yoffset_;
          Vector oldD = editedMap_.get(bestmapx,bestmapy);

          while (window_.button && window_.mouse_x)
          {
            mx= window_.mouse_x, my = window_.mouse_y;
            x=mx-xoffset_;	y=my-yoffset_;

            int movex = round( float( x-nmx ) * MULTIPLY ) ;
            int movey = round( float( y-nmy ) * MULTIPLY ) ;

            editedMap_.set(bestmapx,bestmapy, oldD.x()+movex, oldD.y()+movey);

            VectorMap tempmap = PhotoSculptTools::reInterpolateMap(editedMap_,skip_);

            for (int iy=bestmapy-skip_;iy<bestmapy+skip_;iy++)
              for (int ix=bestmapx-skip_;ix<bestmapx+skip_;ix++)
                if (ix>=0 && iy>=0 && ix<editedMap_.getSizeX() && iy<editedMap_.getSizeY())
                  editedMap_.set (ix,iy, tempmap.get(ix,iy));

            imageBuffer_=initialInterfaceImage_;
            draw_();
            window_.display(imageBuffer_);
          }
        }
      }
    }

    window_.display(imageBuffer_);
  }

  // closes the manual edit window
  close();

  if (!isValidated)
    return VectorMap(); // escape key was pressed

  VectorMap v=editedMap_.getResizeAndMultiply(outputMapSizeX_,outputMapSizeY_);

  return v;
}

void ManualEditInterface::close()
{
  window_.close();
}

