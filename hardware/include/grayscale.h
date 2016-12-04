/**
 * @file grayscale.h
 * @date Sunday, October 30, 2016 at 04:59:10 PM EDT
 * @author Brandon Perez (bmperez)
 * @author Devon White (dww)
 * @author Yiyi Zhang (yiyiz)
 *
 * This file contains the interface to the grayscale module.
 *
 * This defines the grayscale module interface, as both a streaming sequential
 * interface, and a combinational interface. It also defines the input stream,
 * and output data types for the module.
 *
 * @bug No known bugs.
 **/

#ifndef GRAYSCALE_H_
#define GRAYSCALE_H_

#include <hls_stream.h>             // Definition of the hls::stream class
#include <ap_int.h>                 // Arbitrary precision integer types

#include "image.h"                  // Definition of the image format
#include "axis.h"                   // Definition of the AXIS protocol structure

/*----------------------------------------------------------------------------
 * Defintions
 *----------------------------------------------------------------------------*/

/**
 * The input stream type, an RGBA pixel AXIS packet.
 **/
typedef axis<pixel_t, PIXEL_BITS> pixel_axis_t;
typedef hls::stream<pixel_axis_t> pixel_stream_t;

/**
 * The output stream type, a grayscale value AXIS packet.
 **/
typedef ap_uint<COLOR_DEPTH> grayscale_t;
typedef axis<grayscale_t, COLOR_DEPTH> grayscale_axis_t;
typedef hls::stream<grayscale_axis_t> grayscale_stream_t;

/*----------------------------------------------------------------------------
 * Interface
 *----------------------------------------------------------------------------*/

/**
 * Converts the RGBA pixel into its grayscale, by taking the average of its RGB
 * channels.
 *
 * This is the combinational interface to the module
 *
 * @param[in] pixel The pixel to convert to grayscale.
 * @return The 8-bit grayscale (intensity) value of the pixel.
 **/
grayscale_t compute_grayscale(const pixel_t& pixel);

/**
 * Converts the RGBA input stream into its grayscale value, by taking the
 * average of the three RGB channels.
 *
 * This is the sequential interface for the module.
 *
 * @param[in] pixel_stream The input stream of pixels, as RGBA data.
 * @param[out] grayscale_stream The output stream of grayscale values
 **/
void grayscale(pixel_stream_t& pixel_stream,
        grayscale_stream_t& grayscale_mono_stream,
        grayscale_stream_t& grayscale_down_stream);

#endif /* GRAYSCALE_H_ */
