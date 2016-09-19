/*********************************************************
** image.cpp, image.h
** Primary author: Mike E
** Description: JPEG loading
** Used in sim: texture on detail map
*********************************************************/

#ifndef usm_image
#define usm_image

class Image
{
    unsigned pixelWidth;
    unsigned pixelHeight;
    unsigned texelWidth;
    unsigned texelHeight;
    unsigned char* pixels;
    unsigned char* texels;
    int texID;

public:
    Image( const char* path ); // read from jpeg file
    ~Image();

    bool valid();

    void write( const char* path );

    int w();
    int h();

    void drawAt( int x, int y );
    void useAsTexture();
    void map( float s, float t );
};

#endif
