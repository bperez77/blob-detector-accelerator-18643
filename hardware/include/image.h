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

#include <ap_int.h>                         // Arbitrary precision integer types

/*----------------------------------------------------------------------------
 * Image Format Parameters
 *----------------------------------------------------------------------------*/

/**
 * The dimensions of the image being processed, which is a 1080p image.
 **/
#ifdef __SYNTHESIS__
static const int IMAGE_WIDTH                = 1920;
static const int IMAGE_HEIGHT               = 1080;
#else
static const int IMAGE_WIDTH                = 32;
static const int IMAGE_HEIGHT               = 32;
#endif /* BLOB_DETECTION_DEBUG */

/**
 * The number of bits needed to represent a color channel in the image.
 **/
static const int COLOR_DEPTH                = 8;

/**
 * The number of bits needed to represent a pixel, which consists of four color
 * channels (RGBA)
 **/
static const int NUM_COLOR_CHANNELS         = 4;
static const int PIXEL_BITS                 = 4 * COLOR_DEPTH;

/*----------------------------------------------------------------------------
 * Image Format (Pixel) Definition
 *----------------------------------------------------------------------------*/

/**
 * Template for a generic structure representing a pixel in an image, consisting
 * of four color channels of a specified depth (number of bits).
 *
 * This structure defines a type `color_t` that represents a color channel. The
 * alpha channel is the most significant byte, while the red channel is the
 * least significant byte.
 *
 * @tparam COLOR_DEPTH The number of bits needed to represent a color channel.
 **/
template <int COLOR_DEPTH>
struct pixel {
    typedef ap_uint<COLOR_DEPTH> color_t;   // Definition of a color channel

    color_t red;                            // Red channel of the pixel
    color_t green;                          // Green channel of the pixel
    color_t blue;                           // Blue channel of the pixel
    color_t alpha;                          // Alpha channel of the pixel

    // Default constructor for the pixel
    pixel() {}

    // Constructor for the pixel from constant integer values
    pixel(int red, int green, int blue, int alpha) {
        this->red = red;
        this->green = green;
        this->blue = blue;
        this->alpha = alpha;
    }

    // Constructor for the pixel from color_t (ap_uint) values
    pixel(color_t red, color_t green, color_t blue, color_t alpha) {
        this->red = red;
        this->green = green;
        this->blue = blue;
        this->alpha = alpha;
    }
};

/**
 * The definition for the format (pixel) of the image, which is a 32-bit word,
 * consisting of 8-bit RGBA color channels.
 **/
typedef pixel<COLOR_DEPTH> pixel_t;

#endif /* IMAGE_H_ */
