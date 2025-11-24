/*
 * DirectXControlInitScene.cpp
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
#include "Includes.h"
#include "Frame.h"
#include "File.h"
#include "Obfuscate.h"
#include "Mesh.h"
#include "Limit.h"

// determines if an individual vertex is shown or hidden by the alpha mask (value <=128)
bool testAlpha(const Image& alphamap, const Mesh &model, int x, int y)
{
  bool show = true;
  
  if (!alphamap.is_empty())
  {
    show = false;
    int sizex = model.getSizeX(), sizey = model.getSizeY();
  
    if (x>=0 && y>=0 && x+1<sizex && y+1<sizey)
    {
      float sx = float(alphamap.dimx()) - 1.0f, sy = float(alphamap.dimy()) - 1.0f;
      int textx, texty;

      textx = round (model.getVertexU (x,y)     * sx );
      texty = round (model.getVertexV (x,y)     * sy );
      if (alphamap(textx,texty, 0, 0) > 128) 
        show = true;

      textx = round (model.getVertexU (x+1,y)   * sx );
      texty = round (model.getVertexV (x+1,y)   * sy );
      if (alphamap(textx,texty, 0, 0) > 128) 
        show = true;

      textx = round (model.getVertexU (x,y+1)   * sx );
      texty = round (model.getVertexV (x,y+1)   * sy );
      if (alphamap(textx,texty, 0, 0) > 128) 
        show = true;

      textx = round (model.getVertexU (x+1,y+1) * sx );
      texty = round (model.getVertexV (x+1,y+1) * sy );
      if (alphamap(textx,texty, 0, 0) > 128) 
        show = true;
    }
  }

  return show;
}

// Convert a given Map into a Vertex map
//
CUSTOMVERTEX* DirectXControl::getnewVerticesFromMap(const Mesh &model, int& returnsize, bool issmooth, const Image& alpha)
{
  if ( model.isEmpty() )
    return NULL;

  int sizex = model.getSizeX(), sizey = model.getSizeY();
  int size  = (sizex-1) * (sizey-1) * 4 ; // 4 vertexes per square

  CUSTOMVERTEX* p = new CUSTOMVERTEX [size];
  
  if (p == NULL) 
    return NULL;

  Vector3 center (0.5f, 0.5f * float(sizey-1) / float(sizex-1), model.getVertexZ(sizex/2,sizey/2) );

  CUSTOMVERTEX* ptr = p;
  for (int y=0; y<sizey-1 ; y++)
    for (int x=0; x<sizex-1 ; x++)
    {
      float const yratio = float(sizex-1) / float(sizey-1);

      //bool show = true; // <- unquote for debuging
      bool show = testAlpha(alpha, model, x,y);
      // 0 = upper left ; 1 = upper right ; 2 = bottom left ; 3 = bottom right

      if (show)
      {
        Vector3 pos0(model.getVertexX (x,  y  ), model.getVertexY (x,  y  ) , model.getVertexZ (x,  y  ));
        Vector3 pos1(model.getVertexX (x+1,y  ), model.getVertexY (x+1,y  ) , model.getVertexZ (x+1,y  ));
        Vector3 pos2(model.getVertexX (x,  y+1), model.getVertexY (x,  y+1) , model.getVertexZ (x,  y+1));
        Vector3 pos3(model.getVertexX (x+1,y+1), model.getVertexY (x+1,y+1) , model.getVertexZ (x+1,y+1));

        Vector3 posm0 = (pos0 - center ) * 10.0f;
        Vector3 posm1 = (pos1 - center ) * 10.0f;
        Vector3 posm2 = (pos2 - center ) * 10.0f;
        Vector3 posm3 = (pos3 - center ) * 10.0f;

        setCustomVertex (ptr[0], posm0.x , posm0.y , posm0.z );
        setCustomVertex (ptr[1], posm1.x , posm1.y , posm1.z );
        setCustomVertex (ptr[2], posm2.x , posm2.y , posm2.z );
        setCustomVertex (ptr[3], posm3.x , posm3.y , posm3.z );
        
        if (!issmooth)
        {
          // one normal direction only for all 4 corners of the quad
          Vector3 norm = normalize( (pos1 - pos0) ^ (pos2 - pos0) );

          setNormal(ptr[0], convert( norm ) );
          setNormal(ptr[1], convert( norm ) );
          setNormal(ptr[2], convert( norm ) );
          setNormal(ptr[3], convert( norm ) );
        }
        else
        {
          setNormal(ptr[0], convert( model.getNorm(x   , y  ) ) );
          setNormal(ptr[1], convert( model.getNorm(x+1 , y  ) ) );
          setNormal(ptr[2], convert( model.getNorm(x   , y+1) ) );
          setNormal(ptr[3], convert( model.getNorm(x+1 , y+1) ) );
        }

        setTextureCoord(ptr[0], model.getVertexU (x,  y  ), model.getVertexV (x,    y    ) );
        setTextureCoord(ptr[1], model.getVertexU (x+1,y  ), model.getVertexV (x+1,  y    ) );
        setTextureCoord(ptr[2], model.getVertexU (x  ,y+1), model.getVertexV (x,    y+1  ) );
        setTextureCoord(ptr[3], model.getVertexU (x+1,y+1), model.getVertexV (x+1,  y+1  ) );
      }
      else // hide
      {
        setCustomVertex (ptr[0], 0.0f , 0.0f , 0.0f );
        setCustomVertex (ptr[1], 0.0f , 0.0f , 0.0f );
        setCustomVertex (ptr[2], 0.0f , 0.0f , 0.0f );
        setCustomVertex (ptr[3], 0.0f , 0.0f , 0.0f );
        
        D3DVECTOR d={0.0f,0.0f,0.0f};
        setNormal(ptr[0], d );                setNormal(ptr[1], d );
        setNormal(ptr[2], d );                setNormal(ptr[3], d );

        setTextureCoord(ptr[0], 0.0f, 0.0f ); setTextureCoord(ptr[1], 0.0f, 0.0f );
        setTextureCoord(ptr[2], 0.0f, 0.0f ); setTextureCoord(ptr[3], 0.0f, 0.0f );
      }
      ptr+=4 ;
    }

  returnsize = size;
  return p;
}

// get the corresponding index map to the vertex map created with getnewVerticesFromMap
//
short * DirectXControl::getnewIndiceLineFromMap(int sizex, int& returnsize)
{
  int size  = (sizex) * 6 ; // 6 indices per square

  short * p = new short [size];
  
  if (p == NULL) 
    return NULL;

  short * ptr = p;

  for (int x=0; x<sizex ; x++)
  {
    short baseindex =  x * 4 ;

    // first triangle
    ptr[0] = baseindex    ;
    ptr[1] = baseindex + 1;
    ptr[2] = baseindex + 2;

    // second triangle
    ptr[3] = baseindex + 1;
    ptr[4] = baseindex + 3;
    ptr[5] = baseindex + 2;

    ptr+=6 ;
  }

  returnsize = size;
  return p;
}

void DirectXControl::loadTexture_(const Image& image)
{
  if (!image.is_empty())
  {
    HRESULT res = d3ddev_->CreateTexture(image.dimx(), image.dimy(), 1, 0,
                                  D3DFMT_A8R8G8B8, D3DPOOL_MANAGED,
                                  &texture_,
                                  NULL);
    if (FAILED(res))
    {
      d3ddev_ -> Release();
      d3ddev_ = NULL ;
      d3d_ -> Release();
      d3d_ = NULL;

      MUTEXRELEASE;
      THROW_INTERNAL_ERROR( "Display error: could not create texture (in D3DXCreateTextureFromFile() )" );
    }

    D3DLOCKED_RECT     d3dRect;
    if (FAILED(texture_->LockRect(0, &d3dRect, NULL, 0)))
    {
      d3ddev_ -> Release();
      d3ddev_ = NULL ;
      d3d_ -> Release();
      d3d_ = NULL;

      MUTEXRELEASE;
      THROW_INTERNAL_ERROR( "Display error: could not create texture (in D3DXCreateTextureFromFile() )" );
    }

    int  LineWidth = d3dRect.Pitch >> 2; 
    void * ptr = (USHORT *) d3dRect.pBits;

    for (int cy = 0; cy < image.dimy(); cy++)
      for (int cx = 0; cx < image.dimx(); cx++)
      {
        const DWORD ALPHA_MASK = 0xFF000000; // was 0x10000000
        UCHAR R = image(cx,cy,0,0), G = image(cx,cy,0,1), B = image(cx,cy,0,2);
        DWORD col = ALPHA_MASK + (R << 16) + (G << 8) + B ;
        ((DWORD*)ptr)[cx + (cy*LineWidth)] = col ;
      }
    
    texture_->UnlockRect(0);  
  }
}


// puts the 3D model into video RAM
//
void DirectXControl::initVertex_(const Mesh& model, const Image& alpha)
{
  // warning : in case this function or any subroutine fails with an exception, 
  // then use MUTEXRELEASE; before throwing exception to avoid deadlock.
  if (vertexBufferTable_ != NULL)
    THROW_INTERNAL_ERROR("Warning memory leak: vertexBufferTable being erased (in initVertex_())");

  /////////////////////
  // VERTEX TABLES

  // create the vertices using the CUSTOMVERTEX struct
  int count;
  CUSTOMVERTEX * customVertexPtr = getnewVerticesFromMap(model, count, isSmooth_, alpha);

  if (customVertexPtr != NULL)
  {
    // create a vertex buffer interface called vertexBufferTable_
    if (FAILED(d3ddev_->CreateVertexBuffer(sizeof(CUSTOMVERTEX) * count,
                               0, CUSTOMFVF, D3DPOOL_MANAGED, &vertexBufferTable_,
                               NULL)))
    {
      delete[] customVertexPtr;
      d3ddev_ -> Release();
      d3ddev_ = NULL ;
      d3d_ -> Release();
      d3d_ = NULL;

      MUTEXRELEASE;
      THROW_MESSAGE_TO_USER( OUT_OF_MEMORY );
    }

    // lock vertexBufferTable_ and load the vertices into it
    VOID* pVoid;    // a void pointer
    vertexBufferTable_->Lock(0, 0, (void**)&pVoid, 0);
    memcpy(pVoid, customVertexPtr, sizeof(CUSTOMVERTEX) * count);
    vertexBufferTable_->Unlock();

    delete[] customVertexPtr;
  }
  else // problem like "could not allocate memory"
  {
    d3ddev_ -> Release();
    d3ddev_ =  NULL     ;
    d3d_    -> Release();
    d3d_    =  NULL     ;

    MUTEXRELEASE;
    THROW_MEMORY_LIMIT_REACHED();
  }
}



void DirectXControl::initIndex_()
{
  if (indexBufferTable_ != NULL)
    THROW_INTERNAL_ERROR("Warning memory leak: indexBufferTable_ being erased (in initIndex_())");

  /////////////////////
  // INDEX TABLES (INTO VERTEX TABLES)

  // create the indices using an int array
  int nb;
  short *indices = getnewIndiceLineFromMap(modelSizeX_, nb);
  
  if (indices != NULL)
  {
    // create an index buffer interface called i_buffer_
    if (FAILED(d3ddev_->CreateIndexBuffer(sizeof(short) * nb,
                              0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &indexBufferTable_,
                              NULL)))
    {
      // destroys the useless v_buffer that was just created before throwing the error
      vertexBufferTable_ -> Release();
      vertexBufferTable_ = NULL;

      delete[] indices;

      d3ddev_ -> Release();
      d3ddev_ =  NULL     ;
      d3d_    -> Release();
      d3d_    =  NULL     ;

      MUTEXRELEASE;
      THROW_MESSAGE_TO_USER(OUT_OF_MEMORY);
    }

    // lock i_buffer_ and load the indices into it
    VOID* pVoid;    // a void pointer
    indexBufferTable_->Lock(0, 0, (void**)&pVoid, 0);
    memcpy(pVoid, indices, sizeof(short) * nb);
    indexBufferTable_->Unlock();

    delete[] indices;
  }
  else // problem like "could not allocate memory"
  {
    // destroys the useless v_buffer that was just created before throwing the error
    vertexBufferTable_->Release();
    vertexBufferTable_ = NULL;
    d3ddev_ -> Release();
    d3ddev_ =  NULL     ;
    d3d_    -> Release();
    d3d_    =  NULL     ;

    MUTEXRELEASE;
    THROW_MEMORY_LIMIT_REACHED();
  }
}


// this is the function that sets up the lights and materials
void DirectXControl::initLight(float factor)
{
  D3DLIGHT9 light;    // create the light struct
  D3DMATERIAL9 material;    // create the material struct

  ZeroMemory(&light, sizeof(light));    // clear out the light struct for use
  
  light.Type         = D3DLIGHT_DIRECTIONAL;    // make the light type 'directional light'
  light.Diffuse      = D3DXCOLOR(0.9f*factor, 0.9f*factor, 0.9f*factor, 1.0f);    // set the light's color
  light.Direction    = D3DXVECTOR3(0.1f, -0.3f, -1.0f);
  light.Specular     = D3DXCOLOR(0.08f, 0.08f, 0.08f, 1.0f);

  d3ddev_->SetLight(0, &light);    // send the light struct properties to light #0
  d3ddev_->LightEnable(0, TRUE);    // turn on light #0

  ZeroMemory(&material, sizeof(D3DMATERIAL9));    // clear out the struct for use
  material.Diffuse  = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);    // set diffuse color to gray
  material.Ambient  = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);    // set ambient color to white
  material.Specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
  material.Power    = 100.0f;

  d3ddev_->SetMaterial(&material);    // set the globably-used material to &material
}

