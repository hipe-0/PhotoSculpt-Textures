/*
 * GraphicUserInterface.cpp
 *
 *  Created on: 20 dec. 2010
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */

#include "GraphicUserInterface.h"
#include <dwmapi.h>
#include "Limit.h"
#include "Registry.h"

GraphicUserInterface::GraphicUserInterface()
{
  isSculptBarMinimised_ = false ;
  mouseButton_          = false ;
  mouseButtonR_         = false ;
  mouseX_               = 0     ;
  mouseY_               = 0     ;
  windowsHandle_        = 0     ;
  instanceHandle_       = 0     ;
  menuHandle_           = 0     ;
}

bool isAeroGlassDLL()
{
  DEBUG(isAer);
  HINSTANCE hDLL  = LoadLibrary("Dwmapi.dll");

  if(hDLL != 0)
  {
    FreeLibrary(hDLL);
    return true;
  }
  else 
    return false;
}

bool setAeroGlass(HWND windowshandle)
{
  // see http://www.codeproject.com/KB/vista/VGGlassIntro.aspx
  
  if (isAeroGlassDLL())
  {
    BOOL isDWMComposition;
    DwmIsCompositionEnabled (&isDWMComposition);
    if (isDWMComposition)
    {
      MARGINS margin = {-1,-1,-1,-1};
      DwmExtendFrameIntoClientArea ( windowshandle, &margin );
      return true;
    }
    else
      return false;
  }
  else 
    return false;
}

bool loadInterfacePosition(Registry& registry, int &returntop, int &returnbottom, 
                                        int &returnleft, int &returnright)
{
  DWORD top, bottom, left, right;
  bool ok1 = registry.loadDWord( "window top"    , top    );
  bool ok2 = registry.loadDWord( "window bottom" , bottom );
  bool ok3 = registry.loadDWord( "window left"   , left   );
  bool ok4 = registry.loadDWord( "window right"  , right  );

  returntop = top; returnbottom = bottom; returnleft = left ; returnright = right;

  // if at least one load operation fails, the function returns false
  return ok1 && ok2 && ok3 && ok4;
}

