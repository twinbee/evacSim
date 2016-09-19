/*********************************************************
** model.h, model.cpp
** Primary author: Mike E
** Description: mike E's detail map with superhyper rendering
** Used in sim: detail map of stadium
*********************************************************/

#include "Model.h"
#include "graphics.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <set>

using namespace std;
extern const bool debug;

Model::Model( const char* path )
        : displayList( 0 )
{
    ifstream file( path );
    if ( not file )
    {
        throw 0;
        return;
    }

    string materialName;

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

        if ( token == "v") // vertex
        {
            Vec3 foo;
            ss >> foo.x >> foo.y >> foo.z;
            foo = 1.f / 1 * foo; // scale to match pathing map
            vertices.push_back( foo );
            bb.engulf( foo );
        }
        else if ( token == "vt") // texture coordinate
        {
            TexCoord foo;
            ss >> foo.s >> foo.t;
            texCoords.push_back( foo );
        }
        else if ( token == "f") // face
        {
            Poly poly( materialName );

            unsigned verID;
            while ( ss >> verID )
            {
                poly.vertices.push_back( verID - 1 );

                if ( ss.get() == '/' )
                {
                    unsigned texID;
                    if ( ss >> texID )
                        poly.texCoords.push_back( texID - 1 );
                }

                if ( ss.get() == '/' )
                    ss.ignore(16,' '); // skip normal
            }

            if ( poly.vertices.size() >= 3 ) // which it should be, just checking
                polygons.push_back( poly );
        }
        else if ( token == "usemtl")
            ss >> materialName;
        else if ( token == "mtllib")
        {
            string dir( path );
            dir = dir.substr( 0, dir.find_last_of('\\') + 1 );

            string name;
            ss >> name;

            string fullpath = dir + name;
            if ( debug )
                if (debug) cout << "mtl file at " << fullpath << endl;

            materialsRead( materials, fullpath.c_str());

            if ( debug )
                materialsReport( materials, cout );
        }
    }
   //  cout << "finish loading map , num polys :"<<polygons.size()<<" \n";
}

Model::~Model()
{
    if ( displayList )
        glDeleteLists( displayList, 1 );
}

const BoundingBox_& Model::boundingBox() const
{
    return bb;
}

unsigned primitiveCount = 0;
unsigned vertexCount = 0;
unsigned groupCount = 0;

