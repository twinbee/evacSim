/*********************************************************
** polygon.h, polygon.cpp
** Primary author: Mike E
** Description: polygon class
** Used in sim: detail map
*********************************************************/

//#ifndef usm_polygon
//#define usm_polygon

#include "Vec3.h"
#include "TexCoord.h"
#include <vector>
#include <string>

struct Poly
{
    // these index into master lists kept in the Map
    std::vector<int> vertices;
    std::vector<int> texCoords;
    std::string materialName;

    Poly( const std::string& );
    Vec3 center( const std::vector<Vec3>& verList ) const;
    Vec3 normal( const std::vector<Vec3>& verList ) const;
    void drawNaked( const std::vector<Vec3>& verList ) const;
    void drawNaked( const std::vector<Vec3>& verList, const std::vector<TexCoord>& texList ) const;
};

//#endif
