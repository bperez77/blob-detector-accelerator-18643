/**
 * @file grayscale.cpp
 * @date Sunday, October 30, 2016 at 04:58:04 PM EDT
 * @author Brandon Perez (bmperez)
 * @author Devon White (dww)
 * @author Yiyi Zhang (yiyiz)
 *
 * This file contains the implementation of the grayscale module.
 *
 * The grayscale module simply takes a 32-bit input stream, consisting of four
 * channels, red, green, blue, and alpha, and converts it to a 8-bit intensity,
 * or grayscale, value.
 *
 * @bug No known bugs.
 **/

#include <ap_int.h>         // Arbitrary precision integer types

#include "axis.h"           // Definition of the AXIS protocol structure
#include "grayscale.h"      // Our interface and grayscale types

/*----------------------------------------------------------------------------
 * Grayscale Module
 *----------------------------------------------------------------------------*/

/**
 * Converts the image pixel to grayscale, simply taking the average of its 3
 * grayscale channels. This is the combinational interface.
 **/
grayscale_t compute_grayscale(const pixel_t& pixel) {
#pragma HLS INLINE

    // Convert the RGB channels to 10-bit values to prevent overflow
    ap_int<COLOR_DEPTH+2> red = pixel.red;
    ap_int<COLOR_DEPTH+2> blue = pixel.blue;
    ap_int<COLOR_DEPTH+2> green = pixel.green;

    return (red + blue + green) / 3;
}

/**
 * Converts the stream of pixels into grayscale, using the grayscale function.
 * This is the sequential interface for the module.
 **/
void grayscale(pixel_stream_t& pixel_stream,
		grayscale_stream_t& grayscale_stream) {
#pragma HLS INLINE

    // Read in the next image pixel packet
    pixel_axis_t pixel_axis_pkt;
    pixel_stream >> pixel_axis_pkt;

    // Compute the grayscale value, and send the value downstream
    grayscale_axis_t grayscale_axis_pkt;
    grayscale_axis_pkt.tdata = compute_grayscale(pixel_axis_pkt.tdata);

    /* Our transfers are always aligned, so set tkeep to -1, and assert
     * tlast when we reach the last packet. */
    grayscale_axis_pkt.tkeep = -1;
    grayscale_axis_pkt.tlast = pixel_axis_pkt.tlast;

    // Stream out the grayscale packet
    grayscale_stream << grayscale_axis_pkt;
    return;
}

/*----------------------------------------------------------------------------
 * Top Function for Synthesis
 *----------------------------------------------------------------------------*/

/**
 * The top-level function for the grayscale module. This is what gets exported
 * as the IP. Converts the input RGBA image stream to a grayscale stream.
 **/
void grayscale_top(pixel_stream_t& pixel_stream,
        grayscale_stream_t& grayscale_stream) {
#pragma HLS INTERFACE axis port=pixel_stream
#pragma HLS INTERFACE axis port=grayscale_stream

#pragma HLS PIPELINE II=1 rewind

    // Instantiate the grayscale module
    grayscale(pixel_stream, grayscale_stream);
    return;
}
