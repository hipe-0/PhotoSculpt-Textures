/*
 * DirectXControl.cpp
 *
 * Created on: 2010
 *      Author: Hippolyte Mounier, Copyright 2011 All rights reserved
 */
// 80 char per line standard :
/////////_/////////_/////////_/////////_/////////_/////////_/////////_/////////_

#include "DirectXControl.h"
#include "FloatMap.h"
#include "View3D.h"
#include "Win32Tools.h"
#include "Frame.h"
#include "File.h"
#include "Obfuscate.h"
#include "Mesh.h"
#include "Limit.h"
#include "ImageTools.h"
#include "Includes.h"

bool DirectXControl::testLoadDxDll()
{
  DEBUG(testLoa);
  HINSTANCE hDLL  = LoadLibrary("d3d9.dll");
  HINSTANCE hDLL2 = LoadLibrary("d3dx9_43.dll");

  if(hDLL != 0 && hDLL2 != 0)
  {
    FreeLibrary(hDLL);
    FreeLibrary(hDLL2);
    return true;
  }
  else 
    return false;
}


DirectXControl::DirectXControl()
{
  d3ddev_            = NULL;
  d3d_               = NULL;
  vertexBufferTable_ = NULL;
  indexBufferTable_  = NULL; 
  texture_           = NULL;

  cameraRX_          = cameraRY_ = cameraRZ_ = 0.0f; 
  cameraScale_       = 1.0f;

  isForceRedraw_     = true;
  triangleDisplayed_ = 0;
  maxtriangle_       = 400000; // max number of triangles displayed

  isSmooth_          = true;
  luminosity_        = 255 ;
}

void limit(FloatMap &d, int elementlimit)
{
  int sizex=d.getSizeX(), sizey=d.getSizeY();
  int nbvertex=sizex*sizey;

  if (nbvertex > elementlimit)
  { // a limit to the number of points displayed is needed to avoid crash

    float ratio = float(elementlimit) / float(nbvertex); // ratio is < 1

    sizex=int(round(float(sizex)*sqrtf(ratio)));
    sizey=int(round(float(sizey)*sqrtf(ratio)));

    d=d.getResizeAndMultiply(sizex,sizey);
  }
}

void DirectXControl::restrictModelSize_(PFloatMap model)
{
  D3DCAPS9 d3dCaps;
  d3ddev_->GetDeviceCaps(&d3dCaps);

  int trimemorylimit = d3dCaps.MaxVertexIndex /2  ; // 2 vertex per triangle
  int trimemorylimit2 = d3dCaps.MaxPrimitiveCount ; // 1 primitives per triangle

  if (trimemorylimit2<trimemorylimit)
    trimemorylimit = trimemorylimit2 ;

  if ( trimemorylimit > maxtriangle_ )
    trimemorylimit = maxtriangle_;

  int quadcountmax = trimemorylimit /2 ;

  limit(*model, quadcountmax);
}

const char * TEMP_FILE_NAME = "PS_tmp.jpg";

void DirectXControl::resizeTextureToHardWareCapabilities_(PImage texture, int div)
{
  // read max texture width and height
  D3DCAPS9 d3dCaps;
  d3ddev_->GetDeviceCaps(&d3dCaps);

  int TextureLimitH  = d3dCaps.MaxTextureHeight  ;
  int TextureLimitW  = d3dCaps.MaxTextureWidth   ;

  if (!texture->is_empty())
  {
    int texturesizex = texture->dimx();
    int texturesizey = texture->dimy();
    Frame::limit( texturesizex, texturesizey, TextureLimitW/div, TextureLimitH/div);

    if (texturesizex < texture->dimx())
      texture->resize(texturesizex, texturesizey);
  }
}




