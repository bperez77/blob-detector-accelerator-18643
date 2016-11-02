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

typedef void (*window_f)(void* window[KERNEL_HEIGHT][KERNEL_WIDTH]);
const window_f sum_window;

//IN_STREAM_T should be a wrapper of IN_T 
//OUT_STREAM_T should be a wrapper of OUT_T
template <typename IN_T, typename OUT_T, typename IN_STREAM_T, typename OUT_STREAM_T, 
          int IMAGE_WIDTH, int KERNEL_HEIGHT, int KERNEL_WIDTH, window_f window_function> 
struct window_pipeline {
    
    IN_T rowbuffer[KERNEL_HEIGHT][IMAGE_WIDTH];
    IN_T window[KERNEL_HEIGHT][KERNAL_WIDTH];

    //rowbuffer idx 
    int head_row;
    int tail_row; //The row we store into 
    int tail_col; //The col we store into -- equal to head_col

    //window idx 
    int head_win;
    int tail_win;

    // Constructor
    window_pipeline() {
        //initialize head and tail row
        this.head_row = 0;  
        this.tail_row = 0; 
        this.tail_col = 0; //Equal to head_col

        this.head_win = 0;
        this.tail_win = 0;
    }

    // window operation 
    void window_op(IN_STREAM_T& in_stream, OUT_STREAM_T& out_stream)
    { 
        IN_T int_pkt;
        OUT_T out_pkt;

        //Input Processing 
        in_stream >> in_pkt;
        // tail is the newest elem
        // head is the oldest elem   
        rowbuffer[tail_row][tail_col] = in_pkt; 
        

        //Window Forming
        if ((this.head_row - this.tail_row) % KERNEL_HEIGHT == 1) {
            for (int i = 0; i < KERNEL_HEIGHT; i++) {
                window[i][tail_win] = rowbuffer[i][tail_col]; //When i == tail_col we should really be forwarding, not loading again
            }
            this.tail_win = (this.tail_win + 1) % KERNEL_WIDTH;
        }


        //Output Processing
        if ((this.head_win - this.tail_win) % KERNEL_WIDTH == 1) {
            out_pkt = (OUT_T)(&window_function((&this.window));
            out_stream << out_pkt;
        }

        //update the head, tail position 
        this.update_tail();
        this.update_head();

        }
     


     // helper function
     void update_tail(){ 
        if ((this.tail_col + 1) == IMAGE_WIDTH){
             //update row
             this.tail_row = (this.tail_row + 1) % KERNEL_WIDTH; 
        }
        this.tail_col = (this.tail_col + 1) % IMAGE_WIDTH;
     }

     // update head 
     void update_head(){
         //update head_row
         if (this.tail_row == this.head_row){
            this.head_row = (this.head_row + 1) % KERNEL_WIDTH;
         }
     }

     
};



