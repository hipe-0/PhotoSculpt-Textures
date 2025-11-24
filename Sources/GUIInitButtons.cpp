#include "PSApplication.h"
#include "Win32Tools.h"


void GraphicUserInterface::initButtonBarsIcons()
{
  DEBUG(initBut);
  initSculptIcons      ();
  initSculptMiniIcons  ();
  initSculptSolo       ();
  initPopupSave        ();
  initPopup3d          ();
  initPopupLoadLeft    ();
  initPopupLoadRight   ();
  initPopupLoadAlpha   ();
  initTab              ();

  leftPhotoicon_    = Win32Tools::loadImageFromResource(instanceHandle_, "IMAGE_LEFT_PHOTO");
  rightPhotoicon_   = Win32Tools::loadImageFromResource(instanceHandle_, "IMAGE_RIGHT_PHOTO");
}

void GraphicUserInterface::initSculptIcons()
{
  bottomBar_.clearAllButtons();
  Image normal       = Win32Tools::loadImageFromResource(instanceHandle_, "IMAGE_SCULPT_ICONS");
  Image hide_out     = Win32Tools::loadImageFromResource(instanceHandle_, "IMAGE_SCULPT_ICONS_G");
  Image highlights   = Win32Tools::loadImageFromResource(instanceHandle_, "IMAGE_SCULPT_ICONS_H");
  Image pressed_down = Win32Tools::loadImageFromResource(instanceHandle_, "IMAGE_SCULPT_ICONS_S");

  bottomBar_.storeBackground(hide_out);

  bottomBar_.DEFINE2(ERASE_RESULT,         102,22, 135,68,  0,35);
  bottomBar_.DEFINE2(SUBDIVISION_REDUCE,   136,22, 180,68,  5,35);
  bottomBar_.DEFINE2(STOP,                 181,22, 218,68,  7,35);
  bottomBar_.DEFINE2(SUBDIVISION_INCREASE, 219,22, 262,68,  5,35);
  bottomBar_.DEFINE2(SCULPT_NOW,           263,22, 308,68,  5,35);
  bottomBar_.DEFINE2(MODEL_SIZE_BAR,        20, 1, 393,19,  3,5 );
  bottomBar_.DEFINE2(TIME_LEFT_BAR,        315,30, 391,53,  3,2 );
  bottomBar_.DEFINE (MINIMIZE_SCULPT_BAR,    0, 0,  19,19       );

  bottomBar_.loadAllImages         (normal);
  bottomBar_.loadAllImagesHoover   (highlights);
  bottomBar_.loadAllImagesInactive (hide_out);
  bottomBar_.loadAllImagesSelected (pressed_down);
}

void GraphicUserInterface::initSculptMiniIcons()
{
  bottomMiniBar_.clearAllButtons();
  Image normal       = Win32Tools::loadImageFromResource(instanceHandle_, "IMAGE_SCULPT_MINI");
  Image hide_out     = Win32Tools::loadImageFromResource(instanceHandle_, "IMAGE_SCULPT_MINI_G");
  Image highlights   = Win32Tools::loadImageFromResource(instanceHandle_, "IMAGE_SCULPT_MINI_H");
  Image pressed_down = Win32Tools::loadImageFromResource(instanceHandle_, "IMAGE_SCULPT_MINI_S");

  bottomMiniBar_.storeBackground(hide_out);

  bottomMiniBar_.DEFINE (ERASE_RESULT,         293,0,    309,18);
  bottomMiniBar_.DEFINE (SUBDIVISION_REDUCE,   310,0,    330,18);
  bottomMiniBar_.DEFINE (STOP,                 331,0,    349,18);
  bottomMiniBar_.DEFINE (SUBDIVISION_INCREASE, 350,0,    369,18);
  bottomMiniBar_.DEFINE (SCULPT_NOW,           370,0,    391,18);
  bottomMiniBar_.DEFINE (MINIMIZE_SCULPT_BAR,    0,0,     20,18);
  bottomMiniBar_.DEFINE2(MODEL_SIZE_BAR,        21,1,    293,19, 3,5);

  bottomMiniBar_.loadAllImages     (normal);
  bottomMiniBar_.loadAllImagesHoover(highlights);
  bottomMiniBar_.loadAllImagesInactive (hide_out);
  bottomMiniBar_.loadAllImagesSelected  (pressed_down);
}

