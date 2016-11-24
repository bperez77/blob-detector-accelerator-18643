/**
 * @file downscale.cpp
 * @date Wednesday, November 23, 2016 at 05:23:16 PM EST
 * @author Brandon Perez (bmperez)
 *
 * This file contains the implementation of the downscale module.
 *
 * The grayscale module simply takes a 32-bit input stream, consisting
 * FIXME: Insert a short description here
 *
 * FIXME: Insert a longer description here
 *
 * @bug No known bugs.
 **/

#include "grayscale.h"              // Definition of the grayscale types
#include "downscale.h"              // Our interface and downscale definitions

/*----------------------------------------------------------------------------
 * Downscale Module
 *----------------------------------------------------------------------------*/

/**
 * Computes the grascale value of the given window downscaled.
 *
 * This uses a simple average of all the values to determine the resultant
 * value. This is the combinational interface to the module.
 **/
grayscale_t compute_downscale(grayscale_window_t window) {
#pragma HLS INLINE

    // TODO: Implement
    return 0;
}

/**
 * Converts the image represented by the grayscale stream into a smaller image
 * represented by the output stream.
 *
 * This is the sequential interface to the module.
 **/
void downscale(grayscale_stream_t& grayscale_stream,
        grayscale_stream_t& downscale_stream) {
#pragma HLS INLINE

    // TODO: Implement
    return;
}

/*----------------------------------------------------------------------------
 * Top Function for Synthesis
 *----------------------------------------------------------------------------*/

/**
 * The top function for the downscale module when it is synthesized by itself.
 *
 * This is the function that HLS will look for if the blob detection module is
 * synthesized into its own IP block.
 **/
void downscale_top(grayscale_stream_t& grayscale_stream,
        grayscale_stream_t& downscale_stream) {
#pragma HLS INTERFACE axis port=grayscale_stream
#pragma HLS INTERFACE axis port=downscale_stream

    downscale(grayscale_stream, downscale_stream);
    return;
}
