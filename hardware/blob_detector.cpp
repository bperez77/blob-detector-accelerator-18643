/**
 * @file blob_detector.cpp
 * @date Saturday, December 03, 2016 at 03:44:37 PM EST
 * @author Yiyi Zhang (yiyiz)
 *
 * This file conatins the implementation of the multi-scale blob detector
 * pipeline.
 *
 * The blob detection is performed at the given number of scales, with the same
 * steps taken in each one. The processor streams in an input image to the
 * hardware. The bounding boxes of the blobs results are recombined at the
 * end and streamed back to the processor.
 *
 * @bug No known bugs.
 **/

#include "monochrome.h"             // Definition of the monochrome types
#include "blob_detection.h"         // Our interface and blob detection types
#include "image.h"                  // Definition of image info
#include "grayscale.h"              // Definition of grayscale info
#include "downscale.h"              // Definition of downscale

/*----------------------------------------------------------------------------
 * Internal Definitions
 *----------------------------------------------------------------------------*/

/* Unfortunately, there's no equivalent of a generate (compile-time) loop in
 * C++, so we manually enumerate the image sizes for each level. */
static const int NUM_SCALES         = 5;

// Enumeration of the image widths at each scale level
static const int IMAGE_WIDTH1 = IMAGE_WIDTH / DOWNSCALE_FACTOR;
static const int IMAGE_WIDTH2 = IMAGE_WIDTH1 / DOWNSCALE_FACTOR;
static const int IMAGE_WIDTH3 = IMAGE_WIDTH2 / DOWNSCALE_FACTOR;
static const int IMAGE_WIDTH4 = IMAGE_WIDTH3 / DOWNSCALE_FACTOR;

// Enumeration of the image heights at each scale level
static const int IMAGE_HEIGHT1 = IMAGE_HEIGHT / DOWNSCALE_FACTOR;
static const int IMAGE_HEIGHT2 = IMAGE_HEIGHT1 / DOWNSCALE_FACTOR;
static const int IMAGE_HEIGHT3 = IMAGE_HEIGHT2 / DOWNSCALE_FACTOR;
static const int IMAGE_HEIGHT4 = IMAGE_HEIGHT3 / DOWNSCALE_FACTOR;

/*----------------------------------------------------------------------------
 * Helper Functions
 *----------------------------------------------------------------------------*/

template <typename T, int IMAGE_WIDTH, int IMAGE_HEIGHT>
void duplicate_stream(hls::stream<T>& input, hls::stream<T>& output1,
        hls::stream<T>& output2) {
#pragma HLS INLINE

    // While the stream is not empty, send the next packet to both outputs
    for (int row = 0; row < IMAGE_HEIGHT; row++) {
        for (int col = 0; col < IMAGE_WIDTH; col++) {
        #pragma HLS PIPELINE II=1

            T in_pkt = input.read();
            output1.write(in_pkt);
            output2.write(in_pkt);
        }
    }

    return;
}

/*----------------------------------------------------------------------------
 * Multiscale Blob Detector
 *----------------------------------------------------------------------------*/

template <int IMAGE_WIDTH, int IMAGE_HEIGHT>
static void downscale_image(grayscale_stream_t& image,
        grayscale_stream_t& downscaled1, grayscale_stream_t& downscaled2) {
#pragma HLS INLINE

    // Downscale the image, and duplicate the stream
    grayscale_stream_t downscaled_image;
    downscale<IMAGE_WIDTH, IMAGE_HEIGHT>(image, downscaled_image);
    duplicate_stream<grayscale_axis_t, IMAGE_WIDTH, IMAGE_HEIGHT>(
            downscaled_image, downscaled1, downscaled2);
    return;
}

template <int IMAGE_WIDTH, int IMAGE_HEIGHT>
static void single_scale_blob_detector(grayscale_stream_t& image,
        blob_detection_stream_t& blob_mask) {
#pragma HLS INLINE

    // Convert the image to monochrome, and perform blob detection
    monochrome_stream_t mono_image;
    monochrome(image, mono_image);
    blob_detection<IMAGE_WIDTH, IMAGE_HEIGHT>(mono_image, blob_mask);
    return;
}

void blob_detector(pixel_stream_t& rgba_image,
        blob_detection_stream_t& blob_mask0,
        blob_detection_stream_t& blob_mask1,
        blob_detection_stream_t& blob_mask2,
        blob_detection_stream_t& blob_mask3,
        blob_detection_stream_t& blob_mask4) {
#pragma HLS INTERFACE axis port=rgba_image
#pragma HLS INTERFACE axis port=blob_mask0
#pragma HLS INTERFACE axis port=blob_mask1
#pragma HLS INTERFACE axis port=blob_mask2
#pragma HLS INTERFACE axis port=blob_mask3
#pragma HLS INTERFACE axis port=blob_mask4
#pragma HLS INTERFACE ap_ctrl_none port=return

#pragma HLS DATAFLOW

    // Convert the image to grayscale, and duplicate the stream
    grayscale_stream_t gray_image, gray_image1, gray_image2;
    grayscale(rgba_image, gray_image);
    duplicate_stream<grayscale_axis_t, IMAGE_WIDTH, IMAGE_HEIGHT>(gray_image,
            gray_image1, gray_image2);

    // Downscale the image for all 5 scale levels, producing duplicate streams
    grayscale_stream_t level1_image1, level1_image2, level2_image1;
    grayscale_stream_t level2_image2, level3_image1, level3_image2;
    grayscale_stream_t level4_image;
    downscale_image<IMAGE_WIDTH, IMAGE_HEIGHT>(gray_image2, level1_image1,
            level1_image2);
    downscale_image<IMAGE_WIDTH1, IMAGE_HEIGHT1>(level1_image2, level2_image1,
            level2_image2);
    downscale_image<IMAGE_WIDTH2, IMAGE_HEIGHT2>(level2_image2, level3_image1,
            level3_image2);
    downscale<IMAGE_WIDTH3, IMAGE_HEIGHT3>(level3_image2, level4_image);

    // Run blob detection on each of the 5 scale levels
    single_scale_blob_detector<IMAGE_WIDTH, IMAGE_HEIGHT>(gray_image1,
            blob_mask0);
    single_scale_blob_detector<IMAGE_WIDTH1, IMAGE_HEIGHT1>(level1_image1,
            blob_mask1);
    single_scale_blob_detector<IMAGE_WIDTH2, IMAGE_HEIGHT2>(level2_image1,
            blob_mask2);
    single_scale_blob_detector<IMAGE_WIDTH3, IMAGE_HEIGHT3>(level3_image1,
            blob_mask3);
    single_scale_blob_detector<IMAGE_WIDTH4, IMAGE_HEIGHT4>(level4_image,
            blob_mask4);

    return;
}
