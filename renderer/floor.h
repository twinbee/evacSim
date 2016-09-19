//floor.h

#ifndef FLOOR_H
#define FLOOR_H

#include "physworld.h"

#include "vector.h"

#include "physical.h"

#include <vector>

class Floor : public Physical
{
 public:
    Floor(const std::vector <CVector> & vertices, const CVector & Center, const CVector & Normal );
    Floor(const std::vector <CVector> & vertices, const CVector & Center);
    void Draw();
    
    //CVector GetPos();

    int getClass () {return 4;}

 private:
  dVector3 * verts;
  int numVerts;
  int numTriangles;

  int * indices;
  int numIndices;

 dTriMeshDataID TriMeshData;
 dGeomID o_geom;


};


#endif
