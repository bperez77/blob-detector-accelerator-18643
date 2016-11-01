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

#include <hls_stream.h>         // Definition of the stream class

#include "axis.h"               // Definition of the AXIS protocol structure

// The number of rows and columns in the images being processed
// TODO: Eventually move into a different header file
const int IMAGE_ROWS = 1080;
const int IMAGE_COLS = 1920;

/* The number of bits needed to represent a color channel, and the total number
 * of bits needed to represent a pixel, as four color channels. */
const int COLOR_DEPTH = 8;
const int PIXEL_BITS = 4 * COLOR_DEPTH;

// The number of ROWS and COLUMNS in our image

// Represents a pixel in an image, as its RGBA components
typedef struct pixel {
    ap_int<COLOR_DEPTH> red;
    ap_int<COLOR_DEPTH> blue;
    ap_int<COLOR_DEPTH> green;
    ap_int<COLOR_DEPTH> alpha;
} pixel_t;

// The RGB AXIS packet, and its stream variant
typedef axis<pixel_t, PIXEL_BITS> pixel_axis_t;
typedef hls::stream<pixel_axis_t> pixel_stream_t;

// The grayscale AXIS packet, and its stream variant
typedef ap_uint<COLOR_DEPTH> grayscale_t;
typedef axis<grayscale_t, COLOR_DEPTH> grayscale_axis_t;
typedef hls::stream<grayscale_axis_t> grayscale_stream_t;

/**
 * Converts the RGB input stream into its grayscale value, by taking the average
 * of the three RGB channels. The image being processed has dimensions
 * ROWSxCOLS.
 *
 * @tparam ROWS The number of rows in the image being streamed in.
 * @tparam COLS The number of columns in the image being stream in.
 * @param[in] pixel_stream The input stream of pixels, as RGBA data.
 * @param[out] gray_stream The output stream of grayscale values
 **/
void grayscale(pixel_stream_t& rgba_stream,
        grayscale_stream_t& grayscale_stream);

#endif /* GRAYSCALE_H_ */
