#include <hls_stream.h>         // Definition of the stream class
#include "axis.h"               // Definition of the AXIS protocol structure
#include "image.h"              // Definition of the image info
#include "downscale.h"          // Definition of the downscale consts and methods 
#include "grayscale.h"          // Definition of the grayscale consts and methods 
#include <stdlib.h>


const int TEST_HEIGHT = 32;
const int TEST_WIDTH = 32;

/*
static const grayscale_t image[TEST_HEIGHT][TEST_WIDTH] ={
		{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
		{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
		{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
		{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
		{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
		{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
		{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
		{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
		{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
		{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
};*/

grayscale_stream_t grayscale_stream;


int main(){
	// fake image
	grayscale_t image[TEST_HEIGHT][TEST_WIDTH];
	grayscale_axis_t image_pixel_pkt;
	ap_int<1> last;
	for(int i = 0; i < TEST_HEIGHT; i++){
		for(int j = 0; j < TEST_WIDTH; j++){
			image[i][j] = 0xFF;
            image_pixel_pkt.tdata = image[i][j];
            image_pixel_pkt.tkeep = -1;
            if (i == TEST_HEIGHT-1 && j == TEST_WIDTH -1){
            	image_pixel_pkt.tlast = 1;
            }else{
            	image_pixel_pkt.tlast = 0;
            }
            grayscale_stream <<image_pixel_pkt;
			//printf("image[%d][%d] = %d\n", i, j, image[i][j].to_int());
		}
	}
	

	grayscale_stream_t downscale_stream;
	grayscale_axis_t downscale_pkt;
    last = 0;
	downscale<TEST_HEIGHT, TEST_WIDTH>(grayscale_stream, downscale_stream);
	while (last == 0){
		downscale_stream >> downscale_pkt;
		last = downscale_pkt.tlast;
		printf("downscale_pkt = %d\n",downscale_pkt.tdata.to_int());
	}

	return 0;
}
