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
static const int DOWNSCALE_FACTOR = 2;

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
 * @param[in] grayscale_stream The input stream of grayscale values.
 * @param[in] downscale_stream The output stream of grayscale values
 * representing the downscaled image.
 **/
void downscale(grayscale_stream_t& grayscale_stream,
        grayscale_stream_t& downscale_stream);

#endif /* DOWNSCALE_H_ */
