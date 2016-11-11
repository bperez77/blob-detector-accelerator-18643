/**
 * @file windowfetch.h
 * @date Tuesday, November 1, 2016 at 05:29:10 PM EDT
 * @author Brandon Perez (bmperez)
 * @author Devon White (dww)
 * @author Yiyi Zhang (yiyiz)
 *
 * @bug No known bugs.
 **/

//Add #define here 

#include <hls_stream.h>         // Definition of the stream class

#include "axis.h"               // Definition of the AXIS protocol structure


//const window_f sum_window;

//IN_STREAM_T should be a wrapper of IN_T 
//OUT_STREAM_T should be a wrapper of OUT_T
template <typename IN_T, typename OUT_T, typename IN_STREAM_T, typename OUT_STREAM_T, 
          int IMAGE_WIDTH, int KERNEL_HEIGHT, int KERNEL_WIDTH,
          OUT_T (*window_f)(IN_T window[KERNEL_HEIGHT][KERNEL_WIDTH])>
struct window_pipeline {
    
    IN_T rowbuffer[KERNEL_HEIGHT][IMAGE_WIDTH];
    IN_T window[KERNEL_HEIGHT][KERNEL_WIDTH];

    //typedef OUT_T (*window_f)(IN_T window[KERNEL_HEIGHT][KERNEL_WIDTH]);
    //const window_f window_function;
    //rowbuffer idx
    uint head_row;
    uint tail_row; //The row we store into
    uint tail_col; //The col we store into -- equal to head_col
    uint init_row; //Are we doing the first row for the first time

    //window idx 
    uint head_win;
    uint tail_win;

    // Constructor
    window_pipeline() {
        //initialize head and tail row
        head_row = 0;
        tail_row = 0;
        tail_col = 0; //Equal to head_col
        init_row = 1;

        head_win = 0;
        tail_win = 0;
    }

    // window operation 
    void window_op(IN_STREAM_T& in_stream, OUT_STREAM_T& out_stream)
    { 
        IN_T in_pkt;
        OUT_T out_pkt;
        //Input Processing 
        in_stream >> in_pkt;
        // tail is the newest elem
        // head is the oldest elem   
        rowbuffer[tail_row][tail_col] = in_pkt; 
        

        //Window Forming
        printf("Row, head: %d, tail: %d, diff: %d\n", head_row, tail_row, diffOne(head_row, tail_row, KERNEL_HEIGHT));
        if (diffOne(head_row, tail_row, KERNEL_HEIGHT)) {
            for (int i = 0; i < KERNEL_HEIGHT; i++) {
            	printf("Store window row: %d, col: %d, data: %d\n", i, tail_win, rowbuffer[i][tail_col].tdata);
                window[i][tail_win] = rowbuffer[i][tail_col]; //When i == tail_col we should really be forwarding, not loading again
            }
            printf("Window, head: %d, tail: %d, diff: %d\n", head_win, tail_win, diffOne(head_win, tail_win, KERNEL_WIDTH));
            //Output Processing
            if (diffOne(head_win, tail_win, KERNEL_WIDTH)) {
                out_pkt = window_f(window);
                out_stream << out_pkt;
            }
            tail_win = (tail_win + 1) % KERNEL_WIDTH;
            head_win = (tail_win == head_win) ? head_win + 1 : head_win;
        }

        //printf("Window, head:%d, tail:%d, mod:%d\n", head_win, tail_win, diffOne(head_win, tail_win));
        //Output Processing


        //update the head, tail position 
        update_tail();
        update_head();

     }
     


     // helper function
     void update_tail(){ 
        if ((tail_col + 1) == IMAGE_WIDTH){
             //update row
             tail_row = (tail_row + 1) % KERNEL_HEIGHT;
             init_row = 0;
        }
        tail_col = (tail_col + 1) % IMAGE_WIDTH;
     }

     // update head 
     void update_head(){
         //update head_row
         if (tail_row == head_row && !init_row){
            head_row = (head_row + 1) % KERNEL_HEIGHT;
         }
     }

     int diffOne(uint h, uint t, uint size) { //(h - t) % KERNEL_WIDTH == 1
    	 if ((h - t) == 1) return 1;
    	 if (h == 0 && (t == size - 1)) return 1;
    	 return 0;
     }

     
};