// this function initializes and prepares Direct3D for use
bool DirectXControl::initD3D(HWND hWnd, int sizex, int sizey, PFloatMap& inputmodel, 
                             PImage& inputtexture, PImage& alpha, 
                             int shader, bool isrepeat, float focallengthinmm)
{
  MUTEX;
  DEBUG(initD3);
  ALWAYS_EXECUTE_ON_WINDOWS_LOOP_THREAD();

  vertexBufferTable_ = NULL;
  indexBufferTable_ = NULL;
  texture_  = NULL;
  d3ddev_   = NULL;
  d3d_      = NULL;
  hwnd_     = hWnd;

  d3d_ = Direct3DCreate9(D3D_SDK_VERSION);

  if (d3d_ == NULL )
  {
    MUTEXRELEASE;
    return false;
  }

  D3DPRESENT_PARAMETERS d3dpp;

  ZeroMemory(&d3dpp, sizeof(d3dpp));
  d3dpp.Windowed = TRUE;
  d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
  d3dpp.hDeviceWindow = hWnd;
  d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
  d3dpp.BackBufferWidth =  sizex;
  d3dpp.BackBufferHeight = sizey;
  d3dpp.EnableAutoDepthStencil = TRUE;
  d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

  // create a device class using this information and the info from the d3dpp stuct
  HRESULT res = d3d_->CreateDevice(D3DADAPTER_DEFAULT,
                    D3DDEVTYPE_HAL,
                    hWnd,
                    D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                    &d3dpp,
                    &d3ddev_);

  if (FAILED(res))
  {
    if (d3ddev_ != NULL)
    {
      d3ddev_ -> Release();
      d3ddev_ = NULL ;
    }

    d3d_ -> Release();
    d3d_ = NULL;

    MUTEXRELEASE;
    return false;
  }


  int framesizex = (inputtexture->dimx()) * REPEATED_SEAM_PERCENT / 100 ;
  int framesizey = (inputtexture->dimy()) * REPEATED_SEAM_PERCENT / 100 ;


  if (!alpha->is_empty())
    alpha->resize(inputtexture->dimx() , inputtexture->dimy());

  if (isrepeat)
  {
    FloatMap::repeatN(*inputmodel,  1, framesizex, framesizey);

    if (!alpha->is_empty())
      ImageTools::repeatN(*alpha, 1, framesizex, framesizey);
  }

  restrictModelSize_(inputmodel);

  modelSizeX_ = inputmodel->getSizeX() - 1 ;
  modelSizeY_ = inputmodel->getSizeY() - 1 ;

  Mesh modelmesh;
  modelmesh.create(*inputmodel, focallengthinmm);

  inputmodel = new FloatMap; // clear memory

  initVertex_ (modelmesh, *alpha);

  inputmodel = new FloatMap; // clear memory

  initIndex_  ();

  d3ddev_->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);                //Enable alpha blending
  d3ddev_->SetRenderState(D3DRS_LIGHTING, TRUE);                        // turn on the 3D lighting
  d3ddev_->SetRenderState(D3DRS_ZENABLE, TRUE);                         // turn on the z-buffer
  d3ddev_->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(50, 50, 50));    // ambient light

  d3ddev_->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, 8);                // anisotropic level
  d3ddev_->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);  // minification
  d3ddev_->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);       // magnification
  d3ddev_->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);       // mipmap

  DXSizeX_=sizex;
  DXSizeY_=sizey;
  isForceRedraw_ = true;

  MUTEXRELEASE;

  changeShaderOnly(inputtexture, shader, isrepeat);
  return true;
}

void DirectXControl::changeShaderOnly(PImage& inputtexture, int shader, bool isrepeat)
{
  MUTEX;
  DEBUG(changeShad);
  ALWAYS_EXECUTE_ON_WINDOWS_LOOP_THREAD();

  int div = 1;
  if (isrepeat) div = 3;

  if (shader == ACTION_3D_TEXTURE)
    resizeTextureToHardWareCapabilities_(inputtexture, div);

  float factor=1.0f;
  if (shader == ACTION_3D_WHITE || shader == ACTION_3D_WIREFRAME)
  {
    inputtexture = new Image; // clear memory
    // if no texture then make light lower, as the white 3d model will be too bright
    factor=0.8f;
  }

  initLight  (factor);       // call the function to initialize the light and material

  int framesizex = (inputtexture->dimx()) * REPEATED_SEAM_PERCENT / 100 ;
  int framesizey = (inputtexture->dimy()) * REPEATED_SEAM_PERCENT / 100 ;

  if (isrepeat)
    //ImageTools::repeatN(*inputtexture, 3,0,0); 
    ImageTools::repeatN(*inputtexture, 1, framesizex, framesizey);


  if (shader == ACTION_3D_TEXTURE && texture_ == NULL)
    loadTexture_(*inputtexture);
  else if (shader != ACTION_3D_TEXTURE && texture_ != NULL)
  {
    texture_->Release();
    texture_ = NULL;
  }

  if (shader == ACTION_3D_WIREFRAME)
    d3ddev_->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
  else
    d3ddev_->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
    
  d3ddev_->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);                // normalises normals

  if (shader != ACTION_3D_WIREFRAME)
    d3ddev_->SetRenderState(D3DRS_SPECULARENABLE,   TRUE);
    
  forceDrawOnNextRender(); // needed to update the model on screen
  MUTEXRELEASE;
}

