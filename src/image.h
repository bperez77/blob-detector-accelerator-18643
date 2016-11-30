/**
 * @file image.h
 * @date Tuesday, November 29, 2016 at 04:53:14 PM EST
 * @author Brandon Perez (bmperez)
 * @author Devon White (dww)
 * @author Yiyi Zhang (yiyiz)
 *
 * This file contains the definition of an image type.
 *
 * This contains the definitions to represent a complete image that is in RGBA
 * format. This is templated on the image size, and is mainly for convenience
 * when iterating over it.
 *
 * @bug No known bugs.
 **/

#ifndef IMAGE_H_
#define IMAGE_H_

#include <stdint.h>             // Fixed-size integer types

// The size of the images we're going to be processing
static const int IMAGE_WIDTH    = 1920;
static const int IMAGE_HEIGHT   = 1080;

typedef struct pixel {
    uint8_t red;                // Red channel of the pixel
    uint8_t blue;               // Blue channel of the pixel
    uint8_t green;              // Green channel of the pixel
    uint8_t alpha;              // Alpha channel of the pixel

    // Default constructor for the pixel
    pixel() {}

    // Constructor for the pixel from constant integer values
    pixel(int red, int green, int blue, int alpha) {
        this->red = red;
        this->green = green;
        this->blue = blue;
        this->alpha = alpha;
    }
} pixel_t;

// A structure that represents a matrix of values, or a 2D-array
template <typename T, int ROWS, int COLS>
struct matrix {
    T buffer[ROWS][COLS];       // The buffer holding the image

    // Return the number of rows in the image
    int rows() const
    {
        return ROWS;
    }

    // Return the number of columns in the image
    int cols() const
    {
        return COLS;
    }

    // Return the size necessary to hold the image in memory, in bytes
    size_t size() const
    {
        return rows() * cols() * sizeof(T);
    }
};

// Alias for an image containing 32-bit RGBA pixels
typedef matrix<pixel, IMAGE_WIDTH, IMAGE_HEIGHT> image_t;

#endif /* IMAGE_H_ */
