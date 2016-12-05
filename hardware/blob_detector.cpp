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

#include "monochrome.h"         // Definition of the monochrome types
#include "blob_detection.h"     // Our interface and blob detection types
#include "image.h"              // Definition of image info
#include "grayscale.h"          // Definition of grayscale info
#include "downscale.h"          // Definition of downscale

/*----------------------------------------------------------------------------
 * Internal Definitions
 *----------------------------------------------------------------------------*/

// The type used to represent a coordinate in the image
typedef ap_int<16> coord_t;

/* The data sent back to the processor, a list of bounding boxes, (x,y) points,
 * in the image where the blobs have been detected. The list is terminated with
 * the point (-1, -1, -1, -1). */
typedef struct bounding_box {

private:
    ap_int<64> coords;             // (x1, y1), (x2, y2) coordainte

public:
    // Default constructor
    bounding_box() {}

    // Constructor from four points
    bounding_box(coord_t& x1, coord_t& y1, coord_t& x2, coord_t& y2) {
    #pragma HLS INLINE

        this->coords = (y2 << 48) | (x2 << 32) | (y1 << 16) | x1;
    }

    // Constructor from points a centerpoint and radius
    bounding_box(coord_t& cx, coord_t& cy, coord_t& radius) {
    #pragma HLS INLINE

        this->coords = ((cy + radius) << 48) | ((cx + radius) << 32) |
                ((cy - radius) << 16) | (cx - radius);
    }
} bbox_t;

// Definition of the packet and stream types for bounding boxes
typedef axis<bbox_t, 64> bbox_axis_t;
typedef hls::stream<bbox_axis_t> bbox_stream_t;

/* Unfortunately, there's no equivalent of a generate (compile-time) loop in
 * C++, so we manually enumerate the image sizes for each level. */
static const int NUM_SCALES     = 5;

/* The maximum number of permitted bounding boxes in the image. This is done
 * simply so that HLS can provide a timing estimate. */
static const int MAX_BBOXES     = 100;

/* To increase throughput, the image image is split into 4 sections
 * horizontally (row-wise), and this module is instantiated 4 times. */
static const int IMAGE_SPLITS 	= 4;

// Enumeration of the factors of each scale level
static const int SCALE0         = 1;
static const int SCALE1         = SCALE0 * DOWNSCALE_FACTOR;
static const int SCALE2         = SCALE1 * DOWNSCALE_FACTOR;
static const int SCALE3         = SCALE2 * DOWNSCALE_FACTOR;
static const int SCALE4         = SCALE3 * DOWNSCALE_FACTOR;

// Enumeration of the image widths at each scale level
static const int IMAGE_WIDTH0   = IMAGE_WIDTH / SCALE0;
static const int IMAGE_WIDTH1   = IMAGE_WIDTH / SCALE1;
static const int IMAGE_WIDTH2   = IMAGE_WIDTH / SCALE2;
static const int IMAGE_WIDTH3   = IMAGE_WIDTH / SCALE3;
static const int IMAGE_WIDTH4   = IMAGE_WIDTH / SCALE4;

// Enumeration of the image heights at each scale level
static const int IMAGE_HSECTION	= IMAGE_HEIGHT / IMAGE_SPLITS;
static const int IMAGE_HEIGHT0  = IMAGE_HSECTION / SCALE0;
static const int IMAGE_HEIGHT1  = IMAGE_HSECTION / SCALE1;
static const int IMAGE_HEIGHT2  = IMAGE_HSECTION / SCALE2;
static const int IMAGE_HEIGHT3  = IMAGE_HSECTION / SCALE3;
static const int IMAGE_HEIGHT4  = IMAGE_HSECTION / SCALE4;

/*----------------------------------------------------------------------------
 * Helper Functions
 *----------------------------------------------------------------------------*/

template <typename T, int IMAGE_WIDTH, int IMAGE_HEIGHT>
void duplicate_stream(hls::stream<T>& input, hls::stream<T>& output1,
        hls::stream<T>& output2) {
#pragma HLS INLINE

    // While the stream is not empty, send the next packet to both outputs
    dup_row_loop: for (int row = 0; row < IMAGE_HEIGHT; row++) {
        dup_col_loop: for (int col = 0; col < IMAGE_WIDTH; col++) {
        #pragma HLS PIPELINE II=1

            const T& in_pkt = input.read();
            output1.write(in_pkt);
            output2.write(in_pkt);
        }
    }

    return;
}