void GraphicUserInterface::initPopup3d ()
{
  popup3d_.clearAllButtons();
  Image normal       = Win32Tools::loadImageFromResource(instanceHandle_, "IMAGE_3D");
  Image hide_out     = Win32Tools::loadImageFromResource(instanceHandle_, "IMAGE_3D_G");
  Image highlights   = Win32Tools::loadImageFromResource(instanceHandle_, "IMAGE_3D_H");
  Image pressed_down = Win32Tools::loadImageFromResource(instanceHandle_, "IMAGE_3D_S");
  Image on           = Win32Tools::loadImageFromResource(instanceHandle_, "IMAGE_3D_A");

  popup3d_.storeBackground(hide_out);

  popup3d_.DEFINE(3D_DEPTH_MINUS, 4, 3,    32,33  );
  popup3d_.DEFINE(3D_DEPTH_PLUS,  33,3,    57,33  );
  popup3d_.DEFINE(3D_WIREFRAME,   58,3,    84,33  );
  popup3d_.DEFINE(3D_WHITE,       85,3,    108,33 );
  popup3d_.DEFINE(3D_TEXTURE,     109,3,   133,33 );
  popup3d_.DEFINE(FLAT_MAP,       134,3,   166,33 );
  popup3d_.DEFINE(TILEABLE,       167,3,   202,33 );
  popup3d_.DEFINE(CROPEDIT,       203,3,   233,33 );

  popup3d_.loadAllImages         (normal);
  popup3d_.loadAllImagesHoover   (highlights);
  popup3d_.loadAllImagesInactive (hide_out);
  popup3d_.loadAllImagesSelected (pressed_down);
  popup3d_.loadAllImagesClicked  (on);
  popup3d_.loadAllImagesClickedAndSelected (highlights);
}

void GraphicUserInterface::initPopupLoadLeft ()
{
  DEBUG(initPop);
  popupLoadL_.clearAllButtons();
  Image normal       = Win32Tools::loadImageFromResource(instanceHandle_, "IMAGE_LOAD_FRONT"  );
  Image hide_out     = Win32Tools::loadImageFromResource(instanceHandle_, "IMAGE_LOAD_FRONT_G");
  Image highlights   = Win32Tools::loadImageFromResource(instanceHandle_, "IMAGE_LOAD_FRONT_H");
  Image pressed_down = Win32Tools::loadImageFromResource(instanceHandle_, "IMAGE_LOAD_FRONT_S");

  popupLoadL_.storeBackground(hide_out);

  popupLoadL_.defineNewButton(ACTION_OPEN_LEFT_IMAGE,           4,3,     36,33,  "Open a new photo from file" );
  popupLoadL_.defineNewButton(ACTION_LEFT_IMAGE_ROTATE_LEFT,    37,3,    67,33,  "Rotate this photo to left"  );
  popupLoadL_.defineNewButton(ACTION_LEFT_IMAGE_ROTATE_RIGHT,   68,3,    96,33,  "Rotate this photo to right" );
  popupLoadL_.defineNewButton(ACTION_LEFT_RIGHT_SWAP,           97,3,    127,33, "Swap 2 photos (F becomes R)");

  popupLoadL_.defineNewButton(ACTION_ADD_ALPHA,                 128,3,   155,33, "Create alpha selection"     );
  popupLoadL_.defineNewButton(ACTION_DELETE_LEFT,               156,3,   186,33, "Close this photo"           );

  popupLoadL_.loadAllImages         (normal);
  popupLoadL_.loadAllImagesHoover   (highlights);

  popupLoadL_.loadAllImagesInactive (hide_out);
  popupLoadL_.loadAllImagesSelected (pressed_down);

  popupLoadL_.hide();
}

