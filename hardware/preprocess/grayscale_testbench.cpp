/**
 * @file grayscale_testbench.cpp
 * @date Sunday, October 30, 2016 at 06:45:03 PM EDT
 * @author Brandon Perez (bmperez)
 *
 * @bug No known bugs.
 **/

#include <assert.h>         // Assert macro

#include "grayscale.h"      // Grayscale interface and definitions

// TODO: Make more robust
int main()
{
	// Input and output streams for grayscale
	pixel_stream_t pixel_stream;
	grayscale_stream_t grayscale_stream;

	// Setup the pixel AXIS packet
    pixel_axis_t pixel_axis_pkt;
    pixel_axis_pkt.tdata = 0x00010203;
    pixel_axis_pkt.tkeep = -1;
    pixel_axis_pkt.tlast = 1;

    // Stream in the pixel packet, call grayscale, and get the output
    grayscale_axis_t grayscale_axis_pkt;
    pixel_stream << pixel_axis_pkt;
    grayscale(pixel_stream, grayscale_stream);
    grayscale_stream >> grayscale_axis_pkt;

    // Verify that the output is correct
    assert(grayscale_axis_pkt.tdata.to_int() == 2);
    assert(grayscale_axis_pkt.tkeep.to_int() == -1);
    assert(grayscale_axis_pkt.tlast.to_int() == 1);

    return 0;
}
