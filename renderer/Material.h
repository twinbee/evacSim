/*********************************************************
** material.h, material.cpp
** Primary author: Mike E
** Description: applies textures / material properties such as color
** Used in sim: texture on detail map
*********************************************************/

#ifndef usm_material
#define usm_material

#include "new_Color.h"
#include "Image.h"
#include <iosfwd>
#include <map>
#include <string>

struct Material
{
    Color diffuse;
    Image* texture;

    Material(); // boring default
    Material( const Material& ); // copy constructor
    Material( const Color&, Image* = NULL );
    Material( std::istream& ); // load from entry in MTL file
    ~Material();

    void read( std::string path, std::istream& );

    void set() const; // sets color or texture for subsequent drawing
};

// functions for handling collections of materials
bool materialsRead( std::map<std::string,Material>&, const char* path );
void materialsReport( const std::map<std::string,Material>&, std::ostream& );
bool materialsVerify( const std::map<std::string,Material>& );

#endif
