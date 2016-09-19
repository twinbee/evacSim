/*********************************************************
** textures.cpp, textures.h
** Primary author: Mike J
** Description: texture loading for user-interface specific textures
** Used in sim: only for crosshairs, menu textures, and similar
*********************************************************/

//TODO: replace with Mike E's kind of code

#include "textures.h"

#include <fstream>
#include <iostream>

using namespace std;




bool  CreateTexture(unsigned char * Filename, unsigned int * texarray,int param, bool mipmap)
{

    char * extStr;
    // get extension from filename
    extStr = strchr((char *)Filename, '.');
    extStr++;

    tTexture * NewTexture = NULL;
    // set the texture type based on extension of filename
// if ((strcmp(extStr, "BMP") == 0) || (strcmp(extStr, "bmp") == 0))
//  NewTexture = CreateBMPTexture(Filename);
// else if ((strcmp(extStr, "JPG") == 0) || (strcmp(extStr, "jpg") == 0) )
//  NewTexture = CreateJPGTexture(Filename);
    if ((strcmp(extStr, "TGA") == 0) || (strcmp(extStr, "tga") == 0) )
        NewTexture = CreateTGATexture(Filename);

    if (NewTexture)
    {

        glGenTextures(1,texarray);

        glBindTexture(GL_TEXTURE_2D, *texarray);

        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);




        if (mipmap)
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, param);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, param);

            gluBuild2DMipmaps(GL_TEXTURE_2D, NewTexture->components, NewTexture->Width,
                              NewTexture->Height, NewTexture->Format, GL_UNSIGNED_BYTE, NewTexture->ImageData);
        }
        else
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, param);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, param);

            glTexImage2D(GL_TEXTURE_2D, 0, NewTexture->components,NewTexture->Width,
                         NewTexture->Height, 0, NewTexture->Format, GL_UNSIGNED_BYTE, NewTexture->ImageData);
        }




        delete NewTexture->ImageData;
        delete NewTexture;


        return true;
    }

    return false;

}


/*
tTexture * CreateBMPTexture(unsigned char * Filename)
{

    ifstream fin;
    fin.open((char *)Filename,ios::binary);
    if(fin.fail())
        return false;
    fin.seekg(0,ios::end);
    int Filesize = fin.tellg();
    fin.seekg(0,ios::beg);


    unsigned char * Data = new unsigned char[Filesize];
    fin.read((char *)Data,Filesize);
 fin.close();

    unsigned char * ptData = Data;
    BitmapFileHeader * ptBitmapFileHeader=(BitmapFileHeader*)ptData;
    ptData+=sizeof(BitmapFileHeader);

    if(ptBitmapFileHeader->ID != BITMAP_ID)
 {
  delete Data;
  return NULL;
 }

    BitmapInfoHeader * ptBitmapInfoHeader=(BitmapInfoHeader*)ptData;
    ptData+=sizeof(BitmapInfoHeader);

    int size = ptBitmapInfoHeader->Height * ptBitmapInfoHeader->Width * (ptBitmapInfoHeader->BitCount/8);
    unsigned char * ptAlphaData = new unsigned char[size];

    ptData+=ptBitmapFileHeader->DataOffset - (sizeof(BitmapFileHeader) + sizeof(BitmapInfoHeader));


    for (int dst = 0; dst+3 < size; dst +=3)
    {
           // copy pixel data over
           ptAlphaData[dst] = ptData[dst+2];
           ptAlphaData[dst+1] = ptData[dst+1];
           ptAlphaData[dst+2] = ptData[dst];

    }

    tTexture *Tex = new tTexture;
    Tex->Format=GL_RGB;
 Tex->components = 3;
    Tex->Height=ptBitmapInfoHeader->Height;
    Tex->Width=ptBitmapInfoHeader->Width;
    Tex->ImageData=ptAlphaData;
    Tex->Type=GL_UNSIGNED_BYTE;

    delete Data;

  return Tex;

} // end CreateBMPTexture()

*/

