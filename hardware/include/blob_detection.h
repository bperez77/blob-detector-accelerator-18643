/**
 * @file blob_detection.h
 * @date Wednesday, November 23, 2016 at 02:59:28 PM EST
 * @author Brandon Perez (bmperez)
 * @author Devon White (dww)
 * @author Yiyi Zhang (yiyiz)
 *
 * This file contains the interface to the blob detection module.
 *
 * This defines the blob detection module interface, as both a sequential
 * (streaming) interface, and a combinational interface. It also defines the
 * input and outptut types for the module.
 *
 * @bug No known bugs.
 **/

#ifndef BLOB_DETECTION_H_
#define BLOB_DETECTION_H_

#include <hls_stream.h>             // Definition of the hls::stream class
#include <ap_int.h>                 // Arbitrary precision integer types

#include "axis.h"                   // Definition of the AXIS protocol structure
#include "monochrome.h"             // Definition of the monochrome types

/*----------------------------------------------------------------------------
 * Definitions
 *----------------------------------------------------------------------------*/

/**
 * The dimensions of the blob filter (which is LoG). This also determines the
 * size of the window operated on in the image.
 **/
static const int BLOB_FILTER_WIDTH = 5;
static const int BLOB_FILTER_HEIGHT = BLOB_FILTER_WIDTH;

/**
 * A convenient alias for the window of monochrome values matching the filter
 * size.
 **/
typedef monochrome_t monochrome_window_t[BLOB_FILTER_HEIGHT][BLOB_FILTER_WIDTH];

/**
 * The input stream type is a monochrome AXIS packet. The output stream type is
 * a 1-bit boolean value AXIS packet. This boolean indicates if the pixel
 * corresponds to the centerpoint of a blob detection.
 **/
typedef ap_uint<1> blob_detection_t;
typedef axis<blob_detection_t, 1> blob_detection_axis_t;
typedef hls::stream<blob_detection_axis_t> blob_detection_stream_t;

/*----------------------------------------------------------------------------
 * Interface
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
blob_detection_t compute_blob_detection(monochrome_window_t window,
        int start_row, int start_col);

/**
 * Converts the monochrome input stream into an output LoG detection stream.
 *
 * This is done by computing the LoG response on windows of the image, and
 * thresholding to determine if it is a blob. The output stream indicates if the
 * pixel is a centerpoint of a detected blob in an image. This is the sequential
 * interface to the module.
 *
 * @tparam IMAGE_WIDTH The width of the image being processed.
 * @tparam IMAGE_HEIGHT The height of the image being processed.
 *
 * @param[in] monochrome_stream The input stream of monochrome values.
 * @param[out] blob_detection_stream The output stream of LoG detections.
 **/
template <int IMAGE_WIDTH, int IMAGE_HEIGHT>
void blob_detection(monochrome_stream_t& monochrome_stream,
        blob_detection_stream_t& blob_detection_stream);

#endif /* BLOB_DETECTION_H_ */
