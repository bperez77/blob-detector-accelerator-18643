/**
 * @file monochrome.h
 * @date Wednesday, November 23, 2016 at 01:35:55 PM EST
 * @author Brandon Perez (bmperez)
 *
 * This file contains the interface to the monochrome module.
 *
 * This defines the monochrome module interface, as both a sequential
 * (streaming) interface, and a combinational interface. It also defines the
 * input and outptut types for the module.
 *
 * @bug No known bugs.
 **/

#ifndef MONOCHROME_H_
#define MONOCHROME_H_

#include <hls_stream.h>             // Definition of the hls::stream class
#include <ap_int.h>                 // Arbitrary precision integer types

#include "axis.h"                   // Definition of the AXIS protocol structure
#include "grayscale.h"              // Definition of the grayscale types

/*----------------------------------------------------------------------------
 * Definitions
 *----------------------------------------------------------------------------*/

/**
 * The input stream type is a grayscale AXIS packet. The output stream type is a
 * 1-bit monochrome value AXIS packet.
 ***/
typedef ap_uint<1> monochrome_t;
typedef axis<monochrome_t, 1> monochrome_axis_t;
typedef hls::stream<monochrome_axis_t> monochrome_stream_t;

/*----------------------------------------------------------------------------
 * Interface
 *----------------------------------------------------------------------------*/

/**
 * Converts the grayscale value into a binary monochrome value, simply
 * thresholding on the grayscale value.
 *
 * This is the combinational interface to the module.
 *
 * @param[in] grayscale The grayscale value to convert to monochrome.
 * @return The 1-bit monochrome value of the grayscale value.
 **/
monochrome_t compute_monochrome(const grayscale_t& grayscale);

/**
 * Converts the grayscale input stream into an output monochrome stream, by
 * thresholding the grayscale values to convert them to a binary monochrome
 * value.
 *
 * This is the sequential interface to the module.
 *
 * @param[in] grayscale_stream The input stream of grayscale values.
 * @param[out] monochrome_stream The output stream of monochrome values.
 **/
void monochrome(grayscale_stream_t& grayscale_stream,
        monochrome_stream_t& monochrome_stream);

#endif /* MONOCHROME_H_ */
