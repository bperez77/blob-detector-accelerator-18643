# image_to_rgba.sh
#
# Date: Thursday, December 01, 2016 at 11:24:00 PM EST
# Author: Brandon Perez (bmperez)
#
# Converts a given image (e.g. PNG, JPG, etc.) to its raw RGBA format, creating
# a new file. Raw RGBA is 8-bits of color for each channel, 32-bits for each
# pixel. The LSB is red, and the MSB is alpha.

# Check that number of command line arguments matches
num_args=$#
if [ ${num_args} -ne 2 ]; then
    printf "Error: Improper number of command line arguments.\n"
    printf "Usage: image_to_rgba.sh <input_image> <output_image>\n"
    exit 1
fi

# Parse the command line arguments
input_image=$1
output_image=$2

# Convert the input image to 8-bit RGBA image
convert -format rgba -depth 8 ${input_image} ${output_image}