void GraphicUserInterface::initPopupLoadRight ()
{
  popupLoadR_.clearAllButtons();
  Image normal       = Win32Tools::loadImageFromResource(instanceHandle_, "IMAGE_LOAD_3D");
  Image hide_out     = Win32Tools::loadImageFromResource(instanceHandle_, "IMAGE_LOAD_3D_G");
  Image highlights   = Win32Tools::loadImageFromResource(instanceHandle_, "IMAGE_LOAD_3D_H");
  Image pressed_down = Win32Tools::loadImageFromResource(instanceHandle_, "IMAGE_LOAD_3D_S");

  popupLoadR_.storeBackground(hide_out);

  popupLoadR_.defineNewButton(ACTION_OPEN_RIGHT_IMAGE,           4,3,     36,33,  "Open a new photo from file");
  popupLoadR_.defineNewButton(ACTION_RIGHT_IMAGE_ROTATE_LEFT,    37,3,    67,33,  "Rotate this photo to left");
  popupLoadR_.defineNewButton(ACTION_RIGHT_IMAGE_ROTATE_RIGHT,   68,3,    96,33,  "Rotate this photo to right");
  popupLoadR_.defineNewButton(ACTION_LEFT_RIGHT_SWAP,            97,3,    126,33, "Swap 2 photos (L becomes R)");
  popupLoadR_.defineNewButton(ACTION_DELETE_RIGHT,               127,3,   154,33, "Close this photo");

  popupLoadR_.loadAllImages        (normal);
  popupLoadR_.loadAllImagesHoover  (highlights);

  popupLoadR_.loadAllImagesInactive(hide_out);
  popupLoadR_.loadAllImagesSelected(pressed_down);

  popupLoadR_.hide();
}

void GraphicUserInterface::initPopupLoadAlpha ()
{
  popupLoadAlpha_.clearAllButtons();
  Image normal       = Win32Tools::loadImageFromResource(instanceHandle_, "IMAGE_LOAD_ALPHA"  );
  Image hide_out     = Win32Tools::loadImageFromResource(instanceHandle_, "IMAGE_LOAD_ALPHA_G");
  Image highlights   = Win32Tools::loadImageFromResource(instanceHandle_, "IMAGE_LOAD_ALPHA_H");
  Image pressed_down = Win32Tools::loadImageFromResource(instanceHandle_, "IMAGE_LOAD_ALPHA_S");

  popupLoadAlpha_.storeBackground(hide_out);

  popupLoadAlpha_.defineNewButton(ACTION_LOAD_ALPHA,           4,3,     36,33,  "Open a new alpha from file");
  popupLoadAlpha_.defineNewButton(ACTION_ALPHA_ROTATE_LEFT,    63,3,    92,33,  "Rotate this alpha to left" );
  popupLoadAlpha_.defineNewButton(ACTION_ALPHA_ROTATE_RIGHT,   93,3,    120,33, "Rotate this alpha to right");
  popupLoadAlpha_.defineNewButton(ACTION_EDIT_SELECTION,       122,3,   154,33, "Edit this alpha selection" );
  popupLoadAlpha_.defineNewButton(ACTION_CLOSE_ALPHA,          185,3,   212,33, "Close this alpha"          );

  popupLoadAlpha_.loadAllImages         (normal);
  popupLoadAlpha_.loadAllImagesHoover   (highlights);
  popupLoadAlpha_.loadAllImagesInactive (hide_out);
  popupLoadAlpha_.loadAllImagesSelected (pressed_down);

  popupLoadAlpha_.hide();
}

