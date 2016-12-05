/**
 * @file windowfetch.h
 * @date Tuesday, November 1, 2016 at 05:29:10 PM EDT
 * @author Brandon Perez (bmperez)
 * @author Devon White (dww)
 * @author Yiyi Zhang (yiyiz)
 *
 * @bug No known bugs.
 **/

#ifndef WINDOW_FETCH_H_
#define WINDOW_FETCH_H_

//Add #define here


#include <hls_stream.h>         // Definition of the stream class

#include "axis.h"               // Definition of the AXIS protocol structure

const int MAX_WINDOW_DIM = 14; //log2, so max 4,096 bit image
const int MAX_KERNEL_DIM = 5; //log2, so max 31x32 kernel

template <typename IN_T, typename OUT_T, size_t IN_T_BITS, size_t OUT_T_BITS,
          int IMAGE_HEIGHT, int IMAGE_WIDTH, int KERNEL_HEIGHT, int KERNEL_WIDTH,
          OUT_T (*window_f)(IN_T window[KERNEL_HEIGHT][KERNEL_WIDTH], int start_row, int start_col)>
struct window_pipeline {


    //typedef OUT_T (*window_f)(IN_T window[KERNEL_HEIGHT][KERNEL_WIDTH]);
    //const window_f window_function;
    //rowbuffer idx
    ap_uint<MAX_WINDOW_DIM> head_row;
    ap_uint<MAX_WINDOW_DIM> tail_row; //The row we store into
    ap_uint<MAX_WINDOW_DIM> tail_col; //The col we store into -- equal to head_col
    ap_uint<1> init_row; //Are we doing the first row for the first time

    //window idx
    ap_uint<MAX_KERNEL_DIM> head_win;
    ap_uint<MAX_KERNEL_DIM> tail_win;

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

    typedef axis<IN_T, IN_T_BITS> in_pkt_t;
    typedef hls::stream<in_pkt_t> in_stream_t;

    typedef axis<OUT_T, OUT_T_BITS> out_pkt_t;
    typedef hls::stream<out_pkt_t> out_stream_t;

    // window operation
    void window_op(in_stream_t& in_stream, out_stream_t& out_stream) {
    #pragma HLS_INLINE

        in_pkt_t in_pkt;
        out_pkt_t out_pkt;

        int offset = KERNEL_WIDTH/2 + (IMAGE_WIDTH*(KERNEL_HEIGHT/2));
        int out_pointer = -offset; //Which pixel of the output image are we looking at?
        int row_out_pointer = 0;

        IN_T rowbuffer[KERNEL_HEIGHT][IMAGE_WIDTH];
        IN_T window[KERNEL_HEIGHT][KERNEL_WIDTH];
        #pragma HLS DEPENDENCE variable=rowbuffer inter RAW false
        #pragma HLS DEPENDENCE variable=window inter RAW false
        #pragma HLS ARRAY_PARTITION variable=window complete

        pixel_op: for (int in_pointer = 0; in_pointer < IMAGE_WIDTH * IMAGE_HEIGHT + offset; in_pointer++) {
        #pragma HLS PIPELINE II=1

            //Input Processing
            //Stop loading packets when we've loaded them all
            if (in_pointer < IMAGE_WIDTH*IMAGE_HEIGHT)
                in_stream >> in_pkt;
            rowbuffer[tail_row][tail_col] = in_pkt.tdata;

            //Window Forming:
            //If we have loaded in KERNEL_HEIGHT-1 full rows, we can load columns into the window
            col_to_window: if (in_pointer >= (KERNEL_HEIGHT-1)*IMAGE_WIDTH) {
                for (int i = 0; i < KERNEL_HEIGHT; i++) {
                    window[i][tail_win] = rowbuffer[i][tail_col]; //When i == tail_col we should really be forwarding, not loading again
                }
            }

            //Output Processing
            //Only care if output is inside the out window
            //In addition, if we're on an edge, top, bottom, left, or right, assign 0 packets
            if (out_pointer >= 0) {
                if (row_out_pointer < KERNEL_WIDTH/2 || //Left rows
                    row_out_pointer >= IMAGE_WIDTH - KERNEL_WIDTH/2 || //Right rows
                    out_pointer < IMAGE_WIDTH*(KERNEL_HEIGHT/2) || //Top rows
                    out_pointer >= (IMAGE_HEIGHT - KERNEL_HEIGHT/2) * IMAGE_WIDTH) { //Bottom rows
                    out_pkt.tdata = 0;
                    out_pkt.tlast = (out_pointer == IMAGE_WIDTH*IMAGE_HEIGHT-1) ? 1 : 0;
                    out_pkt.tkeep = -1;
                }
                else {
                    out_pkt.tdata = window_f(window, head_row, head_win);
                    out_pkt.tlast = 0;
                    out_pkt.tkeep = -1;
                }
                out_stream << out_pkt;
            }



            //update the rowbuffer head, tail position
            update_tail();
            update_head();

            //update the window head and tail
            tail_win = (tail_win + 1 == KERNEL_WIDTH) ? 0 : tail_win.to_int() + 1;
            head_win = (tail_win == head_win) ? ((head_win + 1 == KERNEL_WIDTH) ? 0 : head_win.to_int() + 1) : head_win.to_int();


            //update the in and out
            if (out_pointer >= 0) {
                row_out_pointer = (row_out_pointer + 1 == IMAGE_WIDTH) ? 0 : row_out_pointer + 1;
            }
            out_pointer = out_pointer + 1;

        }
     }



     // helper functions
     void update_tail(){
        if ((tail_col + 1) == IMAGE_WIDTH){
             //update row
             tail_row = (tail_row + 1 == KERNEL_HEIGHT) ? 0 : tail_row.to_int() + 1;
             init_row = 0;
        }
        tail_col = (tail_col + 1 == IMAGE_WIDTH) ? 0 : tail_col.to_int() + 1;
     }
     void update_head(){
         //update head_row
         if (tail_row == head_row && !init_row){
            head_row = (head_row + 1 == KERNEL_HEIGHT) ? 0 : head_row.to_int() + 1;
         }
     }
};

#endif /* WINDOW_FETCH_H_ */

