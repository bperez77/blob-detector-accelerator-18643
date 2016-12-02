#include <hls_stream.h>         // Definition of the stream class
#include "axis.h"               // Definition of the AXIS protocol structure
#include "image.h"              // Definition of the image info
#include "downscale.h"          // Definition of the downscale consts and methods 
#include "grayscale.h"          // Definition of the grayscale consts and methods 


const int TEST_HEIGHT = 8 ; 
const int TEST_WIDTH = 8 ; 

int main(){
	// grayscale_t compute_downscale(grayscale_window_t window)
	// fake image
	grayscale_t window[TEST_HEIGHT][TEST_WIDTH];
	for(int i = 0; i < TEST_HEIGHT; i++){
		for(int j = 0; j < TEST_WIDTH; j++){
			window[i][j] = 8;
			//printf("window[%d][%d] = %d\n", i, j, window[i][j].to_int());
		}
	}
	

	grayscale_t gray_res;
	grayscale_window_t block; // 2D array of DOWNSCALE_FACTOR

	grayscale_t grayscale_buffer[DOWNSCALE_FACTOR-1][TEST_WIDTH];

	for(int i = 0; i < TEST_HEIGHT; i++){
	    for(int j = 0; j < TEST_WIDTH; j++){
	    	// buffer the value until the last row of a block height
	    	if(i % DOWNSCALE_FACTOR < (DOWNSCALE_FACTOR-1)){
	    		grayscale_buffer[i][j] = window[i][j];
	    	}
	    	else{
	    		// iterate through grayscale_buffer
	    		int b = i % DOWNSCALE_FACTOR;
	    		int c = j % DOWNSCALE_FACTOR;
	       		block[b][c]= window[i][j];
                //printf("block[%d][%d] = %d\n",b,c, block[b][c].to_int());
                //printf("window[%d][%d] = %d\n", i,j,window[i][j].to_int());
	       		if (j % DOWNSCALE_FACTOR == DOWNSCALE_FACTOR-1){
	       			for(int p = 0; p <	(DOWNSCALE_FACTOR - 1); p++){
	       				for(int q = 0; q < DOWNSCALE_FACTOR; q++){
	       					block[p][q] = grayscale_buffer[p][j];
	       				}
	       			}
	       			//////////////PRINT BLOCK////////////////////////
	       			/*for(int m = 0; m < DOWNSCALE_FACTOR; m++){
	       				for(int n = 0; n < DOWNSCALE_FACTOR; n++){
	       					printf("block[%d][%d]=%d\n",m,n,block[m][n].to_int());
	       				}
	       			}*/
	       		    /////////////////////////////////////////////////
	       			gray_res = compute_downscale(block);
	       			printf("window[%d][%d]:gray_res = %d\n", i, j, gray_res.to_uint());
	       		}
	       	}
	      }
	 }

	return 0;
}
