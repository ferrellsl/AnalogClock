/*
 * ImageLoader.cpp
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
 * FIX (MSYS2/MinGW type-redefinition conflicts): updated to use the
 * renamed IL_BYTE / IL_LONG / IL_DWORD / IL_WORD types and
 * ImgBitmapFileHeader / ImgBitmapInfoHeader / ImgRGBQuad structs from
 * the updated ImageLoader.h, instead of BYTE / LONG / DWORD / WORD /
 * BITMAPFILEHEADER / BITMAPINFOHEADER / RGBQUAD, which collided with
 * windows.h on MSYS2/MinGW builds. Behavior is unchanged -- this is a
 * pure rename, no logic was altered.
 * ---------------------------------------------------------------------
 */
 #define WIN32_LEAN_AND_MEAN
 #define NOMINMAX
#include "ImageLoader.h"
#include <cstdio>
#include <cstring>
#include <errno.h>
#define BITMAP_TYPE 19778

ImageLoader::ImageLoader()
{
	reset();
}

ImageLoader::ImageLoader(const char *fileName)
{
	reset();
	loadBMP(fileName);
}

ImageLoader::~ImageLoader()
{
    if(colors != NULL)
    {
        delete[] colors;
    }

    if(pixelData != NULL)
    {
        delete[] pixelData;
    }
}



bool ImageLoader::loadBMP(const char * fileName)
{
	FILE *in = NULL;
	bool result = false;

	//open the file for reading in binary mode
	in = fopen(fileName, "rb");
	if(in == NULL)
	{
		perror("Error");
		printf("errno = %d\n", errno);
		return false;
	}

	fread(&bmfh, sizeof(ImgBitmapFileHeader), 1, in);

	// check if this is even the right type of file
	if(bmfh.bfType != BITMAP_TYPE)
	{
		perror("Error");
		printf("errno = %d\n", errno);
		return false;
	}

	fread(&bmih,sizeof(ImgBitmapInfoHeader),1,in);
	width = bmih.biWidth;
	height = bmih.biHeight;
	bpp = bmih.biBitCount;

	// TODO: make this work for 24bit images as well! It will segfault if given a 24 bit image...
	//set the number of colours
	IL_LONG numColors = 1 << bmih.biBitCount;

    //bitmap is not loaded yet
    loaded = false;
    //make sure memory is not lost
    if(colors != NULL)
    {
        delete[] colors;
    }

    if(pixelData != NULL)
    {
        delete[] pixelData;
    }

	//load the palette for 8 bits per pixel
	if(bmih.biBitCount < 24)
	{
	    colors = new ImgRGBQuad[numColors];
	    fread(colors, sizeof(ImgRGBQuad), numColors, in);
	}

	IL_DWORD size = bmfh.bfSize - bmfh.bfOffBits;

	IL_BYTE *tempPixelData = NULL;

	tempPixelData = new IL_BYTE[size];

	if(tempPixelData == NULL)
	{
		printf("Error: out of memory. Cannot find space to load image into memory.\n");
	    fclose(in);
	    return false;
	}

	fread(tempPixelData, sizeof(IL_BYTE), size, in);

	result = fixPadding(tempPixelData, size);
	loaded = result;

	delete[] tempPixelData;
	fclose(in);

	return result;
}

bool ImageLoader::fixPadding(IL_BYTE const * const tempPixelData, IL_DWORD size)
{
	//byteWidth is the width of the actual image in bytes
	//padWidth is the width of the image plus the extra padding
	IL_LONG byteWidth, padWidth;

	//initially set both to the width of the image
	byteWidth = padWidth = (IL_LONG)((float)width * (float)bpp / 8.0);

	//add any extra space to bring each line to a DWORD boundary
	short padding = padWidth % 4 != 0;
	padWidth += padding;

	IL_DWORD diff;
	int offset;

	height = bmih.biHeight;
	//set diff to the actual image size(no padding)
	diff = height * byteWidth;
	//allocate memory for the image
	pixelData = new IL_BYTE[diff];

	if(pixelData == NULL)
	{
	    return false;
	}

	//bitmap is inverted, so the padding needs to be removed
	//and the image reversed
	//Here you can start from the back of the file or the front,
	//after the header.  The only problem is that some programs
	//will pad not only the data, but also the file size to
	//be divisible by 4 bytes.
	if(height > 0)
	{
	    offset = padWidth - byteWidth;

	    for(unsigned int i = 0; i < size - 2; i += 4)
	    {
	        if( (i + 1) % padWidth == 0)
	        {
	            i += offset;
	        }

	        // swap data for it to have the right order
	        *(pixelData + i)      = *(tempPixelData + i + 2); // R
	        *(pixelData + i + 1 ) = *(tempPixelData + i + 1); // G
	        *(pixelData + i + 2)  = *(tempPixelData + i);     // B
	        *(pixelData + i + 3)  = *(tempPixelData + i + 3); // A
	    }
	}
	//the image is not reversed.  Only the padding needs to be removed.
	else
	{
		// TODO: test this branch!
	    height = height * -1;
	    offset = 0;

	    do
	    {
	    	memcpy((pixelData + (offset * byteWidth)), (tempPixelData + (offset * padWidth)), byteWidth);
	        offset++;
	    } while(offset < height);
	}

	return true;
}

void ImageLoader::reset(void)
{
	width = 0;
	height = 0;
	pixelData = NULL;
	colors = NULL;
	loaded = false;
}

IL_BYTE *ImageLoader::getAlpha() const
{
	IL_LONG arraySize = width * height;
	IL_BYTE *array = new IL_BYTE[arraySize];

	if(array == NULL)
	{
		delete[] array;
		return NULL;
	}

	for(long i = 0; i < arraySize; i++)
	{
		array[i] = pixelData[i * 4 + 3]; // jump to the alpha and extract it everytime
	}

	return array;
}