template <int N, int MAX_ELEMS>
void combine_streams(bbox_stream_t (&streams)[N], bbox_stream_t& output) {
    // Keep track of which last values were seen for each stream
    ap_uint<1> last_seen[N] = {0};
    ap_uint<1> last_count = 0;

    combine_packet: for (int i = 0; i < MAX_ELEMS && last_count < N; i++) {
        combine_streams: for (int j = 0; j < N; j++) {
        #pragma HLS UNROLL

            if (!last_seen[j]) {
                const bbox_axis_t& in_pkt = streams[i].read();
                output.write(in_pkt);
                last_seen[j] = in_pkt.tlast;
                last_count += 1;
            }
        }
    }

    // Terminate the output stream with the terminator
    coord_t term_coord = -1;
    bbox_t terminator = bbox_t(term_coord, term_coord, term_coord, term_coord);
    output.write(bbox_axis_t(terminator, 1));
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

template <int IMAGE_WIDTH, int IMAGE_HEIGHT, int SCALE>
static void blob_bounding_boxes(blob_detection_stream_t& blob_mask,
        bbox_stream_t& blobs) {
#pragma HLS INLINE

    bbox_cy_loop: for (coord_t cy = 0; cy < IMAGE_HEIGHT; cy++) {
        bbox_cx_loop: for (coord_t cx = 0; cx < IMAGE_WIDTH; cx++) {
        #pragma HLS PIPELINE II=1

            ap_int<1> detection = blob_mask.read().tdata;
            if (detection) {
                coord_t scaled_cx = SCALE * cx;
                coord_t scaled_cy = SCALE * cy;
                coord_t radius = SCALE * (BLOB_FILTER_WIDTH + 1) / 2;
                bbox_t bbox = bbox_t(scaled_cx, scaled_cy, radius);
                bbox_axis_t bbox_pkt = bbox_axis_t(bbox, 0);
                blobs.write(bbox_pkt);
            }
            if (cy == IMAGE_HEIGHT - 1 && cx == IMAGE_WIDTH - 1) {
                // Write the -1 terminator to the output stream
                coord_t term_coord = -1;
                bbox_t terminator = bbox_t(term_coord, term_coord, term_coord, term_coord);
                blobs.write(bbox_axis_t(terminator, 1));
            }
        }
    }

    return;
}

template <int IMAGE_WIDTH, int IMAGE_HEIGHT, int SCALE>
static void single_scale_blob_detector(grayscale_stream_t& image,
        bbox_stream_t& blobs) {
#pragma HLS INLINE

    // Convert the image to monochrome, and perform blob detection
    monochrome_stream_t mono_image;
    blob_detection_stream_t blob_mask;
    monochrome(image, mono_image);
    blob_detection<IMAGE_WIDTH, IMAGE_HEIGHT>(mono_image, blob_mask);

    // Convert the blob detections into a stream of bounding boxes
    blob_bounding_boxes<IMAGE_WIDTH, IMAGE_HEIGHT, SCALE>(blob_mask, blobs);
    return;
}

void blob_detector(pixel_stream_t& rgba_image, bbox_stream_t& blobs) {
#pragma HLS INTERFACE axis port=rgba_image
#pragma HLS INTERFACE axis port=blobs
#pragma HLS INTERFACE ap_ctrl_none port=return

#pragma HLS DATAFLOW

    // Convert the image to grayscale, and duplicate the stream
    grayscale_stream_t gray_image, images1[NUM_SCALES], images2[NUM_SCALES-1];
    #pragma HLS ARRAY_PARTITION complete variable=images1
    #pragma HLS ARRAY_PARTITION complete variable=images2
    grayscale(rgba_image, gray_image);
    duplicate_stream<grayscale_axis_t, IMAGE_WIDTH, IMAGE_HEIGHT>(gray_image,
            images1[0], images2[0]);

    // Downscale the image for all 5 scale levels, producing duplicate streams
    downscale_image<IMAGE_WIDTH0, IMAGE_HEIGHT0>(images2[0], images1[1],
            images2[1]);
    downscale_image<IMAGE_WIDTH1, IMAGE_HEIGHT1>(images2[1], images1[2],
            images2[2]);
    downscale_image<IMAGE_WIDTH2, IMAGE_HEIGHT2>(images2[2], images1[3],
            images2[3]);
    downscale<IMAGE_WIDTH3, IMAGE_HEIGHT3>(images2[3], images1[4]);

    // Run blob detection on each of the 5 scale levels
    bbox_stream_t scale_blobs[NUM_SCALES];
    #pragma HLS ARRAY_PARTITION complete variable=scale_blobs
    single_scale_blob_detector<IMAGE_WIDTH0, IMAGE_HEIGHT0, SCALE0>(images1[0],
            scale_blobs[0]);
    single_scale_blob_detector<IMAGE_WIDTH1, IMAGE_HEIGHT1, SCALE1>(images1[1],
            scale_blobs[1]);
    single_scale_blob_detector<IMAGE_WIDTH2, IMAGE_HEIGHT2, SCALE2>(images1[2],
            scale_blobs[2]);
    single_scale_blob_detector<IMAGE_WIDTH3,IMAGE_HEIGHT3, SCALE3>(images1[3],
            scale_blobs[3]);
    single_scale_blob_detector<IMAGE_WIDTH4, IMAGE_HEIGHT4, SCALE4>(images1[4],
            scale_blobs[4]);

    // Combine the 5 streams of blob bounding boxes into a single stream
    combine_streams<NUM_SCALES, MAX_BBOXES>(scale_blobs, blobs);
    return;
}
