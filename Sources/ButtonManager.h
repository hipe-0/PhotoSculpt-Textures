/*
 * ButtonManager.h
 *
 *  Created on: 18 févr. 2009
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */
#pragma once

#include "Image.h"
#include "Obfuscate.h"
#include "Mutex.h"
#include "STRINGS.h"

class ButtonManager 
{
public:
  ButtonManager(); // warning : use setOutput to define the target image before any display

  void storeBackground    (Image & backg);
  void drawBackground     ();
  void setOutput          (Image & out); /// defines this image to be the default output image

  // DECLARATION OF BUTTONS
  void defineNewButton    (int action_number);
  void defineNewButton    (int action_number, int x1,int y1,int x2,int y2,char * txt);
  void changeButton       (int action_number, int x1,int y1,int x2,int y2);
 
  // STATE CHANGE FOR BUTTON LOGIC DESCRIPTION :
  void declare_grayed_out (int action);
  void declare_grayed_out (int i, int j, int k=-1, int l=-1, int m=-1, int n=-1, int o=-1, int p=-1);

  void declare_unsensible (int action);
  void declare_sensible   (int action);

  void declare_active     (int action);
  void declare_active     (int i, int j, int k=-1, int l=-1, int m=-1, int n=-1, int o=-1, int p=-1);

  void declareStatusOn    (int action);
  void declareStatusOff   (int action);

  void declare_all_grayed_out();
  void declare_all_active    ();
  void declareContinuous     (int action);
  bool isContinuous          (int action);

  void declareTextOnButton(int action, char * text, int x, int y);
  void defineNewButton    (int action_number, int x1,int y1,int x2,int y2,char * txt, 
                           char * caption, int cx, int cy);

  // DRAWING FUNCTIONS :
  void draw                 ();
  void drawSolidRectangleOnImage(Image& image, const unsigned char* colorpointer);

  void loadAllImages                   ( const Image& image );
  void loadAllImagesHoover             ( const Image& image );
  void loadAllImagesInactive           ( const Image& image );
  void loadAllImagesClicked            ( const Image& image );
  void loadAllImagesClickedAndSelected ( const Image& image );
  void loadAllImagesSelected           ( const Image& image );

  /// Tells if the pointer is inside the sensible area of a given button
  // returns: the button number as declared with the "defineNewButton" method
  int  detectCursorHoover   (int mousex,int mousey);
  bool isInside             (int mousex,int mousey, int action);
  int  detectUserAction     (int mousex, int mousey, bool mousebutton);
  bool isInsideInterface    (int mousex, int mousey);

  void scaleAll             (int x, int y);
  int  getSizeX             ();
  int  getSizeY             ();
  int  getPositionX         ();
  int  getPositionY         ();
  void setPos               (int x, int y);

  void createWindow         (HWND parenthWnd, HINSTANCE hInstance);
  void updateWindow         (int x, int y, int w, int h);
  HWND getWindowNr          ();

  void hide                 ();
  void show                 ();
  bool isHidden             ();

  bool getButton            (int action, int& x1, int& y1, int& x2, int& y2, char * text) const;
  bool isButton             (int action) const;
  void display              ();

  void repaintStoredButtons (int hue, float saturation);
  void clearAllButtons      ();
  void setAlpha             (unsigned char alpha);
  void setAlpha             (const Image& alpha);

private:
  void drawRectangle_       (int i,unsigned char r,unsigned char g,unsigned char b);
  void setPressed_          (int action);
  void cancelPressed_       ();
  void setHighlight_        (int action);
  void cancelHighlight_     ();

  static const int CAPTION_TEXT_SIZE=30;
  static const int DISPLAY_TEXT_SIZE=15;

  struct Button 
  {
    oint  actionCode_;	    // any number that the user is giving to that button
    oint  x1,y1,x2,y2;      // location on screen

    Image grayedOut; 	      // grayed out
    Image normal;           // off
    Image highlighted;     	// mouse hoover is on
    Image on; 	            // the button is on
    Image onAndHighlighted;	// the button is on and mouse hoover is on
    Image pressedDown;	    // the button is pressedDown

    obool isInactive_;
    obool isStatusOn;       // stores if the button is currently in ON or OFF state
    obool isClickable_;	    // stores if the button can be detected by the mouse
    obool iscontinuous;     // if true, then action launched multiple times

    char  caption [CAPTION_TEXT_SIZE];  // Text that is used as tool tip
    char  txt     [DISPLAY_TEXT_SIZE];	// Text that is used to print with the button
    oint  txtx, txty;
  };

  static const int NB_BUTTON_MAX         = 100;
  static const int ACTION_CODE_REPAINT   = -1;
  static const int ACTION_CODE_NO_ACTION = -2;

  Button     buttons[NB_BUTTON_MAX];  // a table of NB_BUTTON_MAX empty buttons

  Image      background_        ;  // stores the entire background of the buttons for future display
  Image*     outputScreen_      ;  // Where the button manager outputs the button display

  oint       nbButton_          ;  // Number of buttons used
  bool       lastButtonState_   ;
  oint       lasthoover_        ;

  oint       pressed_           ;
  oint       highlighted_       ;

  FakeMutex  mutex_             ;

  oint       posX_, posY_ ; // position of the background image inside the output image

  HWND       hWnd_              ;

  obool      isHide_            ;
  int        alphaValue_        ;
  Image      alphaImage_        ;

  int        sizex_, sizey_     ;
};

#define DEFINE(action, x1,y1, x2,y2) defineNewButton(ACTION_##action, x1,y1, x2,y2, STR(TOOLTIP_##action));

#define DEFINE2(action, x1,y1, x2,y2, cx,cy) defineNewButton(ACTION_##action, x1,y1, x2,y2, STR(TOOLTIP_##action), STR(CAPTION_##action), cx,cy);