#include "MenuIcons.h"
void GraphicUserInterface::createTransparentMenuIcons()
{
  if (isCapableMenuIcons())
  {
    HICON hiconload        = LoadIcon(instanceHandle_, MAKEINTRESOURCE(IDI_MENUICON_LOAD));
    HICON hiconapplication = LoadIcon(instanceHandle_, MAKEINTRESOURCE(IDI_SMALL));
    HICON hicondisk        = LoadIcon(instanceHandle_, MAKEINTRESOURCE(IDI_MENUICON_DISK));
    HICON hicon2disks      = LoadIcon(instanceHandle_, MAKEINTRESOURCE(IDI_MENUICON_2DISKS));
    HICON hiconalpha       = LoadIcon(instanceHandle_, MAKEINTRESOURCE(IDI_MENUICON_ALPHA));
    HICON hiconcopy        = LoadIcon(instanceHandle_, MAKEINTRESOURCE(IDI_MENUICON_COPY));
    HICON hiconcross       = LoadIcon(instanceHandle_, MAKEINTRESOURCE(IDI_MENUICON_CROSS));
    HICON hiconcross2      = LoadIcon(instanceHandle_, MAKEINTRESOURCE(IDI_MENUICON_CROSS2));
    HICON hiconflat        = LoadIcon(instanceHandle_, MAKEINTRESOURCE(IDI_MENUICON_FLAT));
    HICON hiconincrease    = LoadIcon(instanceHandle_, MAKEINTRESOURCE(IDI_MENUICON_INCREASE));
    HICON hiconreduce      = LoadIcon(instanceHandle_, MAKEINTRESOURCE(IDI_MENUICON_REDUCE));
    HICON hiconsculpt      = LoadIcon(instanceHandle_, MAKEINTRESOURCE(IDI_MENUICON_SCULPT));
    HICON hiconstop        = LoadIcon(instanceHandle_, MAKEINTRESOURCE(IDI_MENUICON_STOP));
    HICON hicontileable    = LoadIcon(instanceHandle_, MAKEINTRESOURCE(IDI_MENUICON_TILEABLE));
    HICON hiconcrop        = LoadIcon(instanceHandle_, MAKEINTRESOURCE(IDI_MENUICON_CROP));
    HICON hiconquestion    = LoadIcon(instanceHandle_, MAKEINTRESOURCE(IDI_MENUICON_QUESTION));
    HICON hiconlaboratory  = LoadIcon(instanceHandle_, MAKEINTRESOURCE(IDI_MENUICON_LABORATORY));
    HICON hiconfolder      = LoadIcon(instanceHandle_, MAKEINTRESOURCE(IDI_MENUICON_FOLDER));
    HICON hiconwheels      = LoadIcon(instanceHandle_, MAKEINTRESOURCE(IDI_MENUICON_WHEELS));

    AddIconToMenuItem  (menuHandle_, ACTION_LOAD_SCULPT_FILE,     false, hiconload,        false, 0);
    AddIconToMenuItem  (menuHandle_, ACTION_OPEN_LEFT_IMAGE,      false, hiconload,        false, 0);
    AddIconToMenuItem  (menuHandle_, ACTION_OPEN_RIGHT_IMAGE,     false, hiconload,        false, 0);
    AddIconToMenuItem  (menuHandle_, ACTION_ABOUT,                false, hiconquestion,    false, 0);
    AddIconToMenuItem  (menuHandle_, ACTION_WEBSITE,              false, hiconapplication, false, 0);
    AddIconToMenuItem  (menuHandle_, ACTION_SAVE_AS_IMAGE,        false, hicondisk,        false, 0);
    AddIconToMenuItem  (menuHandle_, ACTION_SAVE_MODEL_FILE,      false, hicondisk,        false, 0);
    AddIconToMenuItem  (menuHandle_, ACTION_SAVE_MULTIPLE,        false, hicon2disks,      false, 0);
    AddIconToMenuItem  (menuHandle_, ACTION_EDIT_SELECTION,       false, hiconalpha,       false, 0);
    AddIconToMenuItem  (menuHandle_, ACTION_COPY_TO_CLIPBOARD,    false, hiconcopy,        false, 0);
    AddIconToMenuItem  (menuHandle_, ACTION_ERASE_RESULT,         false, hiconcross,       false, 0);
    AddIconToMenuItem  (menuHandle_, ACTION_CLEAR_ALL,            false, hiconcross2,      false, 0);
    AddIconToMenuItem  (menuHandle_, ACTION_FLAT_MAP,             false, hiconflat,        false, 0);
    AddIconToMenuItem  (menuHandle_, ACTION_SUBDIVISION_INCREASE, false, hiconincrease,    false, 0);
    AddIconToMenuItem  (menuHandle_, ACTION_SUBDIVISION_REDUCE,   false, hiconreduce,      false, 0);
    AddIconToMenuItem  (menuHandle_, ACTION_SCULPT_NOW,           false, hiconsculpt,      false, 0);
    AddIconToMenuItem  (menuHandle_, ACTION_STOP,                 false, hiconstop,        false, 0);
    AddIconToMenuItem  (menuHandle_, ACTION_TILEABLE,             false, hicontileable,    false, 0);
    AddIconToMenuItem  (menuHandle_, ACTION_CROPEDIT,             false, hiconcrop,        false, 0);
    AddIconToMenuItem  (menuHandle_, ACTION_OPEN_LAST_DIRECTORY,  false, hiconfolder,      false, 0);
    AddIconToMenuItem  (menuHandle_, ACTION_OPEN_SAMPLE_DIRECTORY,false, hiconfolder,      false, 0);
    AddIconToMenuItem  (menuHandle_, ACTION_OPTIONS,              false, hiconwheels,      false, 0);

    HMENU menuquestion = GetSubMenu(menuHandle_, POSITION_MENU_QUESTION);
    AddIconToMenuItem  (menuquestion, POSITION_MENU_LABORATORY, true, hiconlaboratory,  false, 0);
  }
}

