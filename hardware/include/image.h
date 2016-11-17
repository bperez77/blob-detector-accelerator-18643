/**
 * @file image.h
 * @date Thursday, November 17, 2016 at 01:20:33 AM EST
 * @author Brandon Perez (bmperez)
 *
 * This file contains definitions for an image.
 *
 * This file has the definition for the size of the image in the system, as well
 * as the format of the input images (their pixels).
 *
 * @bug No known bugs.
 **/

#ifndef IMAGE_H_
#define IMAGE_H_

#include <ap_int.h>                 // Arbitrary precision integer types

/*----------------------------------------------------------------------------
 * Parameters
 *----------------------------------------------------------------------------*/

// The number of rows and columns in the image being processed (1080p)
const int IMAGE_WIDTH               = 1920;
const int IMAGE_HEIGHT              = 1080;

// The number of bits needed to represent a color channel
const int COLOR_DEPTH = 8;

/*----------------------------------------------------------------------------
 * Image Format (Pixel) Definition
 *----------------------------------------------------------------------------*/

// The number of bits needed to represent a pixel, as four color channels
const int NUM_COLOR_CHANNELS        = 4;
const int PIXEL_BITS                = 4 * COLOR_DEPTH;

/**
 * The format of a pixel in an image, which consists of four color channels:
 * red, green, blue, and alpha. The total size is 32-bits, with alpha being the
 * most-signficiant byte, and red being the least significant byte.
 **/
typedef struct pixel {
    ap_uint<COLOR_DEPTH> red;       // Red channel of the pixel
    ap_uint<COLOR_DEPTH> blue;      // Blue channel of the pixel
    ap_uint<COLOR_DEPTH> green;     // Green channel of the pixel
    ap_uint<COLOR_DEPTH> alpha;     // Alpha channel of the pixel
} pixel_t;

#endif /* IMAGE_H_ */
