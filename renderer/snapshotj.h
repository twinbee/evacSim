/*********************************************************
** snapshotj.h
** Primary author: Matthew B
** Description: JPEG snapshots, using libjpeg. int Quality is 1-100
**   code adapted from web
*********************************************************/

#ifndef SNAPSHOT_H
#define SNAPSHOT_H


extern "C" {

#include <jpeglib.h>

}



bool screenshot(unsigned int width, unsigned int height, char *path, int quality)

{

    bool ret=false;



    struct jpeg_compress_struct cinfo; // the JPEG OBJECT

    struct jpeg_error_mgr jerr; // error handler struct

    unsigned char *row_pointer[1]; // pointer to JSAMPLE row[s]

    GLubyte *pixels=0, *flip=0;

    FILE *shot;

    int row_stride; // width of row in image buffer



    if ((shot=fopen(path, "wb"))!=NULL) { // jpeg file

        // initializatoin

        cinfo.err = jpeg_std_error(&jerr); // error handler

        jpeg_create_compress(&cinfo); // compression object

        jpeg_stdio_dest(&cinfo, shot); // tie stdio object to JPEG object

        row_stride = width * 3;


        pixels = new GLubyte [width*height*3];
        flip = new GLubyte [width*height*3];



        if (pixels!=NULL && flip!=NULL) {

            // save the screen shot into the buffer

            //glReadBuffer(GL_BACK_LEFT);

            glPixelStorei(GL_PACK_ALIGNMENT, 1);

            glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);



            // give some specifications about the image to save to libjpeg

            cinfo.image_width = width;

            cinfo.image_height = height;

            cinfo.input_components = 3; // 3 for R, G, B

            cinfo.in_color_space = JCS_RGB; // type of image



            jpeg_set_defaults(&cinfo);

            jpeg_set_quality(&cinfo, quality, TRUE);

            jpeg_start_compress(&cinfo, TRUE);



            // OpenGL writes from bottom to top.

            // libjpeg goes from top to bottom.

            // flip lines.

            for (int y=0;y<height;y++) {

                for (int x=0;x<width;x++) {

                    flip[(y*width+x)*3] = pixels[((height-1-y)*width+x)*3];

                    flip[(y*width+x)*3+1] = pixels[((height-1-y)*width+x)*3+1];

                    flip[(y*width+x)*3+2] = pixels[((height-1-y)*width+x)*3+2];

                }

            }



            // write the lines

            while (cinfo.next_scanline < cinfo.image_height) {

                row_pointer[0] = &flip[cinfo.next_scanline * row_stride];

                jpeg_write_scanlines(&cinfo, row_pointer, 1);

            }



            ret=true;

            // finish up and free resources

            jpeg_finish_compress(&cinfo);

            jpeg_destroy_compress(&cinfo);

        }

        fclose(shot);

    }



    if (pixels!=0) delete[] pixels;



    if (flip!=0) delete[] flip;




    return ret;

}



int WindowDump(int width, int height)
{
    bool stereo = false;
    int i,j;
    FILE *fptr;
    static int counter = 0;

    char fname[32];

    unsigned char *image;

    // Allocate our buffer for the image
    image = new unsigned char[3*width*height];

    glPixelStorei(GL_PACK_ALIGNMENT,1);

    // Open the file
    if (stereo)
        sprintf(fname,"L_%04d.raw",counter);
    else
        sprintf(fname,"C_%04d.raw",counter);
    if ((fptr = fopen(fname,"w")) == NULL) {
        fprintf(stderr,"Failed to open file for window dump\n");
        return(FALSE);
    }

    // Copy the image into our buffer
    glReadBuffer(GL_BACK_LEFT);
    glReadPixels(0,0,width,height,GL_RGB,GL_UNSIGNED_BYTE,image);

    // Write the raw file
    for (j=height-1;j>=0;j--) {
        for (i=0;i<width;i++) {
            fputc(image[3*j*width+3*i+0],fptr);
            fputc(image[3*j*width+3*i+1],fptr);
            fputc(image[3*j*width+3*i+2],fptr);
        }
    }
    fclose(fptr);

    if (stereo) {
        sprintf(fname,"R_%04d.raw",counter);
        if ((fptr = fopen(fname,"w")) == NULL) {
            fprintf(stderr,"Failed to open file for window dump\n");
            return(FALSE);
        }

        // Copy the image into our buffer
        glReadBuffer(GL_BACK_RIGHT);
        glReadPixels(0,0,width,height,GL_RGB,GL_UNSIGNED_BYTE,image);

        // Write the raw file
        for (j=height-1;j>=0;j--) {
            for (i=0;i<width;i++) {
                fputc(image[3*j*width+3*i+0],fptr);
                fputc(image[3*j*width+3*i+1],fptr);
                fputc(image[3*j*width+3*i+2],fptr);
            }
        }
        fclose(fptr);
    }

    counter++;
    //free(image);
    delete[] image;
    return(TRUE);
}


#endif
