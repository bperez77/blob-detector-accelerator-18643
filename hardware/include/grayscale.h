/**
 * @file grayscale.h
 * @date Sunday, October 30, 2016 at 04:59:10 PM EDT
 * @author Brandon Perez (bmperez)
 * @author Devon White (dww)
 * @author Yiyi Zhang (yiyiz)
 *
 * @bug No known bugs.
 **/

#ifndef GRAYSCALE_H_
#define GRAYSCALE_H_

#include <hls_stream.h>             // Definition of the hls::stream class
#include <ap_int.h>                 // Arbitrary precision integer types

#include "axis.h"                   // Definition of the AXIS protocol structure

/*----------------------------------------------------------------------------
 * Defintions
 *----------------------------------------------------------------------------*/

// The number of rows and columns in the images being processed
// TODO: Eventually move into a different header file
const int IMAGE_ROWS = 1080;
const int IMAGE_COLS = 1920;

/* The number of bits needed to represent a color channel, and the total number
 * of bits needed to represent a pixel, as four color channels. */
const int COLOR_DEPTH = 8;
const int PIXEL_BITS = 4 * COLOR_DEPTH;

// Represents a pixel in an image, as its RGBA components
typedef struct pixel {
    ap_uint<COLOR_DEPTH> red;       // Red channel of the pixel
    ap_uint<COLOR_DEPTH> blue;      // Blue channel of the pixel
    ap_uint<COLOR_DEPTH> green;     // Green channel of the pixel
    ap_uint<COLOR_DEPTH> alpha;     // Alpha channel of the pixel
} pixel_t;

// The RGB AXIS packet, and its stream variant
typedef axis<pixel_t, PIXEL_BITS> pixel_axis_t;
typedef hls::stream<pixel_axis_t> pixel_stream_t;

// The grayscale AXIS packet, and its stream variant
typedef ap_uint<COLOR_DEPTH> grayscale_t;
typedef axis<grayscale_t, COLOR_DEPTH> grayscale_axis_t;
typedef hls::stream<grayscale_axis_t> grayscale_stream_t;

/*----------------------------------------------------------------------------
 * Interface
 *----------------------------------------------------------------------------*/

/**
 * Converts the RGB input stream into its grayscale value, by taking the average
 * of the three RGB channels.
 *
 * @param[in] pixel_stream The input stream of pixels, as RGBA data.
 * @param[out] grayscale_stream The output stream of grayscale values
 **/
template <typename IN_T, typename OUT_T>
void grayscale(pixel_stream_t& pixel_stream,
        grayscale_stream_t& grayscale_stream);

#endif /* GRAYSCALE_H_ */
