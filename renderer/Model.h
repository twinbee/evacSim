/*********************************************************
** model.h, model.cpp
** Primary author: Mike E
** Description: mike E's detail map with superhyper rendering
** Used in sim: detail map of stadium
*********************************************************/

#ifndef usm_model
#define usm_model

// this class represents 3D models
// they are loaded from OBJ files and drawn using OpenGL

#include "BoundingBox.h"
#include "Material.h"
#include "Polygon.h"

class Model
{
    std::vector<Vec3> vertices;
    std::vector<TexCoord> texCoords;
    std::vector<Poly> polygons;
    BoundingBox_ bb;
    std::map<std::string,Material> materials;
    mutable int displayList;

    void showNormals( float length ) const;

public:
    Model( const char* path ); // load from OBJ file
    ~Model();

    const BoundingBox_& boundingBox() const;
    void buildDisplayList() const;
    void light() const;
    void draw() const;
};

#endif