tTexture * CreateTGATexture(unsigned char * Filename)
{

    ifstream fin;
    fin.open((char *)Filename,ios::binary);
    if (fin.fail())
        return false;
    fin.seekg(0,ios::end);
    int Filesize = fin.tellg();
    fin.seekg(0,ios::beg);


    unsigned char * Data = new unsigned char[Filesize];
    fin.read((char *)Data,Filesize);
    fin.close();





    unsigned char * ptData = Data;

    TargaFileHeader * ptTargaFileHeader = (TargaFileHeader*)ptData;
    ptData+=sizeof(TargaFileHeader) + ptTargaFileHeader->IDLength;

    unsigned char * ptTextureData=NULL;


    if (ptTargaFileHeader->ImageTypeCode == 3) //Black and White 1 bytepp image
    {

        unsigned int ImageDataSize = (ptTargaFileHeader->ImageWidth * ptTargaFileHeader->ImageHeight * 3);
        ptTextureData = new unsigned char[ImageDataSize];

        int width = ptTargaFileHeader->ImageWidth;
        int height = ptTargaFileHeader->ImageHeight;

        for (int row = 0; row < height; row ++)
        {
            for (int col = 0; col < width; col++)
            {
                ptTextureData[(row*width*3)+col*3]   = ptData[((height-1-row)*width) + col];
                ptTextureData[(row*width*3)+col*3+1] = ptData[((height-1-row)*width) + col];
                ptTextureData[(row*width*3)+col*3+2] = ptData[((height-1-row)*width) + col];
            }
        }

        tTexture *Tex = new tTexture;
        Tex->Format=GL_RGB;
        Tex->components=3;
        Tex->Height=ptTargaFileHeader->ImageHeight;
        Tex->Width=ptTargaFileHeader->ImageWidth;
        Tex->ImageData=ptTextureData;
        Tex->Type=GL_UNSIGNED_BYTE;

        delete Data;
        return Tex;

    }
    else if (ptTargaFileHeader->ImageTypeCode == 2) //uncompressed rgb or rgba
    {

        if (ptTargaFileHeader->BitCount/8 == 3) //RGB 24 BIT
        {

            unsigned int ImageDataSize = (ptTargaFileHeader->ImageWidth * ptTargaFileHeader->ImageHeight * 3);
            ptTextureData = new unsigned char[ImageDataSize];

            int width = ptTargaFileHeader->ImageWidth;
            int height = ptTargaFileHeader->ImageHeight;

            for (int row = 0; row < height; row ++)
            {

                for (int col = 0; (col+3) <= (width*3); col +=3)
                {

                    ptTextureData[(row*(width)*3)+ col]  = ptData[((height-1-row)*(width)*3) + col+2];
                    ptTextureData[(row*(width)*3)+col+1] = ptData[((height-1-row)*(width)*3) + col+1];
                    ptTextureData[(row*(width)*3)+col+2] = ptData[((height-1-row)*(width)*3) + col  ];
                }
            }

            tTexture *Tex = new tTexture;
            Tex->Format=GL_RGB;
            Tex->components=3;
            Tex->Height=ptTargaFileHeader->ImageHeight;
            Tex->Width=ptTargaFileHeader->ImageWidth;
            Tex->ImageData=ptTextureData;
            Tex->Type=GL_UNSIGNED_BYTE;

            delete Data;
            return Tex;

        }//end if 24 bit
        else if (ptTargaFileHeader->BitCount/8 == 4) //RGBA 32 BIT
        {

            unsigned int ImageDataSize = (ptTargaFileHeader->ImageWidth * ptTargaFileHeader->ImageHeight * 4);
            ptTextureData = new unsigned char[ImageDataSize];

            int width = ptTargaFileHeader->ImageWidth;
            int height = ptTargaFileHeader->ImageHeight;

            for (int row = 0; row < height; row ++)
            {
                for (int col = 0; (col+4) <= (width*4); col +=4)
                {
                    ptTextureData[(row*(width)*4)+ col]  = ptData[((height-1-row)*(width)*4) + col+2];
                    ptTextureData[(row*(width)*4)+col+1] = ptData[((height-1-row)*(width)*4) + col+1];
                    ptTextureData[(row*(width)*4)+col+2] = ptData[((height-1-row)*(width)*4) + col  ];
                    ptTextureData[(row*(width)*4)+col+3] = ptData[((height-1-row)*(width)*4) + col+3];
                }
            }

            tTexture *Tex = new tTexture;
            Tex->Format=GL_RGBA;
            Tex->components=4;
            Tex->Height=ptTargaFileHeader->ImageHeight;
            Tex->Width=ptTargaFileHeader->ImageWidth;
            Tex->ImageData=ptTextureData;
            Tex->Type=GL_UNSIGNED_BYTE;

            delete Data;
            return Tex;

        }//end if 32 bit
    }
    else if (ptTargaFileHeader->ImageTypeCode == 10)//RLE COMPRESSED RBA/RGBA
    {

        if (ptTargaFileHeader->BitCount/8 == 4)
        {
            unsigned int ImageDataSize = (ptTargaFileHeader->ImageWidth * ptTargaFileHeader->ImageHeight * 4);
            ptTextureData = new unsigned char[ImageDataSize];


            int width = ptTargaFileHeader->ImageWidth;
            int height = ptTargaFileHeader->ImageHeight;

            //load rle data
            int numprocessed = 0; //number of color byte sets processed
            unsigned char * header;
            int coutcount = 0;

            while (numprocessed < (ImageDataSize/4))
            {
                header = ptData;
                ptData+=sizeof(char);
                int rle = (int)(*header & 128);
                if (rle) rle = 1;
                int numpackets = *header - (128*rle) +1; //turn off leftmost bit to get number from remaining 7

                if (rle)
                {
                    for (int x = 0; x< numpackets; x++)
                    {
                        ptTextureData[(numprocessed*4)] = *(ptData+2);
                        ptTextureData[(numprocessed*4)+1] = *(ptData+1);
                        ptTextureData[(numprocessed*4)+2] = *ptData;
                        ptTextureData[(numprocessed*4)+3] = *(ptData+3);
                        numprocessed++;
                    }
                    ptData+= sizeof(char)*4;
                }
                else
                {
                    for (int x = 0; x< numpackets; x++)
                    {
                        ptTextureData[(numprocessed*4)] = *(ptData+2);
                        ptTextureData[(numprocessed*4)+1] = *(ptData+1);
                        ptTextureData[(numprocessed*4)+2] = *ptData;
                        ptTextureData[(numprocessed*4)+3] = *(ptData+3);
                        numprocessed++;

                        ptData+=sizeof(char)*4;
                    }
                }

            }//end while numprocessed

            //flip the tga over vertically so it's not upside down
            //int width = ptTargaFileHeader->ImageWidth;
            //int height = ptTargaFileHeader->ImageHeight;

            char * temprow;
            temprow = new char[width*4];
            int top = 0;
            int bottom = height-1;

            while (bottom > top)
            {
                memcpy(temprow, &ptTextureData[top * width*4], width*4);
                memcpy(&ptTextureData[top * width*4],&ptTextureData[bottom*width*4],width*4);
                memcpy(&ptTextureData[bottom * width*4],temprow,width*4);
                top++;
                bottom--;
            }

            delete[] temprow;

            tTexture * Tex = new tTexture;
            Tex->Format=GL_RGBA;
            Tex->components=4;
            Tex->Height=ptTargaFileHeader->ImageHeight;
            Tex->Width=ptTargaFileHeader->ImageWidth;
            Tex->ImageData=ptTextureData;
            Tex->Type=GL_UNSIGNED_BYTE;

            delete Data;
            return Tex;

        }//end if 32 bit
        else if (ptTargaFileHeader->BitCount/8 == 3)
        {

            unsigned int ImageDataSize = (ptTargaFileHeader->ImageWidth * ptTargaFileHeader->ImageHeight * 3);
            ptTextureData = new unsigned char[ImageDataSize];


            int width = ptTargaFileHeader->ImageWidth;
            int height = ptTargaFileHeader->ImageHeight;

            //load rle data
            int numprocessed = 0; //number of color byte sets processed
            unsigned char * header;
            int coutcount = 0;

            while (numprocessed < (ImageDataSize/3))
            {
                header = ptData;
                ptData+=sizeof(char);
                int rle = (int)(*header & 128);
                if (rle) rle = 1;
                int numpackets = *header - (128*rle) +1; //turn off leftmost bit to get number from remaining 7

                if (rle)
                {
                    for (int x = 0; x< numpackets; x++)
                    {
                        ptTextureData[(numprocessed*3)] = *(ptData+2);
                        ptTextureData[(numprocessed*3)+1] = *(ptData+1);
                        ptTextureData[(numprocessed*3)+2] = *ptData;
                        numprocessed++;
                    }
                    ptData+= sizeof(char)*3;
                }
                else
                {
                    for (int x = 0; x< numpackets; x++)
                    {
                        ptTextureData[(numprocessed*3)] = *(ptData+2);
                        ptTextureData[(numprocessed*3)+1] = *(ptData+1);
                        ptTextureData[(numprocessed*3)+2] = *ptData;
                        numprocessed++;

                        ptData+=sizeof(char)*3;
                    }
                }

            }//end while numprocessed

            //flip the tga over vertically so it's not upside down
            //int width = ptTargaFileHeader->ImageWidth;
            //int height = ptTargaFileHeader->ImageHeight;

            char * temprow;
            temprow = new char[width*3];
            int top = 0;
            int bottom = height-1;

            while (bottom > top)
            {
                memcpy(temprow, &ptTextureData[top * width*3], width*3);
                memcpy(&ptTextureData[top * width*3],&ptTextureData[bottom*width*3],width*3);
                memcpy(&ptTextureData[bottom * width*3],temprow,width*3);
                top++;
                bottom--;
            }

            delete[] temprow;

            tTexture * Tex = new tTexture;
            Tex->Format=GL_RGB;
            Tex->components=3;
            Tex->Height=ptTargaFileHeader->ImageHeight;
            Tex->Width=ptTargaFileHeader->ImageWidth;
            Tex->ImageData=ptTextureData;
            Tex->Type=GL_UNSIGNED_BYTE;

            delete Data;
            return Tex;

        }//end if 24 bit


    }//end if typecode == 10

    delete Data;
    return NULL;

} // end CreateTGATexture()



