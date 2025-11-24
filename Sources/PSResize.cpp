/*
 * PSInterfaceResize.cpp
 *
 *  Created on: 18 avr. 2009
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */
#define _WIN32_WINNT 0x0500

#include "PSApplication.h"

void PSApplication::resizeMainWindow(int newClientSizeX, int newClientSizeY)
{
  DEBUG(resiainWin);

  if (newClientSizeX <= 0 || newClientSizeY <= 0) return;
  int contentHeight;
  if (!interface_.isSculptBarMinimised_)
    contentHeight = newClientSizeY - interface_.bottomBar_.getSizeY() - interface_.topTabBar_.getSizeY() ;
  else
    contentHeight = newClientSizeY - interface_.bottomMiniBar_.getSizeY() - interface_.topTabBar_.getSizeY() ;

  interface_.image_.resize ( newClientSizeX, newClientSizeY, -100,-100,0);
  interface_.alpha_.resize ( newClientSizeX, newClientSizeY, -100,-100,0);

  if (newClientSizeX<50 || contentHeight<50) return;

  int contentPosY   = interface_.topTabBar_.getSizeY() ; 
  int bottomBarPosX, bottomBarPosY;
  if (!interface_.isSculptBarMinimised_)
  {
    bottomBarPosX= (newClientSizeX - interface_.bottomBar_.getSizeX())/2 ;
    bottomBarPosY = newClientSizeY - interface_.bottomBar_.getSizeY() ;
  }
  else
  {
    bottomBarPosX = (newClientSizeX- interface_.bottomMiniBar_.getSizeX())/2 ;
    bottomBarPosY = newClientSizeY - interface_.bottomMiniBar_.getSizeY() ;
  }

  interface_.contentZone_.setPos   ( 0 , contentPosY );
  interface_.bottomBar_  .setPos   ( bottomBarPosX, bottomBarPosY );
  interface_.bottomMiniBar_.setPos ( bottomBarPosX, bottomBarPosY );
  interface_.bottomSolo_ .setPos   ( bottomBarPosX, bottomBarPosY );
  interface_.bottomZone_ .setPos   ( 0, bottomBarPosY );

  interface_.contentZone_.updateWindow  ( 0, contentPosY, newClientSizeX, contentHeight);
  if (!interface_.isSculptBarMinimised_)
    interface_.bottomZone_ .updateWindow  ( 0, bottomBarPosY, newClientSizeX, interface_.bottomBar_.getSizeY() );
  else
    interface_.bottomZone_ .updateWindow  ( 0, bottomBarPosY, newClientSizeX, interface_.bottomMiniBar_.getSizeY() );

  initializeProgressionBar();

  reinit3DDisplay();
  requestRedraw();
}