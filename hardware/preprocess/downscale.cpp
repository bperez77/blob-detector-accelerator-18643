/**
 * @file downscale.cpp
 * @date Wednesday, November 23, 2016 at 05:23:16 PM EST
 * @author Brandon Perez (bmperez)
 * @author Devon White (dww)
 * @author Yiyi Zhang (yiyiz)
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
 * Computes the grayscale value of given image window downscaled.
 *
 * This uses a simple average of all the values in the window to determine what
 * the resultant value in the downsampled image should be. This is the
 * combinational interface to the module.
 *
 * @param[in] window A window of grayscale values from the image to downscale.
 * @return The downscaled (scalar) value for the window, using an average of the
 * pixels.
 **/
grayscale_t compute_downscale(grayscale_window_t window) {
#pragma HLS INLINE

	// TODO: Add in fractional input support?
    ap_int<16> sum = 0;
    ap_int<8> average = 0;
    for (int i = 0; i < DOWNSCALE_FACTOR; i++){
    	for (int j = 0; j < DOWNSCALE_FACTOR; j++){
    		sum += window[i][j];
    		//printf("sum = %d\n", sum.to_int());
    	}
    } 
    //printf("sum = %d\n", sum.to_int());
    average = sum / (DOWNSCALE_FACTOR * DOWNSCALE_FACTOR);
    return average;
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

    downscale<IMAGE_HEIGHT, IMAGE_WIDTH>(grayscale_stream, downscale_stream);
    return;
}