void Model::buildDisplayList() const
{
    // this code seems long and complicated, so hang on!
    // it does extra work up front to make a really tight display list
    // remember, only the OpenGL calls are stored

    // verify each material - this also does the major work of rescaling and
    // building texture objects, so should be outside the display list
    materialsVerify( materials );

    displayList = glGenLists( 1 );
    glNewList( displayList, GL_COMPILE );
    glShadeModel( GL_FLAT );

    // group polygons by material, to avoid OpenGL state changes

    map< string, set< const Poly* > > coloredPolygons;
    map< string, set< const Poly* > > texturedPolygons;

    for ( vector<Poly>::const_iterator i = polygons.begin(); i != polygons.end(); ++i )
    {
        const Poly& poly = *i;
        const Material& material = materials.find( poly.materialName )->second;

        if ( material.texture == NULL )
            coloredPolygons[ poly.materialName ].insert( &poly );
        else
            texturedPolygons[ poly.materialName ].insert( &poly );
    }

    // now that they're grouped, it's time to draw

    map< string, set< const Poly* > >::const_iterator i;
    set< const Poly* >::const_iterator j;

    // draw non-textured polygons first...

    glDisable( GL_TEXTURE_2D );

    primitiveCount = coloredPolygons.size();

    for ( i = coloredPolygons.begin(); i != coloredPolygons.end(); ++i )
    {
        const Material& material = materials.find(i->first)->second;
        material.diffuse.set();

        set< const Poly* > triangles;
        set< const Poly* > quads;
        set< const Poly* > others;

        for ( j = i->second.begin(); j != i->second.end(); ++j )
        {
            const Poly& poly = **j;
            const unsigned n = poly.vertices.size();

            if ( n == 3 )
                triangles.insert( &poly );
            else if ( n == 4 )
                quads.insert( &poly );
            else if ( n >= 5 )
                others.insert( &poly );
        }

        if ( not triangles.empty())
        {
            ++groupCount;
            glBegin( GL_TRIANGLES );
            for ( j = triangles.begin(); j != triangles.end(); ++j )
                (**j).drawNaked( vertices );
            glEnd();
        }
        if ( not quads.empty())
        {
            ++groupCount;
            glBegin( GL_QUADS );
            for ( j = quads.begin(); j != quads.end(); ++j )
                (**j).drawNaked( vertices );
            glEnd();
        }
        if ( not others.empty())
        {
            for ( j = others.begin(); j != others.end(); ++j )
            {
                ++groupCount;
                glBegin( GL_POLYGON );
                (**j).drawNaked( vertices );
                glEnd();
            }
        }
    }

    // ... and then the textured polygons

    glEnable( GL_TEXTURE_2D );
    glColor3f( 1, 1, 1 ); // modulate with white
    // use other colors to indicate and highlight specific features

    for ( i = texturedPolygons.begin(); i != texturedPolygons.end(); ++i )
    {
        const Material& material = materials.find(i->first)->second;
        material.texture->useAsTexture();
        set< const Poly* > triangles;
        set< const Poly* > quads;
        set< const Poly* > others;

        for ( j = i->second.begin(); j != i->second.end(); ++j )
        {
            const Poly& poly = **j;
            const unsigned n = poly.vertices.size();

            if ( n == 3 )
                triangles.insert( &poly );
            else if ( n == 4 )
                quads.insert( &poly );
            else if ( n >= 5 )
                others.insert( &poly );
        }

        if ( not triangles.empty())
        {
            ++groupCount;
            glBegin( GL_TRIANGLES );
            for ( j = triangles.begin(); j != triangles.end(); ++j )
                (**j).drawNaked( vertices, texCoords );
            glEnd();
        }
        if ( not quads.empty())
        {
            ++groupCount;
            glBegin( GL_QUADS );
            for ( j = quads.begin(); j != quads.end(); ++j )
                (**j).drawNaked( vertices, texCoords );
            glEnd();
        }
        if ( not others.empty())
        {
            for ( j = others.begin(); j != others.end(); ++j )
            {
                ++groupCount;
                glBegin( GL_POLYGON );
                (**j).drawNaked( vertices, texCoords );
                glEnd();
            }
        }
    }

    glDisable( GL_TEXTURE_2D );
    glEndList();

    if ( debug ) cout << primitiveCount << " primitives in " << groupCount << " groups\n";

    ShowGLError
}



void Model::draw() const
{
    if ( not displayList )
        buildDisplayList();

    light();
    glCallList( displayList );
}

void Model::light() const
{
    //create the lights
    GLfloat LightPosition0[]= { (bb.xmin + bb.xmax) / 2.0, (bb.ymin + bb.ymax) /2.0, bb.zmax , 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION,LightPosition0);
    glEnable( GL_LIGHT0 );

    GLfloat LightPosition1[]= { (bb.xmin + bb.xmax) / 2.0, (bb.ymin + bb.ymax) /2, (bb.zmax + bb.zmin ) /2.0 , 1.0f };
    glLightfv(GL_LIGHT1, GL_POSITION,LightPosition1);
    glEnable( GL_LIGHT1 );

    GLfloat LightPosition2[]= { bb.xmax, bb.ymin, bb.zmin , 1.0f };
    glLightfv(GL_LIGHT2, GL_POSITION,LightPosition2);
    glEnable( GL_LIGHT2 );

    GLfloat LightPosition3[]= { bb.xmin, bb.ymax, bb.zmin , 1.0f };
    glLightfv(GL_LIGHT3, GL_POSITION,LightPosition3);
    glEnable( GL_LIGHT3 );

    glEnable( GL_LIGHTING );
}

void Model::showNormals( float length ) const
{
    // show normals (bad ones converge in black)

    glBegin( GL_LINES );
    for ( vector<Poly>::const_iterator i = polygons.begin(); i != polygons.end(); ++i )
    {
        const Poly& poly = *i;
        const Vec3& a = poly.center( vertices ); // center( poly );
        const Vec3& b = a + length * poly.normal( vertices ); // normal( poly );
        glColor3f( 0, 0, 0 );
        glVertex3f( a.x, a.y, a.z );
        glColor3f( 1, 1, 1 );
        glVertex3f( b.x, b.y, b.z );
    }
    glEnd();
}
