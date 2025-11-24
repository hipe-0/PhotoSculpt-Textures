// basic file operations
#include "ExportStl.h"

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include "Vector3.h"
#include "IntMap.h"
#include "Includes.h"
#include "Mesh.h"
#include "Convert.h"

/*
Example of stl file:

solid WRAP
facet normal 0.373061 0.846172 0.380550
outer loop
vertex -120.677099 32.569900 38.680002
vertex -116.843104 29.565001 41.602999
vertex -117.083997 32.688901 34.893002
endloop
endfacet
facet normal -0.594370 0.776551 -0.209025
outer loop
vertex -51.498100 60.365003 -0.002100
vertex -55.682998 57.486001 1.202000
vertex -54.008000 59.411898 3.594000
endloop
endfacet
end solid
*/

bool testAlpha(const Image& alphamap, const Mesh &model, int x, int y);

bool ExportStl::exportStlFile (char* filename, const IntMap&  map, 
  const Image& alpha,
  int skip,float xyScale, float zScale, float focallengthinmm)
{
  DEBUG(exportStl);
  DEBUG_VALUE("  file", filename);

  std::ofstream myfile;

  int sizex=map.getSizeX(), sizey=map.getSizeY();

  // Open the file to export to.
  myfile.open (filename);
  if (!myfile.is_open()) return false;

  // header : definition of an object name
  myfile << "solid Photosculpt\n";

  Mesh mesh;
  FloatMap floatmap = convertToFloat(map) * zScale;
  mesh.create(floatmap, focallengthinmm);
  

  // Triangle declaration export
  //
  for (int y=0; y<sizey-skip ; y+=skip)
    for (int x=0; x<sizex-skip ; x+=skip)
      if (testAlpha(alpha, mesh, x,y) )
      {
        // Computings all 4 vertexes for the quad at x,y
        //
        // v1 is top left, v2 is top right, v3 is bottom left, v4 is bottom right
        //
        Vector3 v1( mesh.getVertexX(x,      y      ),
                   -mesh.getVertexY(x,      y      ) + float(sizey-1) / float(sizex-1),
                    mesh.getVertexZ(x,      y      ));

        Vector3 v2( mesh.getVertexX(x+skip, y      ),
                   -mesh.getVertexY(x+skip, y      ) + float(sizey-1) / float(sizex-1),
                    mesh.getVertexZ(x+skip, y      ));

        Vector3 v3( mesh.getVertexX(x,      y+skip ),
                   -mesh.getVertexY(x,      y+skip ) + float(sizey-1) / float(sizex-1),
                    mesh.getVertexZ(x,      y+skip ));

        Vector3 v4( mesh.getVertexX(x+skip, y+skip ),
                   -mesh.getVertexY(x+skip, y+skip ) + float(sizey-1) / float(sizex-1),
                    mesh.getVertexZ(x+skip, y+skip ));

        // export the computed quad, spitting it thru the diagonalinto 2 triangles
        //
        // first triangle using corners 1,2,4
        //
        // Computing triangle normal using the vector cross product (operator^)
        //
        Vector3 normal1=normalize( (v1-v2) ^ (v1-v4) );
        myfile << "facet normal " << normal1.x << " " << normal1.y << " " << normal1.z << "\n";

        myfile << "outer loop\n";

        myfile << "vertex " << v1.x << " " << v1.y << " " << v1.z << "\n";
        myfile << "vertex " << v4.x << " " << v4.y << " " << v4.z << "\n";
        myfile << "vertex " << v2.x << " " << v2.y << " " << v2.z << "\n";

        myfile << "endloop\n";
        myfile << "endfacet\n";

        // second triangle using corners 1,4,3
        //
        // Computing trianglenormal using the vector cross product (operator^)
        //
        Vector3 normal2=normalize( (v1-v4) ^ (v1-v3) );
        myfile << "facet normal " << normal2.x << " " << normal2.y << " " << normal2.z << "\n";

        myfile << "outer loop\n";

        myfile << "vertex " << v1.x << " " << v1.y << " " << v1.z << "\n";
        myfile << "vertex " << v3.x << " " << v3.y << " " << v3.z << "\n";
        myfile << "vertex " << v4.x << " " << v4.y << " " << v4.z << "\n";

        myfile << "endloop\n";
        myfile << "endfacet\n";
      }

  myfile << "end solid\n";

  myfile.close();
  return true;
}