bool DirectXControl::isRedrawNeeded() const
{
  ALWAYS_EXECUTE_ON_WINDOWS_LOOP_THREAD();
  static float oldcameraRX    = cameraRX_    ; 
  static float oldcameraRY    = cameraRY_    ; 
  static float oldcameraRZ    = cameraRZ_    ; 
  static float oldcameraTX    = cameraTX_    ; 
  static float oldcameraTY    = cameraTY_    ; 
  static float oldcameraTZ    = cameraTZ_    ; 
  static float oldcameraScale = cameraScale_ ;
  static int   oldDXSizeX     = DXSizeX_     ;
  static int   oldDXSizeY     = DXSizeY_     ;

  bool needed =  oldcameraRX    != cameraRX_    || 
                 oldcameraRY    != cameraRY_    ||
                 oldcameraRZ    != cameraRZ_    ||
                 oldcameraTX    != cameraTX_    ||
                 oldcameraTY    != cameraTY_    ||
                 oldcameraTZ    != cameraTZ_    ||
                 oldcameraScale != cameraScale_ ||
                 oldDXSizeX     != DXSizeX_     ||
                 oldDXSizeY     != DXSizeY_     ||
                 isForceRedraw_  ;

  // remember for next call
  oldcameraRX    = cameraRX_    ; oldcameraRY    = cameraRY_ ;   oldcameraRZ    = cameraRZ_    ; 
  oldcameraTX    = cameraTX_    ; oldcameraTY    = cameraTY_ ;   oldcameraTZ    = cameraTZ_    ; 
  oldcameraScale = cameraScale_ ;
  oldDXSizeX     = DXSizeX_     ; oldDXSizeY     = DXSizeY_  ;

  return needed;
}

void DirectXControl::setCamera(float camerarx, float camerary, float camerarz, 
                               float cameratx, float cameraty, float cameratz, 
                               float camerascale)
{
  cameraRX_      = camerarx;  cameraRY_      = camerary;  cameraRZ_      = camerarz;
  cameraTX_      = cameratx;  cameraTY_      = cameraty;  cameraTZ_      = cameratz;
  cameraScale_   = camerascale;
}

void DirectXControl::forceDrawOnNextRender()
{
  isForceRedraw_ = true;
}

bool DirectXControl::isOn()
{
  return (d3ddev_ != NULL);
}

