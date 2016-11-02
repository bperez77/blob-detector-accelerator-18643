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
          int IMAGE_WIDTH, int KERNEL_HEIGHT, int KERNEL_WIDTH>
struct window_pipeline {
    
    IN_T rowbuffer[KERNEL_HEIGHT][IMAGE_WIDTH];
    IN_T window[KERNEL_HEIGHT][KERNEL_WIDTH];

    typedef OUT_T (*window_f)(IN_T window[KERNEL_HEIGHT][KERNEL_WIDTH]);
    const window_f window_function;
    //rowbuffer idx
    int head_row;
    int tail_row; //The row we store into 
    int tail_col; //The col we store into -- equal to head_col

    //window idx 
    int head_win;
    int tail_win;

    // Constructor
    window_pipeline(window_f window_function) : window_function(window_function) {
        //initialize head and tail row
        head_row = 0;
        tail_row = 0;
        tail_col = 0; //Equal to head_col

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
        if ((head_row - tail_row) % KERNEL_HEIGHT == 1) {
            for (int i = 0; i < KERNEL_HEIGHT; i++) {
                window[i][tail_win] = rowbuffer[i][tail_col]; //When i == tail_col we should really be forwarding, not loading again
            }
            tail_win = (tail_win + 1) % KERNEL_WIDTH;
        }


        //Output Processing
        if ((head_win - tail_win) % KERNEL_WIDTH == 1) {
            out_pkt = (OUT_T)(&window_function((&window));
            out_stream << out_pkt;
        }

        //update the head, tail position 
        update_tail();
        update_head();

        }
     


     // helper function
     void update_tail(){ 
        if ((tail_col + 1) == IMAGE_WIDTH){
             //update row
             tail_row = (tail_row + 1) % KERNEL_WIDTH;
        }
        tail_col = (tail_col + 1) % IMAGE_WIDTH;
     }

     // update head 
     void update_head(){
         //update head_row
         if (tail_row == head_row){
            head_row = (head_row + 1) % KERNEL_WIDTH;
         }
     }

     
};