/*
///////////////////////////////// DECODE JPG \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
///// This decodes the jpeg and fills in the tImage structure
/////
///////////////////////////////// DECODE JPG \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void DecodeJPG(jpeg_decompress_struct* cinfo, tTexture *pImageData)
{
 // Read in the header of the jpeg file
 jpeg_read_header(cinfo, TRUE);

 // Start to decompress the jpeg file with our compression info
 jpeg_start_decompress(cinfo);

 // Get the image dimensions and channels to read in the pixel data
 if(cinfo->num_components == 3)
 {
  pImageData->Format = GL_RGB;
  pImageData->components = 3;
 }
    if(cinfo->num_components == 4)
 {
  pImageData->Format = GL_RGBA;
  pImageData->components = 4;
 }
 pImageData->Width    = cinfo->image_width;
 pImageData->Height    = cinfo->image_height;
    pImageData->Type = GL_UNSIGNED_BYTE;
 // Get the row span in bytes for each row
 int rowSpan = cinfo->image_width * cinfo->num_components;

 // Allocate memory for the pixel buffer
 pImageData->ImageData = new unsigned char[rowSpan* pImageData->Height];//((unsigned char*)malloc(sizeof(unsigned char)*rowSpan*pImageData->sizeY));

 // Create an array of row pointers
 unsigned char** rowPtr = new unsigned char*[pImageData->Height];

 for (int i = 0; i < pImageData->Height; i++)
  rowPtr[i] = &(pImageData->ImageData[i * rowSpan]);

 // Now comes the juice of our work, here we extract all the pixel data
 int rowsRead = 0;
 while (cinfo->output_scanline < cinfo->output_height)
 {
  // Read in the current row of pixels and increase the rowsRead count
  rowsRead += jpeg_read_scanlines(cinfo,
          &rowPtr[rowsRead], cinfo->output_height - rowsRead);
 }

 // Delete the temporary row pointers
 delete [] rowPtr;

 // Finish decompressing the data
 jpeg_finish_decompress(cinfo);
}


///////////////////////////////// LOAD JPG \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
///// This loads the JPG file and returns it's data in a tImage struct
/////
///////////////////////////////// LOAD JPG \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

tTexture *CreateJPGTexture( char *strFileName)
{
 struct jpeg_decompress_struct cinfo;
 tTexture *pImageData = NULL;
 FILE *pFile;

 // Open a file pointer to the jpeg file and check if it was found and opened
 if((pFile = fopen(strFileName, "rb")) == NULL)
   return NULL;


 // Create an error handler
 jpeg_error_mgr jerr;

 // Have our compression info object point to the error handler address
 cinfo.err = jpeg_std_error(&jerr);

 // Initialize the decompression object
 jpeg_create_decompress(&cinfo);

 // Specify the data source (Our file pointer)
 jpeg_stdio_src(&cinfo, pFile);

 // Allocate the structure that will hold our eventual jpeg data (must free it!)
 pImageData = new tTexture;//(tImage*)malloc(sizeof(tImage));

 // Decode the jpeg file and fill in the image data structure to pass back
 DecodeJPG(&cinfo, pImageData);

 // This releases all the stored memory for reading and decoding the jpeg
 jpeg_destroy_decompress(&cinfo);

 // Close the file pointer that opened the file
 fclose(pFile);


 // Return the jpeg data (remember, you must free this data after you are done)
 return pImageData;
}

*/