void GraphicUserInterface::createMainWindow(HINSTANCE hinstance, int isshow, Registry& registry)
{
  instanceHandle_ = hinstance; // Stocke le handle d'instance

  registerMainWindowClass(hinstance);
  
  DWORD style =  WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_THICKFRAME | WS_MAXIMIZEBOX;

  // limits of desktop client screen size
  int windowsclientareax = GetSystemMetrics(SM_CXFULLSCREEN);
  int windowsclientareay = GetSystemMetrics(SM_CYFULLSCREEN);

  int top, bottom, left, right ;
  bool isPosition = loadInterfacePosition(registry, top, bottom, left, right);

  if (!isPosition)
  {
    windowsHandle_ = CreateWindow(szWindowClass_, szTitle_, style,
                        CW_USEDEFAULT, 0, APPWINDOW_INITIAL_SIZE_X, APPWINDOW_INITIAL_SIZE_Y, 
                        NULL, NULL, hinstance, NULL);
        
    image_       = Image(APPWINDOW_INITIAL_SIZE_X,APPWINDOW_INITIAL_SIZE_Y,1,3);
    alpha_       = Image(APPWINDOW_INITIAL_SIZE_X,APPWINDOW_INITIAL_SIZE_Y,1,1);

    if (!windowsHandle_)
      THROW_INTERNAL_ERROR( "Windows could not create the main window" );
  }
  else
  {
    limit (left,   1, windowsclientareax - MIN_WINDOW_OVERLAP);
    limit (right,  1, windowsclientareax - MIN_WINDOW_OVERLAP);
    limit (top,    1, windowsclientareay - MIN_WINDOW_OVERLAP);
    limit (bottom, 1, windowsclientareay - MIN_WINDOW_OVERLAP);

    if (bottom < top + MIN_WINDOW_SIZE) 
      bottom = top + MIN_WINDOW_SIZE;
    if (right < left + MIN_WINDOW_SIZE) 
      right = left + MIN_WINDOW_SIZE;

    const int width  = right-left+1;
    const int height = bottom-top+1;
      
    image_       = Image(width,height,1,3);
    alpha_       = Image(width,height,1,1);

    windowsHandle_ = CreateWindow(szWindowClass_, szTitle_, style,
                          left, top, width, height, 
                          NULL, NULL, hinstance, NULL);

    if (!windowsHandle_)
        THROW_INTERNAL_ERROR( "Windows could not create the main window" );
  }

  menuHandle_ = GetMenu(windowsHandle_);

  createTransparentMenuIcons();

  if (!menuHandle_)
    THROW_INTERNAL_ERROR( "Windows could not create the main menu" );
  
  initButtonBarsIcons();
  contentZone_ .createWindow(windowsHandle_,             instanceHandle_);
  bottomZone_  .createWindow(windowsHandle_,             instanceHandle_);
  topTabBar_   .createWindow(windowsHandle_,             instanceHandle_);
  popup3d_     .createWindow(contentZone_.getWindowNr(), instanceHandle_);
  popupSave_   .createWindow(contentZone_.getWindowNr(), instanceHandle_);

  // hide the popups from view
  ShowWindow(popup3d_  .getWindowNr(), FALSE);
  ShowWindow(popupSave_.getWindowNr(), FALSE);

  isAeroGlass_ = setAeroGlass(windowsHandle_);

  contentZone_.setOutput(image_);
  frontPage_  .setOutput(image_);
  frontPage_.defineNewButton(ACTION_IMAGE_SOURCE_LEFT );
  frontPage_.defineNewButton(ACTION_IMAGE_SOURCE_RIGHT);
  frontPage_.defineNewButton(ACTION_IMAGE_CENTER      );
  frontPage_.defineNewButton(ACTION_IMAGE_SOURCE_ALPHA);

  bottomZone_    .setOutput(image_);
  bottomBar_     .setOutput(image_);
  bottomMiniBar_ .setOutput(image_);
  bottomSolo_    .setOutput(image_);
  popupSave_     .setOutput(image_);
  popup3d_       .setOutput(image_);
  popupLoadL_    .setOutput(image_);
  popupLoadR_    .setOutput(image_);
  popupLoadAlpha_.setOutput(image_);
  topTabBar_     .setOutput(image_);

  bottomBar_     .setPos(0, image_.dimy() - bottomBar_.getSizeY());
  bottomMiniBar_ .setPos(0, image_.dimy() - bottomMiniBar_.getSizeY());
  bottomSolo_    .setPos(0, image_.dimy() - bottomBar_.getSizeY());

  frame_.init(hinstance, "IMAGE_FRAME", 12);

  // from now on, launches events when files are dropped into the main window
  DragAcceptFiles(windowsHandle_, TRUE);

  UpdateWindow(windowsHandle_);

  toolTip_.init( windowsHandle_,  instanceHandle_);
}

