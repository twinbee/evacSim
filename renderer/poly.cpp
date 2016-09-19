/*********************************************************
** poly.cpp, poly.h
** Primary author: Mike E
** Description: generic polygon functions
** Used in sim: all over
*********************************************************/

#include "poly.h"
#include "Material.h"
#include "TexCoord.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

// function to fill poly list from OBJ file

void polygonsRead(
    map<string,Material>& materials,
    vector<CVector>& vertices,
    vector<CLine*>& lines,
    vector<CPolygon*>& polygons,
    const char* path )
{
    const bool debug = false;

    ifstream file( path );
    if ( not file )
    {
        if (debug) cout << "<!> could not open object file: " << path << endl;
        return;
    }

    vector<TexCoord> tex;
    Material material;

    const float mystery = 1;

    while ( file )
    {
        string line;
        getline( file, line );
        if ( line.length() <= 1 ) // blank
            continue;
        if ( line.at(0) == '#' ) // comment
            continue;

        istringstream ss( line );
        string token;
        ss >> token;

        if ( token == "v")
        {
            CVector foo;
            ss >> foo;
            vertices.push_back( foo / mystery );
        }
        else if ( token == "vt")
        {
            TexCoord foo;
            ss >> foo.s >> foo.t;
            foo.s /= mystery;
            foo.t /= mystery;
            tex.push_back( foo );
            if (debug) cout << "tex <" << foo.s << "," << foo.t << ">\n";
        }
        else if ( token == "f")
        {
            CPolygon* poly = new CPolygon;

            unsigned verID;

            while ( ss >> verID )
            {
                poly->addVertex( vertices[ verID - 1 ]);

                if ( ss.get() == '/' )
                {
                    unsigned texID;
                    if ( ss >> texID )
                        poly->addTexCoord( tex[ texID - 1 ]);
                }

                if ( ss.get() == '/' )
                    ss.ignore(16,' '); // skip normal
            }

            if ( poly->NumVerts() >= 3 )
            {
                poly->computeAll(0.0);
                poly->SetMaterial( material );
                polygons.push_back( poly );
            }
            else if ( poly->NumVerts() == 2 )
            {
                const vector<CVector>& v = poly->GetVertices();
                lines.push_back( new CLine( v[0], v[1], convert( material.diffuse )));
                delete poly;
            }
            else
                delete poly;
        }
        else if ( token == "usemtl")
        {
            string name;
            ss >> name;
            material = materials[ name ];
        }
        else if ( token == "mtllib")
        {
            string name;
            ss >> name;
            materialsRead( materials, name.c_str());
        }
    }
}

/*
void polygonsReport( const vector<Polygon>& polygons )
 {
 map<unsigned, unsigned> cornerCount;
 map<unsigned, unsigned>::const_iterator j;

 vector<Polygon>::const_iterator i;
 for ( i = polygons.begin(); i != polygons.end(); ++i )
  cornerCount[ i->n() ]++;

 if (debug) cout << polygons.size() << " polygons, distributed as such:\n";

 for ( j = cornerCount.begin(); j != cornerCount.end(); ++j )
  if (debug) cout << j->first << " -> " << j->second << endl;
 }
*/
