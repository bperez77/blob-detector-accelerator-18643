/**
 * @file single_scale.cpp
 * @date Saturday, December 03, 2016 at 03:44:37 PM EST
 * @author Yiyi Zhang (yiyiz)
 *
 *
 * This file contains the implementation of the multi-scale implementation.
 *
 * FIXME: Insert a longer description here
 *
 * @bug No known bugs.
 **/

#include <ap_fixed.h>               // Arbitrary precision fixed-point types

#include "axis.h"                   // Definition of the AXIS protocol structure
#include "monochrome.h"             // Definition of the monochrome types
#include "blob_detection.h"         // Our interface and blob detection types
#include "windowfetch.h"            // Our implementation of window operation
#include "image.h"                  // Definition of image info
#include "grayscale.h"              // Definition of grayscale info
#include "downscale.h"              // Definition of downscale
/**
 * Implements the multi scale implementation for now.
 *
 * Contains Grayscale, Monochrome and LoG.
 *
 * @param[in] var_name : pixel_stream stream input to grayscale module
 *
 * @param[out] var_name : blob_detection_stream output from LoG
 *
 * @return : None
 **/
void multi_scale_top(pixel_stream_t& pixel_stream,
        blob_detection_stream_t& blob_detection_stream, blob_detection_stream_t& blob_detection_stream_1,
        blob_detection_stream_t& blob_detection_stream_2, blob_detection_stream_t& blob_detection_stream_3,
        blob_detection_stream_t& blob_detection_stream_4) {
#pragma HLS INLINE
//#pragma HLS DATAFLOW
#pragma HLS INTERFACE axis port=pixel_stream
#pragma HLS INTERFACE axis port=blob_detection_stream

	// Grayscale
	grayscale_stream_t grayscale_mono_stream;
	grayscale_stream_t grayscale_down_stream;
	grayscale(pixel_stream, grayscale_mono_stream, grayscale_down_stream);

    // width const assignment
	static const int WIDTH_LEVEL1 = IMAGE_WIDTH / DOWNSCALE_FACTOR;
	static const int WIDTH_LEVEL2 = WIDTH_LEVEL1 / DOWNSCALE_FACTOR;
	static const int WIDTH_LEVEL3 = WIDTH_LEVEL2 / DOWNSCALE_FACTOR;
	static const int WIDTH_LEVEL4 = WIDTH_LEVEL3 / DOWNSCALE_FACTOR;

	// height const assignment
	static const int HEIGHT_LEVEL1 = IMAGE_HEIGHT / DOWNSCALE_FACTOR;
	static const int HEIGHT_LEVEL2 = HEIGHT_LEVEL1 / DOWNSCALE_FACTOR;
	static const int HEIGHT_LEVEL3 = HEIGHT_LEVEL2 / DOWNSCALE_FACTOR;
	static const int HEIGHT_LEVEL4 = HEIGHT_LEVEL3 / DOWNSCALE_FACTOR;

	// Downscale 5 times
	grayscale_stream_t downscale_stream;
	downscale<IMAGE_HEIGHT, IMAGE_WIDTH>(grayscale_mono_stream, downscale_stream);
	grayscale_stream_t downscale_stream_1;
	downscale<HEIGHT_LEVEL1, WIDTH_LEVEL1>(downscale_stream,downscale_stream_1);
	grayscale_stream_t downscale_stream_2;
    downscale<HEIGHT_LEVEL2, WIDTH_LEVEL2>(downscale_stream_1,downscale_stream_2);
    grayscale_stream_t downscale_stream_3;
    downscale<HEIGHT_LEVEL3, WIDTH_LEVEL3>(downscale_stream_2,downscale_stream_3);
    grayscale_stream_t downscale_stream_4;
    downscale<HEIGHT_LEVEL4, WIDTH_LEVEL4>(downscale_stream_3,downscale_stream_4);


    // Monochrome
	monochrome_stream_t monochrome_stream;
	monochrome(grayscale_down_stream, monochrome_stream);
	monochrome_stream_t monochrome_stream_1;
	monochrome(downscale_stream_1, monochrome_stream_1);
	monochrome_stream_t monochrome_stream_2;
	monochrome(downscale_stream_2, monochrome_stream_2);
	monochrome_stream_t monochrome_stream_3;
	monochrome(downscale_stream_3, monochrome_stream_3);
	monochrome_stream_t monochrome_stream_4;
	monochrome(downscale_stream_4, monochrome_stream_4);


	// LoG
	blob_detection(monochrome_stream, blob_detection_stream);
	blob_detection(monochrome_stream_1, blob_detection_stream_1);
	blob_detection(monochrome_stream_2, blob_detection_stream_2);
	blob_detection(monochrome_stream_3, blob_detection_stream_3);
	blob_detection(monochrome_stream_4, blob_detection_stream_4);


	return;
}