const unsigned char GraphicUserInterface::textColor_ []            = { 100, 109, 128 };

LRESULT CALLBACK	WindowsProcedure(HWND, UINT, WPARAM, LPARAM);

//
//  FONCTION : MyRegisterClass()
//
//  BUT : inscrit la classe de fenêtre.
//
//  COMMENTAIRES :
//
//    Cette fonction et son utilisation sont nécessaires uniquement si vous souhaitez que ce code
//    soit compatible avec les systèmes Win32 avant la fonction 'RegisterClassEx'
//    qui a été ajoutée à Windows 95. Il est important d'appeler cette fonction
//    afin que l'application dispose des petites icônes correctes qui lui sont
//    associées.
//
ATOM GraphicUserInterface::registerMainWindowClass(HINSTANCE hInstance)
{
	DEBUG(MyRegi);
  WNDCLASSEX windowclass;

  ZeroMemory(&windowclass, sizeof(WNDCLASSEX));

	windowclass.cbSize        = sizeof(WNDCLASSEX);

	windowclass.style			    = CS_DBLCLKS;
  windowclass.lpfnWndProc	  = WindowsProcedure;
	windowclass.cbClsExtra    = 0;
	windowclass.cbWndExtra    = 0;
	windowclass.hInstance		  = hInstance;
	windowclass.hIcon			    = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PHOTOSCULPT));
 	windowclass.hCursor		    = LoadCursor(NULL, IDC_ARROW);
	windowclass.lpszMenuName	= MAKEINTRESOURCE(IDC_PHOTOSCULPT);

	// Initialise les chaînes globales
	LoadString(hInstance, IDS_APP_TITLE, szTitle_, MAX_LOADSTRING);
	LoadString(hInstance, IDC_PHOTOSCULPT, szWindowClass_, MAX_LOADSTRING);

	windowclass.lpszClassName = szWindowClass_;
	windowclass.hIconSm		    = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&windowclass);
}

void GraphicUserInterface::changeTitle(char * filename)
{
  LoadString(instanceHandle_, IDS_APP_TITLE, szTitle_, MAX_LOADSTRING);

  int length = strlen(filename) ;

  // is this string too big to fit into the szTitle_ buffer? Then don't display the filename
  // string is empty ? Then don't display the filename
  if (length + strlen(szTitle_) + 4 < MAX_PATH && length > 0 )
  {
    strcat(szTitle_, " - ");
    strcat(szTitle_, filename);
  }

  BOOL isok = SetWindowText(windowsHandle_, szTitle_);
  if (!isok)
    THROW_INTERNAL_ERROR("Could not change window title (in changeTitle())");
}

int GraphicUserInterface::getClientSizeX()
{
  RECT r;
  GetClientRect(windowsHandle_, &r);

  return r.right-r.left;
}

int GraphicUserInterface::getClientSizeY()
{
  RECT r;
  GetClientRect(windowsHandle_, &r);

  return  r.bottom-r.top;
}

int GraphicUserInterface::getContentSizeY()
{
  if (!isSculptBarMinimised_)
    return  getClientSizeY()-bottomBar_.getSizeY()-topTabBar_.getSizeY();
  else
    return  getClientSizeY()-bottomMiniBar_.getSizeY()-topTabBar_.getSizeY();
}

int GraphicUserInterface::getBottomBarSizeY()
{
  if (!isSculptBarMinimised_)
    return  bottomBar_.getSizeY();
  else
    return  bottomMiniBar_.getSizeY();
}

int GraphicUserInterface::getBottomBarPosY()
{
  if (!isSculptBarMinimised_)
    return  bottomBar_.getPositionY();
  else
    return  bottomMiniBar_.getPositionY();
}

void GraphicUserInterface::drawText(char* text, int positionx, int positiony)
{

  if (GlobalOptions.luminosity_ > SWITCH_MUTED_BRIGHT_COLOR)
    image_.draw_text(text, positionx, positiony, TEXT_MUTED_COLOR);
  else
    image_.draw_text(text, positionx, positiony, TEXT_BRIGHT_COLOR);

  alpha_.draw_text(text, positionx, positiony, TEXT_ALPHA);
}

void GraphicUserInterface::drawTextOnOpaqueWhite(char* text, int positionx, int positiony)
{
  image_.draw_text(text, positionx, positiony, TEXT_MUTED_COLOR);
}

