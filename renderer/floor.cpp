//floor.cpp

#include "floor.h"

#include "draw.h"

#include <iostream>


Floor::Floor(const std::vector <CVector> & vertices, const CVector & center)
{

 numVerts = vertices.size()+1;
 verts = new dVector3[numVerts];

 verts[0][0] = verts[0][1] = verts[0][2] = 0;

 for (int i = 1; i < numVerts; i++)
 {
  verts[i][0] = vertices[i-1].x - center.x;
  verts[i][1] = vertices[i-1].y - center.y;
  verts[i][2] = vertices[i-1].z - center.z;
 }
 
 //questionable: assumes winding is good and planar, convex

 numTriangles = numVerts-1;
 numIndices = (numTriangles)*3; //numverts - 2 triangles, 3 vertices each

 indices = new int[numIndices];

 for (int i = 0; i < numTriangles; i++)
 {
  indices[(i*3+0)] = 0;
  indices[(i*3+1)%numIndices] = (i+1)%(numVerts);
  indices[(i*3+2)%numIndices] = (i+2)%(numVerts);
  //std::cout << "i*3:" << i*3 << " " << indices[(i*3+0)] << " " << indices[(i*3+1)] << " " << indices[(i*3+2)] << std::endl;
 }

 indices[numIndices-1] = 1;

 TriMeshData=dGeomTriMeshDataCreate();

 dGeomTriMeshDataBuildSimple (TriMeshData, (dReal*)verts, numVerts, indices, numIndices);
 //dGeomTriMeshDataBuildSimple1 (TriMeshData, (dReal*)verts, numVerts, indices, numIndices, normals );

 o_geom = dCreateTriMesh(S, TriMeshData, NULL, NULL, NULL);
 
 dGeomSetCategoryBits (o_geom,CAT_FLOOR );
 dGeomSetCollideBits (o_geom,CBIT_FLOOR );

 CVector pos = center;
 dGeomSetPosition(o_geom, pos.x, pos.y, pos.z);
}

void Floor::Draw()
{

    if (DrawFlags & DRAW_FLOOR)
    {

    glPushMatrix();

   	const dReal *realP;
    dVector3 v1, v2, v3;
 		realP = dGeomGetPosition(o_geom);

        glColor3f(0.0, 0.3, 0.0);
        glBegin( GL_LINES );

       for (int i = 0; i < numTriangles; ++i) {
				dGeomTriMeshGetTriangle(o_geom, i, &v1, &v2, &v3 );
				glVertex3fv(v1);
				glVertex3fv(v2);
				glVertex3fv(v3);
				glVertex3fv(v1);
			}

        glEnd();

        glColor3f(0.0, 0.7, 0.0);
        glBegin( GL_TRIANGLES );

       for (int i = 0; i < numTriangles; ++i) {
				dGeomTriMeshGetTriangle(o_geom, i, &v1, &v2, &v3 );
				glVertex3fv(v1);
				glVertex3fv(v2);
				glVertex3fv(v3);
			}

        glEnd();
        glPopMatrix();
    }

}