void getVerticesIndexesSquare(Vector3 position, float size,
                         int& returnnbvertex, CUSTOMVERTEX*& returnvertexes,
                         int& returnnbindex,  short*&        returnindexes )
{
  int nbvertexes = 4 ; // 4 vertexes per square

  CUSTOMVERTEX* vertexes = new CUSTOMVERTEX [nbvertexes];
  
  if (vertexes == NULL) return;

  vertexes[0].X        = position.x + size/2.0f;
  vertexes[0].Y        = position.y + size/2.0f;
  vertexes[0].Z        = position.z            ;
  vertexes[0].NORMAL.x = 0.0f;
  vertexes[0].NORMAL.y = 0.0f;
  vertexes[0].NORMAL.z = 1.0f;

  vertexes[1].X        = position.x - size/2.0f;
  vertexes[1].Y        = position.y + size/2.0f;
  vertexes[1].Z        = position.z            ;
  vertexes[1].NORMAL.x = 0.0f;
  vertexes[1].NORMAL.y = 0.0f;
  vertexes[1].NORMAL.z = 1.0f;

  vertexes[2].X        = position.x + size/2.0f;
  vertexes[2].Y        = position.y - size/2.0f;
  vertexes[2].Z        = position.z            ;
  vertexes[2].NORMAL.x = 0.0f;
  vertexes[2].NORMAL.y = 0.0f;
  vertexes[2].NORMAL.z = 1.0f;

  vertexes[3].X        = position.x - size/2.0f;
  vertexes[3].Y        = position.y - size/2.0f;
  vertexes[3].Z        = position.z            ;
  vertexes[3].NORMAL.x = 0.0f;
  vertexes[3].NORMAL.y = 0.0f;
  vertexes[3].NORMAL.z = 1.0f;

  int nbindexes  = 6 ; // 6 indices per square
  short * indexes = new short [nbindexes];
  
  if (indexes == NULL) 
  {
    delete vertexes;
    return;
  }

  // first triangle
  indexes[0] = 0;
  indexes[1] = 1;
  indexes[2] = 2;

  // second triangle
  indexes[3] = 1;
  indexes[4] = 3;
  indexes[5] = 2;

  returnnbindex  = nbindexes  ;
  returnvertexes = vertexes   ;
  returnnbvertex = nbvertexes ;
  returnindexes  = indexes    ;
}


// this is the function used to render a single frame
void DirectXControl::render3DImageFrame(void)
{
  ALWAYS_EXECUTE_ON_WINDOWS_LOOP_THREAD();
  MUTEX;
  NANOBEGIN;

  if ( isOn() ) 
  {
    bool redraw = isRedrawNeeded();

    if (redraw)
    {
      //DEBUG(render3DIm);

      // cancels the flag that says to force redraw, until next time it is forced again
      isForceRedraw_  = false;

      d3ddev_->Clear(0, NULL, D3DCLEAR_TARGET,
                     D3DCOLOR_ARGB(255, luminosity_, luminosity_, luminosity_), 1.0f, 0);
      // was:
      // d3ddev_->Clear(0, NULL, D3DCLEAR_TARGET,  
      //               D3DCOLOR_XRGB(luminosity_, luminosity_, luminosity_), 1.0f, 0);
      d3ddev_->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0),   1.0f, 0);

      d3ddev_->BeginScene();

      // select which vertex format we are using
      d3ddev_->SetFVF(CUSTOMFVF);

      // set the view transform
      D3DXMATRIX matView;
      
      float DISTANCE = 15.0f;

      D3DXVECTOR3 cameraPosition(cameraTX_, cameraTY_, DISTANCE + cameraTZ_);
      D3DXVECTOR3 lookAtPosition(cameraTX_, cameraTY_, cameraTZ_);
      D3DXVECTOR3 upPosition    (0.0f, 1.0f, 0.0f );

      D3DXMatrixLookAtLH(&matView, &cameraPosition, &lookAtPosition, &upPosition);
      d3ddev_->SetTransform(D3DTS_VIEW, &matView);

      // set the projection transform
      D3DXMATRIX matProjection;
      D3DXMatrixPerspectiveFovLH(&matProjection,
                                 D3DXToRadian(45),
                                 (FLOAT) DXSizeX_ / (FLOAT) DXSizeY_,
                                 1.0f,       // the near view-plane
                                 100.0f);    // the far view-plane
      d3ddev_->SetTransform(D3DTS_PROJECTION, &matProjection);

      D3DXMATRIX matRotateX, matRotateY , matRotateZ, matRotateZ180 , matcameraScale;
      const float PI = 3.141592f;

      D3DXMatrixRotationY(&matRotateY, cameraRX_);
      D3DXMatrixRotationX(&matRotateX, cameraRY_);
      D3DXMatrixRotationZ(&matRotateZ, cameraRZ_);
      D3DXMatrixRotationZ(&matRotateZ180, PI);
      D3DXMatrixScaling  (&matcameraScale,   cameraScale_, cameraScale_, cameraScale_);

      D3DXMATRIX matRotate =   matRotateZ180 * matRotateZ * matRotateY * matRotateX * matcameraScale;

      d3ddev_->SetTransform(D3DTS_WORLD, &(matRotate));

      for (int y=0; y < modelSizeY_; y++)
      {
        // select the vertex and index buffers to use
        int offsetInBytes = sizeof(CUSTOMVERTEX) * y * modelSizeX_ * 4; // 4 vertex per square
        d3ddev_->SetStreamSource(0, vertexBufferTable_, offsetInBytes, sizeof(CUSTOMVERTEX));
        d3ddev_->SetIndices(indexBufferTable_);

        // set the texture
        d3ddev_->SetTexture(0, texture_);

        // draw the 3D object
        int linevertexcount   = modelSizeX_ * 4;
        int linetrianglecount = modelSizeX_ * 2;
        d3ddev_->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, linevertexcount, 0, linetrianglecount); 
      }

      /*{
          int nbvertex, nbindex  ;
          CUSTOMVERTEX* vertexes ;
          short*        indexes  ;
          LPDIRECT3DVERTEXBUFFER9 vertexbuffer;

          getVerticesIndexesSquare(Vector3(0,0,0), 1.0f, nbvertex, vertexes, nbindex, indexes);

          d3ddev_->CreateVertexBuffer(sizeof(CUSTOMVERTEX) * nbvertex,
                                   0, CUSTOMFVF, D3DPOOL_MANAGED, &vertexbuffer,
                                   NULL);
          // lock vertexBufferTable_ and load the vertices into it
          VOID* pVoid;    // a void pointer
          vertexBufferTable_->Lock(0, 0, (void**)&pVoid, 0);
          memcpy(pVoid, vertexes, sizeof(CUSTOMVERTEX) * nbvertex);
          vertexBufferTable_->Unlock();
          delete[] vertexes;

          LPDIRECT3DINDEXBUFFER9 indexbuffer;
          d3ddev_->CreateIndexBuffer(sizeof(short) * nbindex,
                                  0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &indexbuffer,
                                  NULL);
       

        int offsetInBytes = sizeof(CUSTOMVERTEX) * 0 * 4; // 4 vertex per square
        d3ddev_->SetStreamSource(0, vertexbuffer, offsetInBytes, sizeof(CUSTOMVERTEX));
        d3ddev_->SetIndices(indexbuffer);

        // set the texture
        d3ddev_->SetTexture(0, texture_);

        // draw the 3D object
        int linevertexcount   = 4;
        int linetrianglecount = 2;
        d3ddev_->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, linevertexcount, 0, linetrianglecount); 
      }*/

      d3ddev_->EndScene(); 
    }

    d3ddev_->Present(NULL, NULL, NULL, NULL);

    triangleDisplayed_ = modelSizeX_ * modelSizeY_ * 2 ;
  }
  NANOEND;
  MUTEXRELEASE;
}

