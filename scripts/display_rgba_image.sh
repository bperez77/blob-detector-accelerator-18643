# display_rgba_image.sh
#
# Date: Thursday, December 01, 2016 at 11:29:49 PM EST
# Author: Brandon Perez (bmperez)
#
# Display an image that is in 8-bit color raw RGBA format. The user must
# specify the width and height of the image, since the file does not
# conatin this information.

# Check that number of command line arguments matches
num_args=$#
if [ ${num_args} -ne 3 ]; then
    printf "Error: Improper number of command line arguments.\n"
    printf "Usage: image_to_rgba.sh <width> <height> <image>\n"
    exit 1
fi

# Parse the command line arguments
width=$1
height=$2
image=$3

# Display the image, indicating that its format is raw RGBA
display -depth 8 -size ${width}x${height} ${image}
