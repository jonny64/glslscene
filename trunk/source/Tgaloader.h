# pragma once
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

class Texture  
{
  // GLubyte will work for images up to 32-bits.
  GLubyte *imageData;

  // Stores the color depth of the image in bits
  // per pixel.

  GLuint   bpp;
  GLuint   width;
  GLuint   height;
  // Header data.
  GLubyte    header[6];
  GLuint   texID;     // For texture objects;

  bool LoadTGA(const char * filename)
  {

    GLubyte    targaMagic[12] = 
    { 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

    GLubyte    fileMagic[12];


    GLuint     bytesPerPixel;

    GLuint     imageSize;
    GLuint     tmp;
    GLuint     i;

    FILE *file = fopen(filename, "rb");

    // Start reading in the header, make sure we
    // have the right Targa data type.  

    if(file == NULL)
    {
      texID = -1;
      return false;
    }

    if(fread(fileMagic, 1, sizeof(fileMagic), file) != sizeof(fileMagic) ||
      memcmp(targaMagic, fileMagic, sizeof(targaMagic)) != 0 ||
      fread(header, 1, sizeof(header), file) != sizeof(header))
    {
      texID = -1;
      fclose(file);
      return false;
    }

    // Determine width and height from header data.
    width  = header[1] * 256 + header[0];
    height = header[3] * 256 + header[2];

    // Determine color bit depth of the image (24 or 32).

    if(width <= 0 || height <= 0 || (header[4] != 24 && header[4] != 32))
    {
      texID = -1;
      fclose(file);
      return false;
    }

    bpp  = header[4];
    bytesPerPixel = bpp / 8;
    imageSize     = width * height * bytesPerPixel;

    imageData = (GLubyte *)malloc(imageSize);

    if(imageData == NULL ||
      (int)fread(imageData, 1, imageSize, file) != imageSize)
    {
      if(imageData != NULL)
        free(imageData);

      texID = -1;
      fclose(file);
      return false;
    } 

    for(i = 0; i < imageSize; i += bytesPerPixel)
    {
      tmp = imageData[i];
      imageData[i] = imageData[i + 2];
      imageData[i + 2] = tmp;
    }

    fclose (file);
    return true;
  }

  bool Build()
  {
    glGenTextures( 1, &texID );

    glActiveTextureARB(GL_TEXTURE0_ARB);
    glBindTexture( GL_TEXTURE_2D, texID );

    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_REPEAT);


    gluBuild2DMipmaps( 
      GL_TEXTURE_2D,
      header[4]/8,
      width, 
      height,
      (header[4] == 24)? GL_RGB : GL_RGBA,
      GL_UNSIGNED_BYTE, 
      imageData 
      );

    free( imageData );

    return true;
  }


public:
  bool Load(const char *filename)
  {
    if (!LoadTGA(filename)) 
      return false;
    if (!Build()) 
      return false;
    return true;
  }

  GLuint getID()
  {
    return texID;
  }
  Texture()
  {
    imageData= NULL;
    bpp= 0;
    width= 0;
    height= 0;
    texID=-1;
  }

  virtual ~Texture()
  {
    //if( texID != -1 )
    //  glDeleteTextures( 1, &texID );
  };
};