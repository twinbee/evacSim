/*********************************************************
** material.h, material.cpp
** Primary author: Mike E
** Description: applies textures / material properties such as color
** Used in sim: texture on detail map
*********************************************************/


#include "Material.h"
#include "graphics.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

extern const bool debug;

Material::Material()
        :  texture( NULL )
{
    if ( debug )
        if (debug) cout << "  empty default material\n";
}

Material::Material( const Material& m )
{
    //if ( debug ) cout << "  copy of existing material\n";
    diffuse = m.diffuse;
    texture = m.texture; // no copy
}

Material::Material( const Color& c, Image* t )
        : diffuse( c )
        , texture( t )
{
    //if ( debug ) cout << "  new material from components\n";
}

Material::Material( istream& in )
        :  texture( NULL )
{
    read( "", in );
}

void Material::read( string path, istream& in )
{
    if ( debug ) cout << "  replace material with input\n";

    in.ignore(3,'\n'); // newline should be immediate
    while ( true )
    {
        string line, key;
        getline( in, line );

        if ( line.length() <= 1 ) // practically empty
            return;

        istringstream ss( line );

        ss >> key;

        if ( key == "Kd" ) // diffuse color
            ss >> diffuse.red >> diffuse.green >> diffuse.blue;
        else if ( key == "map_Kd" ) // texture map
        {
            string filename;
            ss >> filename;
            string fullpath = path + filename;
            if ( debug ) cout << "image at " << fullpath << endl;
            texture = new Image( fullpath.c_str());
        }
    }
}

Material::~Material()
{
    // if ( debug ) cout << "  destroying material\n";
}

void Material::set() const
{
    // this is safe and not efficient
    // see Map::buildDisplayList for a quicker way
    // this will probably be changed to use that style

    if ( texture )
    {
        glEnable( GL_TEXTURE_2D );
        glColor3f( 1, 1, 1 ); // modulate with white by default (change elsewhere to highlight)
        texture->useAsTexture();
    }
    else
    {
        glDisable( GL_TEXTURE_2D );
        diffuse.set();
    }
}

// functions for handling collections of materials

bool materialsRead( map<string,Material>& materials, const char* path )
{
    ifstream file( path );
    string dir( path );
    dir = dir.substr( 0, dir.find_last_of('\\') + 1 );

    if ( not file )
    {
        if (debug) cout << "<!> could not open materials file: " << path << endl;
        return false;
    }

    while ( file )
    {
        string token;
        file >> token;

        if ( token == "newmtl")
        {
            string name;
            file >> name;

//      string fullpath = dir + name;
//      if ( debug )
//        cout << "jpeg file at " << fullpath << endl;

            materials[ name ].read( dir, file );// = Material( file );
        }
    }

    return true;
}

void materialsReport( const map<string,Material>& materials, ostream& o )
{
    for ( map<string,Material>::const_iterator i = materials.begin(); i != materials.end(); ++i )
    {
        const string& name = i->first;
        const Material& material = i->second;

        o << "   " << name << " (color";
        if ( material.texture )
            o << ",texture";
        o << ")\n";
    }

    o << "-- " << materials.size() << " materials --" << endl;
}

bool materialsVerify( const map<string,Material>& materials )
{
    bool ok = true;
    for ( map<string,Material>::const_iterator i = materials.begin(); i != materials.end(); ++i )
    {
        const string& name = i->first;
        const Material& material = i->second;

        if ( material.texture and not material.texture->valid())
        {
            if (debug) cout << "<!> bad texture in material " << name << endl;
            ok = false;
        }
    }
    return ok;
}
