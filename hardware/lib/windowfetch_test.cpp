/**
 * @file windowfetch_test.cpp
 * @date Wednesday, November 2, 2016 at 02:42:10 PM EDT
 * @author Brandon Perez (bmperez)
 * @author Devon White (dww)
 * @author Yiyi Zhang (yiyiz)
 *
 * @bug No known bugs.
 **/

//Add #define here 

#include <hls_stream.h>         // Definition of the stream class
#include "windowfetch.h"        // Definition of the window pipeline class 
#include "axis.h"               // Definition of the AXIS protocol structure

typedef axis<int, 32> num_axis_t;
typedef hls::stream<num_axis_t> num_stream_t;
const int KERNEL_HEIGHT = 4;
const int KERNEL_WIDTH = 3;
const int IMAGE_WIDTH = 32; //Factor of Kheight


//this is just a simple summing kernel
num_axis_t sum_window(num_axis_t window[KERNEL_HEIGHT][KERNEL_WIDTH]){
    int sum, last;
    num_axis_t pkt, outpkt;
    printf("Sum Window\n");
    last = 0;
    for(int i = 0; i < KERNEL_HEIGHT; i++){
        for(int j = 0; j < KERNEL_WIDTH; j++){
            pkt = window[i][j];//(j+head_col) % KERNEL_WIDTH];
            sum += pkt.tdata;
            printf("Window, i:%d, j:%d, data: %d tlast: %d\n", i, j, pkt.tdata, pkt.tlast.to_int());
            if (pkt.tlast.to_int()) last = 1;
        }
    }
    outpkt.tdata = sum;
    outpkt.tlast = last;
    outpkt.tkeep = -1;
    return outpkt;
}


int main() {
    //num_axis_t -> IN_T num_axis_t -> OUT_T
    //num_stream_t -> IN_STREAM_T num_stream_t-> OUT_STREAM_T
    //32 -> IMAGE_WIDTH 3-> KERNEL_HEIGHT 4-> KERNEL_WIDTH
    window_pipeline<num_axis_t, num_axis_t, num_stream_t, num_stream_t, IMAGE_WIDTH, KERNEL_HEIGHT, KERNEL_WIDTH, sum_window> w;

    num_stream_t stream1;
    num_stream_t stream2;
    for (int i = 0; i < 1024; i++) {
        num_axis_t inpkt;
        inpkt.tdata = i;
        inpkt.tkeep = -1;
        inpkt.tlast = (i == 1023) ? 1 : 0;
        //printf("Tlast: %d\n", inpkt.tlast.to_int());
        stream1 << inpkt;
    }
    int last = 0;
    while (last == 0) {
    	printf("Window Op\n");
        w.window_op(stream1, stream2);
        if (!stream2.empty()) {
        	num_axis_t outpkt;
        	stream2 >> outpkt;
        	last = outpkt.tlast.to_int();
        	printf("Sum = %d, last = %d\n", outpkt.tdata, last);
        }
    }

    return 0;
}
