/*********************************************************
** textures.cpp, textures.h
** Primary author: Mike J
** Description: texture loading for user-interface specific textures
** Used in sim: only for crosshairs, menu textures, and similar
*********************************************************/

//TODO: replace with Mike E's kind of code

#ifndef TEXTURES_H
#define TEXTURES_H

/***************************************************************************
TEXTURE SYSTEM - Loads, registers, stores, and destroys all textures used in
the game.  CreateTexture() accepts a pointer to an image file and adds it
as a Texture struct to the TextureList.  It also registers the texture
as a Texture Object in the TextureObject[] array.  The function
FilenameToID is used to quickly bind textures using only their names,
being as you won't know what their ID is unless you keep count of their
creation order.  BindTexture(GL_TEXTURE_2D,FilenameToID("texturefilename.bmp")) ...etc.
Simply Load the data with the resource manager, create the texture using
CreateTexture() and the data the ResManager Loaded, then bind in the manner
shown above, and the textures will work fine.
****************************************************************************/


#include <GL/gl.h>
#include <GL/glu.h>

#define      GL_COMBINE_ARB                                     0x8570
#define      GL_COMBINE_RGB_ARB                                 0x8571
#define      GL_COMBINE_ALPHA_ARB                               0x8572
#define      GL_SOURCE0_RGB_ARB                                 0x8580
#define      GL_SOURCE1_RGB_ARB                                 0x8581
#define      GL_SOURCE2_RGB_ARB                                 0x8582
#define      GL_SOURCE0_ALPHA_ARB                               0x8588
#define      GL_SOURCE1_ALPHA_ARB                               0x8589
#define      GL_SOURCE2_ALPHA_ARB                               0x858A
#define      GL_OPERAND0_RGB_ARB                                0x8590
#define      GL_OPERAND1_RGB_ARB                                0x8591
#define      GL_OPERAND2_RGB_ARB                                0x8592
#define      GL_OPERAND0_ALPHA_ARB                              0x8598
#define      GL_OPERAND1_ALPHA_ARB                              0x8599
#define      GL_OPERAND2_ALPHA_ARB                              0x859A
#define      GL_RGB_SCALE_ARB                                   0x8573


#define      GL_ADD_SIGNED_ARB                                  0x8574
#define      GL_INTERPOLATE_ARB                                 0x8575
#define      GL_SUBTRACT_ARB                                    0x84E7

#define      GL_CONSTANT_ARB                                    0x8576
#define      GL_PRIMARY_COLOR_ARB                               0x8577
#define      GL_PREVIOUS_ARB                                    0x8578




#define BITMAP_ID 0x4D42
#define MAX_TEXTURES 250
// BUTTbyte-align structures
#ifdef _MSC_VER
#	pragma pack( push, packing )
#	pragma pack( 1 )
#	define PACK_STRUCT
#elif defined( __GNUC__ )
#	define PACK_STRUCT	__attribute__((packed))
#else
#	error you must BUTTbyte-align these structures with the appropriate compiler directives
#endif

typedef short BUTTWORD;
typedef int DBUTTWORD;
typedef long BUTTLONG;
typedef char BUTTbyte;



struct TargaFileHeader
{
    unsigned char IDLength;
    unsigned char ColorMapType;     //type of color map; always 0
    unsigned char ImageTypeCode;    //uncompressed RGB is 2; uncompressed grayscale is 3
    //RLE RGB = 10;  Compressed black and white = 11
    short int ColorMapOrigin;       //always 0
    short int ColorMapLength;       //always 0
    unsigned char ColorMapEntrySize;    //always 0
    short int ImageXOrigin;         //x coordinate of lower-left corner of image
    short int ImageYOrigin;         //y coordinate of lower-left corner of image
    short int ImageWidth;           //width of image in pixels (lo-hi)
    short int ImageHeight;          //height of image in pixels (lo-hi)
    unsigned char BitCount;         //number of bits; 16,24,32
    unsigned char ImageDescriptor;  //24bit = 0x00; 32bit = 0x08
} PACK_STRUCT;


// Default alignment
#ifdef _MSC_VER
#	pragma pack( pop, packing )
#endif

#undef PACK_STRUCT



struct tTexture
{
    //BUTTLONG Width;
    //BUTTLONG Height;
    int Width;
    int Height;
    GLenum Format;
    GLenum Type;
    unsigned int components;
    unsigned char * ImageData;

};


bool CreateTexture(unsigned char * , unsigned int *,int param, bool mipmap);
//tTexture * CreateBMPTexture(unsigned char *);
tTexture * CreateTGATexture(unsigned char *);
//tTexture * CreateJPGTexture(char *);
//void DecodeJPG(jpeg_decompress_struct* , tTexture *);


#endif


