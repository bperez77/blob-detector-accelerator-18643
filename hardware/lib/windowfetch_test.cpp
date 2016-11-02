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

//this is just a simple summing kernel
void *sum_window(void* window[KERNEL_HEIGHT][KERNEL_WIDTH]){
    int sum, last;
    num_axis_t pkt, outpkt;
    last = 0;
    for(int i = 0; i < KERMEL_HEIGHT; i++){
        for(int j = 0; j < KERNEL_WIDTH; j++){
            pkt = (num_axis_t)window[i][j];
            sum += pkt.data; 
            last = (last || pkt.tlast) ? 1 : 0; 
        }
    }
    outpkt.data = sum;
    outpkt.tlast = last;
    outpkt.tkeep = -1;
    return (void *)outpkt;
}


int main() {
    //num_axis_t -> IN_T num_axis_t -> OUT_T
    //num_stream_t -> IN_STREAM_T num_stream_t-> OUT_STREAM_T
    //32 -> IMAGE_WIDTH 3-> KERNEL_HEIGHT 4-> KERNEL_WIDTH
    //sum_window ->window_f 
    window_pipeline<num_axis_t, num_axis_t, num_stream_t, num_stream_t, 32, 3, 4, sum_window> w;

    num_stream_t stream1;
    num_stream_t stream2;
    for (int i = 0; i < 1024; i++) {
        num_axis_t inpkt;
        inpkt.data = i;
        inpkt.tkeep = -1;
        inpkt.tlast = (i == 1023) ? 1 : 0;
        stream1 << inpkt;
    }
    int last = 0;
    while (last == 0) {
        window_op (stream1, stream2);
        int load_success;
        num_axis_t outpkt;
        load_success = stream2 >> outpkt; //Assume returns 0 if success
        if (!load_success) {
            last = outpkt.tlast;
            prinft("Sum = %d", outpkt.data);
        }
    }
}
