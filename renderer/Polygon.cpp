/*********************************************************
** polygon.h, polygon.cpp
** Primary author: Mike E
** Description: polygon class
** Used in sim: detail map
*********************************************************/

#include "Polygon.h"
#include "graphics.h"

using namespace std;

Poly::Poly( const string& s )
        : materialName( s )
{ }

Vec3 Poly::center( const vector<Vec3>& verList ) const
{
    Vec3 sum;
    int n = vertices.size();
    for ( int i = 0; i < n; ++i )
        sum += verList[ vertices[i] ];
    return 1.f / n * sum;
}

Vec3 Poly::normal( const vector<Vec3>& verList ) const
{
    const Vec3& a = verList[ vertices[0] ];
    const Vec3& b = verList[ vertices[1] ];
    const Vec3& c = center( verList );
    return cross( c - a, c - b ).normalized();
}

void Poly::drawNaked( const vector<Vec3>& verList ) const
{
    const unsigned n = vertices.size();
    const Vec3 norm = normal( verList );

    glNormal3f( norm.x, norm.y, norm.z );

    for ( unsigned i = 0; i < n; ++i )
    {
        const Vec3& ver = verList[ vertices[ i ]];
        glVertex3f( ver.x, ver.y, ver.z );
    }
}


void Poly::drawNaked( const vector<Vec3>& verList, const vector<TexCoord>& texList ) const
{
    const unsigned n = vertices.size();
    const Vec3 norm = normal( verList );

    glNormal3f( norm.x, norm.y, norm.z );

    for ( unsigned i = 0; i < n; ++i )
    {
        texList[ texCoords[ i ]].set();
        const Vec3& ver = verList[ vertices[ i ]];
        glVertex3f( ver.x, ver.y, ver.z );
    }
}

