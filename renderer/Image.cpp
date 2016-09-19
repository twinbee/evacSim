/*********************************************************
** image.cpp, image.h
** Primary author: Mike E
** Description: JPEG loading
** Used in sim: texture on detail map
*********************************************************/


#include "Image.h"
#include "graphics.h"
#include <iostream>
extern "C" {
#include <jpeglib.h>
}

using namespace std;

extern const bool debug;

Image::Image( const char* path )
        : pixelWidth( 0 )
        , pixelHeight( 0 )
        , texelWidth( 0 )
        , texelHeight( 0 )
        , pixels( NULL )
        , texels( NULL )
        , texID( 0 )
{
    jpeg_decompress_struct cinfo;
    FILE *file;

    if (( file = fopen( path, "rb")) == NULL )
        return; // or throw something

    jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, file);
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);

    assert ( cinfo.num_components == 3 ); // we're only concerned about RGB images at this point

    pixelWidth = cinfo.image_width;
    pixelHeight = cinfo.image_height;

    int rowSpan = pixelWidth * cinfo.num_components;

    pixels = new unsigned char [ rowSpan * pixelHeight ];

    // start at the end of the array, fill backward
    unsigned char* rowPtr = pixels + rowSpan * pixelHeight;

    while ( cinfo.output_scanline < cinfo.output_height )
    {
        rowPtr -= rowSpan;
        jpeg_read_scanlines( &cinfo, &rowPtr, 1 );
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(file);

    if ( debug )
        if (debug) cout << "image created\n";
}

Image::~Image()
{
    if ( texID )
        glDeleteTextures( 1, (const GLuint*) &texID );

    delete [] pixels;
    if ( texels != pixels )
        delete [] texels;

    if ( debug )
        if (debug) cout << "image destroyed\n";
}

bool Image::valid()
{
    if ( pixels )
    {
        useAsTexture();
        return glGetError() == GL_NO_ERROR;
    }
    else
        return false;
}

void Image::write( const char* path )
{
}

int Image::w()
{
    return pixelWidth;
}

int Image::h()
{
    return pixelHeight;
}

void Image::drawAt( int x, int y )
{
    if ( pixels )
    {
        glRasterPos2i( x, y );
        glDrawPixels( pixelWidth, pixelHeight, GL_RGB, GL_UNSIGNED_BYTE, pixels );
        ShowGLError
    }
}

unsigned nextPowerOfTwo( unsigned n )
{
    unsigned exp;
    for ( exp = 1, --n; n > 1; ++exp )
        n >>= 1;
    return 1 << exp;
}

unsigned textureSize( unsigned n )
{
    unsigned max = 2048;
    unsigned x = nextPowerOfTwo( n );
    if ( x > max )
        return max;
    else
        return x;
}

void Image::useAsTexture()
{
    if ( texID == 0 ) // first time to be used as texture
    {
        if ( debug )
            if (debug) cout << "creating texture object\n";

        glGenTextures( 1, (GLuint*) &texID );
        glBindTexture( GL_TEXTURE_2D, texID );
        glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glPixelStorei( GL_PACK_ALIGNMENT, 1 );
        glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

        ShowGLError

        texelWidth = textureSize( pixelWidth );
        texelHeight = textureSize( pixelHeight );

        if ( texelWidth == pixelWidth and texelHeight == pixelHeight )
            texels = pixels;
        else
        {
            // if ( debug )
            //  if (debug) cout << pixelWidth << 'x' << pixelHeight << " -> " << texelWidth << 'x' << texelHeight << endl;

            texels = new unsigned char [ texelWidth * texelHeight * 3 ];

            gluScaleImage(GL_RGB, pixelWidth, pixelHeight,GL_UNSIGNED_BYTE,pixels,texelWidth,texelHeight,GL_UNSIGNED_BYTE,texels);

            ShowGLError

        }

        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, texelWidth, texelHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, texels );

        ShowGLError

    }
    else
    {
        // if ( debug )
        //   if (debug) cout << "reusing texture object\n";
        glBindTexture( GL_TEXTURE_2D, texID );
// ShowGLError
    }
}

void Image::map( float s, float t )
{
    glTexCoord2f( s, t );
}
