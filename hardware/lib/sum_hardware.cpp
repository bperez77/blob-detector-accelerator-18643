#include <hls_stream.h>         // Definition of the stream class
#include "windowfetch.h"        // Definition of the window pipeline class 
#include "axis.h"               // Definition of the AXIS protocol structure

typedef axis<int, 32> num_axis_t;
typedef hls::stream<num_axis_t> num_stream_t;
const int KERNEL_HEIGHT = 3;
const int KERNEL_WIDTH = 3;
const int IMAGE_WIDTH = 32;
const int IMAGE_HEIGHT = 32;

//this is just a simple summing kernel
int sum_window_h(int window[KERNEL_HEIGHT][KERNEL_WIDTH], int start_row, int start_col){
    int sum, last;
    int pkt;
    sum = 0;
    last = 0;
    row: for(int i = 0; i < KERNEL_HEIGHT; i++){
#pragma HLS UNROLL
        col: for(int j = 0; j < KERNEL_WIDTH; j++){
#pragma HLS UNROLL
            pkt = window[(i+start_row) % KERNEL_HEIGHT][(j+start_col)%KERNEL_WIDTH];//(j+head_col) % KERNEL_WIDTH];
            sum += pkt;
        }
    }
    return sum;
}




int doWindow(num_stream_t& stream1, num_stream_t& stream2) {
    //num_axis_t -> IN_T num_axis_t -> OUT_T
    //num_stream_t -> IN_STREAM_T num_stream_t-> OUT_STREAM_T
    //32 -> IMAGE_WIDTH 3-> KERNEL_HEIGHT 4-> KERNEL_WIDTH
    window_pipeline<int, int, 32, 32, IMAGE_WIDTH, IMAGE_HEIGHT, KERNEL_HEIGHT, KERNEL_WIDTH, sum_window_h> w;

    w.window_op(stream1, stream2);

    return 0;
}
