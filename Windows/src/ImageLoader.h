/*
 * ImageLoader.h
 *
 * An image loader designed to read a 32-bit bitmap. Although it may be extended to be
 * do other things too in the future.
 *
 *  Created on: 2010-08-11
 *      Author: Michael Yagudaev
 *      Copyright: yagudaev.com
 *      Version: $0.1.2$
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License Version 3 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * ---------------------------------------------------------------------
 * FIX (MSYS2/MinGW compile error: "conflicting declaration 'typedef long
 * int LONG'"): this header used to define its own LONG/WORD/DWORD/BYTE
 * typedefs and BITMAPFILEHEADER/BITMAPINFOHEADER/RGBQUAD structs, which
 * are also names windows.h (pulled in transitively via GL/glut.h on
 * MSYS2) defines. An earlier attempt guarded these with
 * "#ifndef _WINDOWS_", but that only works if windows.h happens to be
 * processed before this header in every translation unit -- a fragile,
 * include-order-dependent assumption that broke as soon as the order
 * changed.
 *
 * Fix: all Windows-reserved names have been replaced with uniquely
 * prefixed ones (IL_BYTE, IL_WORD, IL_DWORD, IL_LONG, and
 * ImgBitmapFileHeader / ImgBitmapInfoHeader / ImgRGBQuad) built on
 * fixed-width <cstdint> types. These can never collide with windows.h,
 * regardless of include order, on any platform. ImageLoader.cpp has
 * been updated to match.
 * ---------------------------------------------------------------------
 */

#ifndef IMAGELOADER_H_
#define IMAGELOADER_H_

#include <cstdint>

// Fixed-width types used for BMP parsing. Uniquely named (IL_ prefix) so
// they can never collide with windows.h's WORD/DWORD/BYTE/LONG, no matter
// what order headers are included in.
typedef uint8_t  IL_BYTE;
typedef int32_t  IL_LONG;
typedef uint32_t IL_DWORD;
typedef uint16_t IL_WORD;

//File information header
//provides general information about the file
typedef struct __attribute__ ((__packed__)) tagImgBitmapFileHeader
{
  IL_WORD    bfType;
  IL_DWORD   bfSize;
  IL_WORD    bfReserved1;
  IL_WORD    bfReserved2;
  IL_DWORD   bfOffBits;
} ImgBitmapFileHeader, *PImgBitmapFileHeader;

//Bitmap information header
//provides information specific to the image data
typedef struct __attribute__ ((__packed__)) tagImgBitmapInfoHeader
{
  IL_DWORD  biSize;
  IL_LONG   biWidth;
  IL_LONG   biHeight;
  IL_WORD   biPlanes;
  IL_WORD   biBitCount;
  IL_DWORD  biCompression;
  IL_DWORD  biSizeImage;
  IL_DWORD  biXPelsPerMeter;
  IL_DWORD  biYPelsPerMeter;
  IL_DWORD  biClrUsed;
  IL_DWORD  biClrImportant;
} ImgBitmapInfoHeader, *PImgBitmapInfoHeader;

//Colour palette
typedef struct __attribute__ ((__packed__)) tagImgRGBQuad
{
  IL_BYTE    rgbBlue;
  IL_BYTE    rgbGreen;
  IL_BYTE    rgbRed;
  IL_BYTE    rgbReserved;
} ImgRGBQuad;


class ImageLoader
{
public:
    //variables

    //methods

    /**
     * Initializes an empty image
     */
    ImageLoader(void);

    /**
     * Initializes an image with the given image loaded from disk
     */
    ImageLoader(const char *fileName);

    /**
     * Destructor...
     */
    virtual ~ImageLoader();

    /**
     * Loads the given image.
     * @return True on success false on failure
     */
    bool loadBMP(const char *fileName);

    /**
     * Get the alpha channel as an array of bytes
     * @param size The size of the returned array, will return -1 on failure
     */
    IL_BYTE *getAlpha() const;

    // Getter and setters...
    IL_LONG getHeight() const
    {
        return height;
    }

    ImgRGBQuad *getColors() const
    {
        return colors;
    }

    bool getLoaded() const
    {
        return loaded;
    }

    IL_BYTE *getPixelData() const
    {
        return pixelData;
    }

    IL_LONG getWidth() const
    {
        return width;
    }

private:
    //variables
    ImgBitmapFileHeader bmfh;
    ImgBitmapInfoHeader bmih;
    ImgRGBQuad *colors;
    IL_BYTE *pixelData;
    bool loaded;
    IL_LONG width;
    IL_LONG height;
    IL_WORD bpp;

    //methods
    void reset(void);
    bool fixPadding(IL_BYTE const * const tempPixelData, IL_DWORD size);
};
#endif /* IMAGELOADER_H_ */
