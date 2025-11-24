/*
 * PSInterfaceInit.cpp
 *
 *  Created on: 18 avr. 2009
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */

#define _WIN32_WINNT 0x0500

#include <windows.h>
#include <fstream>
#include <Commctrl.h>

#include "PSApplication.h"
#include "resource.h"
#include "Win32Tools.h"
#include "View3D.h"
#include "ExportObj.h"
#include "DirectXControl.h"
#include "Limit.h"
#include "Includes.h"
#include "BurstMap.h"
#include "ToolTip.h"
#include "ImageFrame.h"
#include "DirectXControl.h"

LRESULT CALLBACK	WindowsProcedure(HWND, UINT, WPARAM, LPARAM);

PSApplication::PSApplication()
{  
  DEBUG(PSAp);
  interface_.instanceHandle_= 0;
}

Options * official_option;

void PSApplication::init(HINSTANCE hInstance)
{

}

void PSApplication::createMainInterface(HINSTANCE hinstance, int isshow)
{
  DEBUG(creMaIn);

  if (interface_.instanceHandle_ != 0)
    THROW_INTERNAL_ERROR("Attempt to initialise interface twice");

  BurstMap::init(BURSTMAP_RANGE_IN_PIXEL, BURSTMAP_FIXED_COMA);

  isSculpting_                = false ;
  isCurrentModelJustSculpted_ = false ;

  sources_.leftFileName_ [0]  = 0     ;
  sources_.rightFileName_[0]  = 0     ;

  registry_   .setRegistryKey(REGISTRY_KEY);
  registryAll_.setRegistryKey(REGISTRY_KEY_LM);
  registryAll_.setAllUsers();

  OptionStore::loadFromRegistry(GlobalOptions, &registry_);
  official_option = &GlobalOptions;

  interface_.createMainWindow(hinstance, isshow, registry_);
  applyOptions();
  ShowWindow(interface_.windowsHandle_, isshow);

  updateMenuRecentFiles_();
  ProgressDialog::init(interface_.instanceHandle_, interface_.windowsHandle_);
}

void PSApplication::setTrackMouseLeave()
{
  track_.set(interface_.windowsHandle_);
}

void PSApplication::resetTrackMouseLeave()
{
  track_.reset();
}
