/**
 * @file axis.h
 * @date Sunday, October 30, 2016 at 05:07:14 PM EDT
 * @author Brandon Perez (bmperez)
 * @author Devon White (dww)
 * @author Yiyi Zhang (yiyiz)
 *
 * @bug No known bugs.
 **/

#ifndef AXIS_H_
#define AXIS_H_

#include <ap_int.h>         // Arbitrary precision integers type and functions

/* The number of bits per byte, and a Macro to convert bits to bytes, rounding
 * up the value. */
#define BITS_PER_BYTE       8
#define bits_to_bytes(bits) ((bits + BITS_PER_BYTE - 1) / (BITS_PER_BYTE))

/* Template to define a type that represents a single packet of the AXIS
 * protocol. T can be any type, and DATA_BITS should be the number of bits
 * needed to represent the given type. */
template <typename T, size_t T_BITS>
struct axis {
    T tdata;                                // The data of the packet
    ap_uint<bits_to_bytes(T_BITS)> tkeep;   // Which bytes of the data are valid
    ap_uint<1> tlast;                       // Indicates the last packet

    // Default constructor
    axis() {}

    axis(T tdata, ap_uint<1> tlast, ap_uint<bits_to_bytes(T_BITS)> tkeep=-1) {
        this->tdata = tdata;
        this->tlast = tlast;
        this->tkeep = tkeep;
    }
};

#endif /* AXIS_H_ */
