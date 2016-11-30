/**
 * @file blob_detection.cpp
 * @date Wednesday, November 23, 2016 at 02:29:28 PM EST
 * @author Brandon Perez (bmperez)
 * @author Devon White (dww)
 * @author Yiyi Zhang (yiyiz)
 *
 * This file contains the implementation of the blob detection module.
 *
 * The blob detection module detects blobs at a single scale in the image, and
 * is used by the multi-scale blob detector to perform the detection at multiple
 * scales. Blobs are detected using an LoG filter, with the filter specifically
 * tuned for circular or elliptical light blobs in the image. These correspond
 * to potential headlights in the image.
 *
 * @bug No known bugs.
 **/

#include <ap_fixed.h>               // Arbitrary precision fixed-point types

#include "axis.h"                   // Definition of the AXIS protocol structure
#include "monochrome.h"             // Definition of the monochrome types
#include "blob_detection.h"         // Our interface and blob detection types
#include "windowfetch.h"           // Our implementation of window operation
#include "image.h"                  // Definition of image info 

/*----------------------------------------------------------------------------
 * Internal Definitions
 *----------------------------------------------------------------------------*/

/**
 * The fractional precision used for the LoG computation. This determines how
 * many bits are used for the fractional part of the fixed point representation.
 **/
static const int LOG_FRACTIONAL_BITS = 7;

/**
 * The type of an LoG response. This is a fixed point value with 1 bit for the
 * integer part, and LOG_FRACTIONAL_BITS bits for the fractional part. As all
 * the monochrome values are either 0 or 1, we only need 1 integral bit.
 **/
typedef ap_fixed<LOG_FRACTIONAL_BITS+1, LOG_FRACTIONAL_BITS> log_response_t;

/**
 * The threshold value used to determine if an LoG response corresponds to a
 * blob detection. If the response is greater than or equal to the threshold,
 * it becomes 1, otherwise, it becomes 0.
 **/
static const log_response_t LOG_RESPONSE_THRESHOLD = 0.492 * 1.0;

/**
 * The LoG filter kernel used to determine the LoG response for a window of the
 * image.
 **/
static const log_response_t LOG_FILTER[BLOB_FILTER_HEIGHT][BLOB_FILTER_WIDTH] = {
    {-0.0239, -0.0460, -0.0499, -0.0460, -0.0239},
    {-0.0460, -0.0061,  0.0923, -0.0061, -0.0460},
    {-0.0499,  0.0923,  0.3182,  0.0923, -0.0499},
    {-0.0460, -0.0061,  0.0923, -0.0061, -0.0460},
    {-0.0239, -0.0460, -0.0499, -0.0460, -0.0239},
};

/*----------------------------------------------------------------------------
 * LoG Filter Module
 *----------------------------------------------------------------------------*/

/**
 * Decides if the given window in the image corresponds a blob detection.
 *
 * This computes the LoG filter response for the given window of monochrome
 * values, and thresholds the response to determine if this window corresponds
 * to a blob. This is the combinational interface to the module.
 *
 * @param[in] window A window of monochrome values from an image.
 * @return 1 if the window corresponds to a blob, 0 otherwise.
 **/

blob_detection_t compute_blob_detection(monochrome_window_t window, int start_row, int start_col) {
#pragma HLS INLINE

    // FIXME: Test
    // Convolute two matrices
	(void) start_row;
	(void) start_col;
    log_response_t response = 0;
    for(int i = 0; i < BLOB_FILTER_HEIGHT; i++){
    	for(int j =0; j < BLOB_FILTER_WIDTH; j++){
            response += window[i][j] * LOG_FILTER[i][j];
        }
    } 
    return response>=LOG_RESPONSE_THRESHOLD;
}

/**
 * Converts the monochrome stream into a stream of blob detections.
 *
 * The blob detections are binary values that indicate if the pixel is the
 * centerpoint of a detected blob. This is the sequential interface to the
 * module.
 **/
void blob_detection(monochrome_stream_t& monochrome_stream,
        blob_detection_stream_t& blob_detection_stream) {
#pragma HLS INLINE

    // FIXME: Test

    // Declare a window object   
    window_pipeline <monochrome_t, blob_detection_t, 1, 1, IMAGE_HEIGHT,IMAGE_WIDTH,
    BLOB_FILTER_HEIGHT, BLOB_FILTER_WIDTH, compute_blob_detection> w;
 
    // Apply this operation
    w.window_op(monochrome_stream, blob_detection_stream);

    return;
}

/*----------------------------------------------------------------------------
 * Top Function for Synthesis
 *----------------------------------------------------------------------------*/

/**
 * The top function for the blob detection module when it is synthesized by
 * itself.
 *
 * This is the function that HLS will look for if the blob detection module is
 * synthesized into its own IP block.
 **/
void blob_detection_top(monochrome_stream_t& monochrome_stream,
        blob_detection_stream_t& blob_detection_stream) {
#pragma HLS INTERFACE axis port=monochrome_stream
#pragma HLS INTERFACE axis port=blob_detection_stream

    blob_detection(monochrome_stream, blob_detection_stream);
    return;
}
