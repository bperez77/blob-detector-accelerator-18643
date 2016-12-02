/**
 * @file downscale.h
 * @date Wednesday, November 23, 2016 at 05:23:21 PM EST
 * @author Brandon Perez (bmperez)
 *
 * This file contains the interface to the downscale module.
 *
 * This defines the downscale module interface, as both a sequential and
 * combinational interface.
 *
 * @bug No known bugs.
 **/

#ifndef DOWNSCALE_H_
#define DOWNSCALE_H_

#include "grayscale.h"              // Definition of the grayscale types

/*----------------------------------------------------------------------------
 * Definitions
 *----------------------------------------------------------------------------*/

/**
 * The amount that the image is scaled down by at each scale level, in each
 * dimension. For example, a WxH would become(W/FACTOR)x(H/FACTOR) image.
 **/
static const int DOWNSCALE_FACTOR = 4;

/**
 * A convenient alias for the window of grayscale values matching the size of
 * the image window being down-sized.
 **/
typedef grayscale_t grayscale_window_t[DOWNSCALE_FACTOR][DOWNSCALE_FACTOR];

/*----------------------------------------------------------------------------
 * Interface
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
grayscale_t compute_downscale(grayscale_window_t window);


/**
 * Downscales the image represented by the grayscale input stream into a smaller
 * image represented by the output grayscale stream.
 *
 * This uses a simple average over windows of the image (factor by factor
 * windows) to determine the resultant value in the output image. This is the
 * sequential interface to the module.
 *
 * @param image_width The width of image being downscaled.
 * @param image_height The height of image being downscaled.
 * @param[in] grayscale_stream The input stream of grayscale values.
 * @param[in] downscale_stream The output stream of grayscale values
 * representing the downscaled image.
 **/
template <int IMAGE_HEIGHT, int IMAGE_WIDTH>
void downscale(grayscale_stream_t& grayscale_stream, grayscale_stream_t& downscale_stream) {
#pragma HLS INLINE

    // TODO: Add in fractional support
    // Read in all pixels at once
	//grayscale_t gray_res;
	grayscale_t grayscale_buffer[DOWNSCALE_FACTOR-1][IMAGE_WIDTH];
	grayscale_t block[DOWNSCALE_FACTOR][DOWNSCALE_FACTOR];
	grayscale_axis_t downscale_stream_pkt;
	for(int i = 0; i < IMAGE_HEIGHT; i++){
		for(int j = 0; j < IMAGE_WIDTH; j++){
		    // buffer the value until the last row of a block height
		    // iterate through grayscale_buffer
		    int b = i % DOWNSCALE_FACTOR;
		    int c = j % DOWNSCALE_FACTOR;
		    if(i % DOWNSCALE_FACTOR < (DOWNSCALE_FACTOR-1)){
		    	grayscale_buffer[b][j] = grayscale_stream.read().tdata;
		    }
		    else{
		       	        block[b][c]= grayscale_stream.read().tdata;
		       		if (j % DOWNSCALE_FACTOR == DOWNSCALE_FACTOR-1){
		       			for(int p = 0; p <	(DOWNSCALE_FACTOR - 1); p++){
		       				for(int q = 0; q < DOWNSCALE_FACTOR; q++){
		       					block[p][q] = grayscale_buffer[p][j];
		       				}
		       			}
		       			downscale_stream_pkt.tdata = compute_downscale(block);
		       			// Our transfers are always aligned, so set tkeep to -1, and assert
		       			// tlast when we reach the last packet
		       			downscale_stream_pkt.tkeep = -1;
		       			downscale_stream_pkt.tlast = block[DOWNSCALE_FACTOR-1][DOWNSCALE_FACTOR-1];

		       			// Stream out the grayscale packet
		       			downscale_stream.write(downscale_stream_pkt);
		       	    }
		       	}
		  }
	}

    return;
}



#endif /* DOWNSCALE_H_ */
