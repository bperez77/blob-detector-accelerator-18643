/**
 * @file grayscale.cpp
 * @date Sunday, October 30, 2016 at 04:58:04 PM EDT
 * @author Brandon Perez (bmperez)
 * @author Devon White (dww)
 * @author Yiyi Zhang (yiyiz)
 *
 * @bug No known bugs.
 **/

#include <ap_int.h>         // Arbitrary precision integers type and functions

#include "grayscale.h"      // Grayscale interface and definitions

/**
 * Converts the image pixel to grayscale, simply taking the average of its 3
 * grayscale channels.
 **/
static grayscale_t compute_grayscale(const pixel_t& pixel)
{
    return (pixel.red + pixel.blue + pixel.green) / 3;
}

/**
 * The main grayscale function for the module. This is what gets invoked by the
 * testbench, and would be templated on the image size if necessary. Converts
 * the input RGBA image stream to a grayscale stream.
 **/
void grayscale(pixel_stream_t& pixel_stream,
		grayscale_stream_t& grayscale_stream) {
#pragma HLS PIPELINE

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

/**
 * The top-level function for the grayscale module. This is what gets exported
 * as the IP. Converts the input RGBA image stream to a grayscale stream.
 **/
void grayscale_top(pixel_stream_t& pixel_stream,
        grayscale_stream_t& grayscale_stream) {
#pragma HLS INTERFACE axis port=pixel_stream
#pragma HLS INTERFACE axis port=grayscale_stream

    // Instantiate the grayscale module
    grayscale(pixel_stream, grayscale_stream);
    return;
}
