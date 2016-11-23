/**
 * @file monochrome.cpp
 * @date Wednesday, November 23, 2016 at 01:50:16 PM EST
 * @author Brandon Perez (bmperez)
 *
 * This file contains the implementation of the monochrome module.
 *
 * The monochrome module simply takes an 8-bit grayscale input stream, and uses
 * the defined threshold to convert it to a 1-bit monochrome value.
 *
 * @bug No known bugs.
 **/

#include "grayscale.h"      // Definition of grayscale types
#include "monochrome.h"     // Our interface and grayscale types

/*----------------------------------------------------------------------------
 * Internal Definitions
 *----------------------------------------------------------------------------*/

/**
 * The threshold value used to convert grayscale. If the grayscale value is
 * greater than or equal to the threshold it becomes 1, otherwise, it becomes 0.
 **/
const grayscale_t MONOCHROME_THRESHOLD = 0.85 * 255;

/*----------------------------------------------------------------------------
 * Monochrome Module
 *----------------------------------------------------------------------------*/

/**
 * Converts the grayscale into a binary monochrome value. This is the
 * combinational interface to the module.
 *
 **/
monochrome_t compute_monochrome(const grayscale_t& grayscale)
{
    return grayscale >= MONOCHROME_THRESHOLD;
}

/**
 * Converts the stream of grayscale values into monochrome, using the monochrome
 * function. This is the sequential interface to the module.
 **/
void monochrome(grayscale_stream_t& grayscale_stream,
        monochrome_stream_t& monochrome_stream) {
#pragma HLS INLINE

    // Read in the next grayscale value packet
    grayscale_axis_t grayscale_axis_pkt;
    grayscale_stream >> grayscale_axis_pkt;

    // Compute the monochrome value, and send the value downstream
    monochrome_axis_t monochrome_axis_pkt;
    monochrome_axis_pkt.tdata = compute_monochrome(grayscale_axis_pkt.tdata);

    /* Our transfers are always aligned, so set tkeep to -1, and assert
     * tlast when we reach the last packet. */
    monochrome_axis_pkt.tkeep = -1;
    monochrome_axis_pkt.tlast = monochrome_axis_pkt.tlast;

    // Stream out the grayscale packet
    monochrome_stream << monochrome_axis_pkt;
    return;

}

/*----------------------------------------------------------------------------
 * Top Function for Synthesis
 *----------------------------------------------------------------------------*/

void monochrome_top(grayscale_stream_t& grayscale_stream,
        monochrome_stream_t& monochrome_stream) {
#pragma HLS INTERFACE axis port=grayscale_stream
#pragma HLS INTERFACE axis port=monochrome_stream

#pragma HLS PIPELINE II=1 rewind

    monochrome(grayscale_stream, monochrome_stream);
    return;
}