void GraphicUserInterface::initPopupSave ()
{
  popupSave_.clearAllButtons();
  Image normal       = Win32Tools::loadImageFromResource(instanceHandle_, "IMAGE_SAVE_3D");
  Image hide_out     = Win32Tools::loadImageFromResource(instanceHandle_, "IMAGE_SAVE_3D_G");
  Image highlights   = Win32Tools::loadImageFromResource(instanceHandle_, "IMAGE_SAVE_3D_H");
  Image pressed_down = Win32Tools::loadImageFromResource(instanceHandle_, "IMAGE_SAVE_3D_S");

  popupSave_.storeBackground(hide_out);

  popupSave_.defineNewButton(ACTION_LOAD_SCULPT_FILE,  2,1,    32,35,  "Load sculpt file");
  popupSave_.defineNewButton(ACTION_SAVE_AS,           33,1,    61,35, "Save as");
  popupSave_.defineNewButton(ACTION_SAVE_MULTIPLE,     62,1,    96,35, "Save multiple");
  popupSave_.defineNewButton(ACTION_COPY_TO_CLIPBOARD, 97,1,   126,35, "Copy image to clipboard");
  popupSave_.defineNewButton(ACTION_ERASE_RESULT,      127,1,  154,35, "Close 3D model");

  popupSave_.loadAllImages         (normal);
  popupSave_.loadAllImagesHoover   (highlights);
  popupSave_.loadAllImagesInactive (hide_out);
  popupSave_.loadAllImagesSelected (pressed_down);

  popupSave_.hide();
}

void GraphicUserInterface::initSculptSolo()
{
  bottomSolo_.clearAllButtons();
  Image normal       = Win32Tools::loadImageFromResource(instanceHandle_, "IMAGE_SCULPT_SOLO");
  Image hide_out     = Win32Tools::loadImageFromResource(instanceHandle_, "IMAGE_SCULPT_SOLO_G");
  Image highlights   = Win32Tools::loadImageFromResource(instanceHandle_, "IMAGE_SCULPT_SOLO_H");
  Image pressed_down = Win32Tools::loadImageFromResource(instanceHandle_, "IMAGE_SCULPT_SOLO_S");

  bottomSolo_.storeBackground(hide_out);

  bottomSolo_.DEFINE2(SCULPT_NOW, 179,13,    225,59, 7,33);

  bottomSolo_.loadAllImages         (normal);
  bottomSolo_.loadAllImagesHoover   (highlights);
  bottomSolo_.loadAllImagesInactive (hide_out);
  bottomSolo_.loadAllImagesSelected (pressed_down);
}

void GraphicUserInterface::initTab()
{
  topTabBar_.clearAllButtons();
  Image normal       = Win32Tools::loadImageFromResource(instanceHandle_, "IMAGE_TAB");
  Image hide_out     = Win32Tools::loadImageFromResource(instanceHandle_, "IMAGE_TAB_G");
  Image highlights   = Win32Tools::loadImageFromResource(instanceHandle_, "IMAGE_TAB_H");
  Image pressed_down = Win32Tools::loadImageFromResource(instanceHandle_, "IMAGE_TAB_S");
  Image turn_on      = Win32Tools::loadImageFromResource(instanceHandle_, "IMAGE_TAB_A");

  topTabBar_.storeBackground(hide_out);

  topTabBar_.DEFINE2(MODE_SOURCES,  3, 0,      98,24,  10,10 );
  topTabBar_.DEFINE2(MODE_3D,       99,0,     161,24,  7,10  );
  topTabBar_.DEFINE2(MODE_DISPLACE, 162,0,    218,24,  6,10  );
  topTabBar_.DEFINE2(MODE_NORMAL,   219,0,    273,24,  8,10  );
  topTabBar_.DEFINE2(MODE_AMBOCC,   274,0,    341,24,  8,10  );
  topTabBar_.DEFINE2(MODE_SPECULAR, 342,0,    403,24,  7,10  );
  topTabBar_.DEFINE2(MODE_DIFFUSE,  404,0,    463,24,  8,10  );
  topTabBar_.DEFINE2(MODE_BUMP,     464,0,    511,24,  10,10 );
  topTabBar_.DEFINE2(MODE_LIMDISP,  512,0,    561,24,  2,10  );
  topTabBar_.DEFINE2(MODE_ALPHA,    562,0,    607,24,  7,10  );

  topTabBar_.loadAllImages                    (normal);
  topTabBar_.loadAllImagesHoover              (highlights);
  topTabBar_.loadAllImagesInactive            (hide_out);
  topTabBar_.loadAllImagesSelected            (pressed_down);
  topTabBar_.loadAllImagesClicked             (turn_on);
  topTabBar_.loadAllImagesClickedAndSelected  (highlights);
}
