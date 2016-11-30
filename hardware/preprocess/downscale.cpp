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
    ap_int<16> sum = 0; // window size <= 64; int_max_8_bit = 0xFF; 0xFF * 64 is a 16-bit number 
    ap_int<8> average = 0; // this will still be an 8-bit number because of the division 
    for (int i = 0; i < DOWNSCALE_FACTOR; i++){
	for (int j = 0; j < DOWNSCALE_FACTOR; j++){
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

    // TODO: Add in fractional support 
    // Read in all pixels at once  
    grayscale_t grayscale_pkt[DOWNSCALE_FACTOR][DOWNSCALE_FACTOR];
    grayscale_axis_t downscale_stream_pkt;
    for(int i = 0; i < IMAGE_HEIGHT; i+=DOWNSCALE_FACTOR){
       	for(int j = 0; j < IMAGE_WIDTH; j+=DOWNSCALE_FACTOR){
       		for(int i = 0; i < DOWNSCALE_FACTOR; i++){
       			for(int j = 0; j < DOWNSCALE_FACTOR; j++){
       				grayscale_pkt[i][j] = grayscale_stream.read().tdata;
       			}
       		}
       		downscale_stream_pkt.tdata = compute_downscale(grayscale_pkt);
       	}
    }    


    // Our transfers are always aligned, so set tkeep to -1, and assert
    // tlast when we reach the last packet
    downscale_stream_pkt.tkeep = -1;
    downscale_stream_pkt.tlast = grayscale_pkt[DOWNSCALE_FACTOR-1][DOWNSCALE_FACTOR-1];

    // Stream out the grayscale packet 
    downscale_stream << downscale_stream_pkt;
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
