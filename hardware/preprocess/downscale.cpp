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

    // TODO: Add in frational input support?
    ap_int sum = 0;
    ap_int average = 0; 
    for (i = 0; i < DOWNSCALE_FACTOR; i++){
	for (j = 0; j < DOWNSCALE_FACTOR; j++){
    		sum += window[i][j];
	}
    } 
    average = sum / (DOWNSCALE_FACTOR * DOWNSCALE_FACTOR);
    return average;
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

    // TODO: 
    // Read in the next grayscale_axis packet 
    grayscale_axis_t grayscale_axis_pkt;    
    grayscale_stream >> grayscale_axis_pkt; 

    // Compute the downscale value and send value to the downstream 
    grayscale_axis_t downscale_stream_pkt;  
    downscale_stream_pkt.tdata = compute_downscale();

    // Our transfers are always aligned, so set tkeep to -1, and assert
    // tlast when we reach the last packet
    downscale_stream_pkt.tkeep = -1;
    downscale_stream_pkt.tlast = grayscale_axis_pkt.tlast;  

    // Stream out the grayscale packet 
    downscale_stream << grayscale_axis_pkt; 
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