int  DirectXControl::getTriangleDisplayed()
{
  return triangleDisplayed_;
}


// this is the function that cleans up Direct3D and COM
void DirectXControl::cleanD3D(void)
{
  DEBUG(cleanD3);
  NANOBEGIN;
  MUTEX;
  //ALWAYS_EXECUTE_ON_WINDOWS_LOOP_THREAD();

  if (vertexBufferTable_ != NULL)
    vertexBufferTable_->Release();

  if (indexBufferTable_ != NULL)
    indexBufferTable_->Release();

  if (texture_ != NULL)
    texture_->Release();    // close and release the texture

  if (d3ddev_ != NULL) 
    d3ddev_->Release();

  if (d3d_ != NULL) 
    d3d_->Release();

  vertexBufferTable_ = NULL;
  indexBufferTable_  = NULL;
  d3ddev_            = NULL;
  d3d_               = NULL;
  texture_           = NULL;
  triangleDisplayed_ = 0;

  MUTEXRELEASE;
  NANOEND;
}

void DirectXControl::setMaxTriangle(int i)
{
  maxtriangle_ = i ;
}

void DirectXControl::setSmooth(bool value)
{
  isSmooth_ = value;
}

bool DirectXControl::getSmooth()
{
  return isSmooth_;
}

void DirectXControl::setLuminosity  (int luminosity)
{
  luminosity_ = luminosity;
}
