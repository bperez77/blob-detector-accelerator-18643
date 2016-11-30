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
const int KERNEL_HEIGHT = 3;
const int KERNEL_WIDTH = 3;
const int IMAGE_WIDTH = 32;
const int IMAGE_HEIGHT = 32;


extern int doWindow(num_stream_t& stream1, num_stream_t& stream2);

int main() {
    //num_axis_t -> IN_T num_axis_t -> OUT_T
    //num_stream_t -> IN_STREAM_T num_stream_t-> OUT_STREAM_T
    //32 -> IMAGE_WIDTH 3-> KERNEL_HEIGHT 4-> KERNEL_WIDTH

    num_stream_t stream1;
    num_stream_t stream2;
    for (int i = 0; i < IMAGE_WIDTH*IMAGE_HEIGHT; i++) {
        num_axis_t inpkt;
        inpkt.tdata = 1;
        inpkt.tkeep = -1;
        inpkt.tlast = (i == IMAGE_WIDTH*IMAGE_HEIGHT-1) ? 1 : 0;
        //printf("Tlast: %d\n", inpkt.tlast.to_int());
        stream1 << inpkt;
    }

    doWindow(stream1, stream2);

    int last = 0;
    int count = 0;
    int image[IMAGE_HEIGHT][IMAGE_WIDTH];

    while (last == 0) {
        if (!stream2.empty()) {
        	num_axis_t outpkt;
        	stream2 >> outpkt;
        	last = outpkt.tlast.to_int();
        	image[0][count] = outpkt.tdata;
        	count += 1;
        	//printf("Sum = %d, last = %d\n", outpkt.tdata, last);
        }
    }
    printf("Count: %d\n", count);
    for (int i = 0; i < IMAGE_HEIGHT; i++) {
        for (int j = 0; j < IMAGE_WIDTH; j++) {
        	printf("%d ", image[i][j]%10);
        }
        printf("\n");
    }


    return 0;
}
