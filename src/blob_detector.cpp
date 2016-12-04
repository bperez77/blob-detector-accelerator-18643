/**
 * @file blob_detector.cpp
 * @date Tuesday, November 01, 2016 at 04:44:05 PM EDT
 * @author Brandon Perez (bmperez)
 * @author Devon White (dww)
 * @author Yiyi Zhang (yiyiz)
 *
 * @bug No known bugs.
 **/

#include <cstdlib>                  // C standard library
#include <cstdio>                   // C standard I/O library
#include <cassert>                  // Assert macro

#include <xil_cache.h>              // Cache control functions
#include <xparameters.h>            // Auto-generated params for FPGA IP
#include <xaxidma.h>                // Functions and definitions for AXI CDMA
#include <xtime_l.h>                // Cycle counter timer
#include <ff.h>                     // Xilinx FAT filesystem interface
#include <ffconf.h>                 // Xilinx FAT filesystem configuration

#include "image.h"                  // Image definitions and the image type

/*----------------------------------------------------------------------------
 * Internal Definitions
 *----------------------------------------------------------------------------*/

// Controls whether or not verbose print statements are enabled
#define VERBOSE

// A macro to print an error message
#define log_err(msg, ...) printf("%s: %s: %d: Error: " msg, __FILE__, \
        __func__, __LINE__, ##__VA_ARGS__)

// A macro to enable print messages when DEBUG is defined
#ifdef VERBOSE
#define log_verbose(...) printf(__VA_ARGS__)
#else
#define log_verbose(...)
#endif

// Alias for an image containing 32-bit RGBA pixels
typedef matrix<pixel, IMAGE_WIDTH, IMAGE_HEIGHT> input_image_t;

// A structure representing an AXI DMA device, holds information about the IP
typedef struct axidma {
    int id;                         // ID used to identify the device
    XAxiDma_Config *config;         // Config parameters for the AXI DMA
    XAxiDma dev;                    // AXI DMA device structure
} axidma_t;

// A structure representing the context on the device
typedef struct device_context {
    FATFS sd_card_fs;               // Handle the the SD card filesystem (FAT)
    axidma_t axidma;                // The AXI DMA device
} devices_context_t;

// Maximum size any given path is allowed to be
static const size_t MAX_PATH_LEN    = 100;

// Shorten the clunky name for id defines for the AXI DMA devices
static const int AXIDMA_ID          = XPAR_INPUT_OUTPUT_DMA_DEVICE_ID;

// The path to the SD card for the f_mount function
static const TCHAR *SD_CARD_PATH    = "0:/";

/* The name of the input and output directories for the image files. IMPORTANT:
 * For any given file path, no component (file/directory) of the path can
 * exceed 8 characters (including the extension). Also, the extension cannot
 * exceed 3 characters. */
static const TCHAR *IMAGE_DIR_PATH  = "images";
static const TCHAR *OUTPUT_DIR_PATH = "output";

// Allocate global buffers for the input and output images
static image_t IMAGE;
static image_t OUTPUT_IMAGE;

/*----------------------------------------------------------------------------
 * Initialization
 *----------------------------------------------------------------------------*/

static int init_axidma(axidma_t& axidma, int device_id)
{
    // Lookup the configuration for the AXI DMA device
    axidma.id = device_id;
    axidma.config = XAxiDma_LookupConfig(axidma.id);
    if (axidma.config == NULL) {
        log_err("Unable to find AXI DMA device with id %d\n", device_id);
        return XST_DEVICE_NOT_FOUND;
    }

    int rc = XAxiDma_CfgInitialize(&axidma.dev, axidma.config);
    if (rc != XST_SUCCESS) {
        log_err("Unable to initialize the AXI DMA device with id %d\n",
                device_id);
        return rc;
    }

    // Disable all DMA interrupts for both channels, as we're polling
    XAxiDma_IntrDisable(&axidma.dev, XAXIDMA_IRQ_ALL_MASK,
            XAXIDMA_DEVICE_TO_DMA);
    XAxiDma_IntrDisable(&axidma.dev, XAXIDMA_IRQ_ALL_MASK,
                        XAXIDMA_DMA_TO_DEVICE);

    return XST_SUCCESS;
}

static int init_devices(devices_context_t& devices)
{
    // Mount the SD card as a FAT filesystem
    int rc = f_mount(&devices.sd_card_fs, SD_CARD_PATH, 0);
    if (rc != XST_SUCCESS) {
        log_err("Error: %s: Unable to mount SD card as a FAT filesystem",
                SD_CARD_PATH);
        return rc;
    }

    // Initialize the AXI DMA devices
    rc = init_axidma(devices.axidma, AXIDMA_ID);
    if (rc != XST_SUCCESS) {
        return rc;
    }

    return XST_SUCCESS;
}

/*----------------------------------------------------------------------------
 * File I/O Handling
 *----------------------------------------------------------------------------*/

static void join_paths(const char* path1, const char *path2, char *output_path,
        size_t output_size)
{
    assert(strlen(path1) + 1 + strlen(path2) <= output_size - 1);

    // Copy path one to the output
    size_t path1_len = strlen(path1);
    strncpy(output_path, path1, output_size);

    // Join path2 with a '/' character and copy path2
    output_path[path1_len] = '/';
    strncpy(output_path + path1_len + 1, path2, output_size - path1_len - 1);
    return;
}

static int open_image(const TCHAR *root_path, const TCHAR *tail_path,
        image_t& image)
{
    // Join the root and tail paths to get the path to the image
    TCHAR image_path[MAX_PATH_LEN+1];
    join_paths(root_path, tail_path, image_path, sizeof(image_path));

    // Try to open the specified image file
    FIL file;
    FRESULT rc = f_open(&file, image_path, FA_READ);
    if (rc != FR_OK) {
        log_err("%s: Unable to open input image file.\n", image_path);
        return rc;
    }

    // Check that the file is the expected size
    if (file_size(&file) != image.size()) {
        log_err("%s: File size does not match input image's. Expected %u "
                "bytes, but the file size is %lu.\n", image_path, image.size(),
                file.fsize);
        return XST_BUFFER_TOO_SMALL;
    }

    // Read an image's worth of data plus 1 byte to detect for too large files
    size_t bytes_read;
    rc = f_read(&file, image.buffer, image.size(), &bytes_read);
    assert(bytes_read == image.size());
    if (rc != FR_OK) {
        log_err("%s: Unable to read input image.\n", image_path);
        return rc;
    }

    // Close the file
    rc = f_close(&file);
    if (rc != FR_OK) {
        log_err("%s: Unable to close input image.\n", image_path);
        return rc;
    }

    return XST_SUCCESS;
}

static int save_image(const TCHAR *root_path, const TCHAR *tail_path,
        const image_t& image)
{
    // Join the root and tail paths to get the path to the image
    TCHAR image_path[MAX_PATH_LEN+1];
    join_paths(root_path, tail_path, image_path, sizeof(image_path));

    // Try to open the specified output file
    FIL file;
    FRESULT rc = f_open(&file, image_path, FA_CREATE_ALWAYS|FA_WRITE);
    if (rc != FR_OK) {
        log_err("%s: Unable to open output image file.\n", image_path);
        return rc;
    }

    // Write the entire image to the file, and verify that it was successful
    size_t bytes_written;
    rc = f_write(&file, image.buffer, image.size(), &bytes_written);
    if (rc != FR_OK) {
        log_err("%s: Unable to write output image to file.\n", image_path);
        return rc;
    } else if (bytes_written != image.size()) {
        log_err("%s: File size does not match output image's. Expected %u "
                "bytes, but the file size is %lu.\n", image_path, image.size(),
                file.fsize);
        return XST_BUFFER_TOO_SMALL;
    }

    // Close the output file
    rc = f_close(&file);
    if (rc != FR_OK) {
        log_err("%s: Unable to close output image file.\n", image_path);
        return rc;
    }

    return XST_SUCCESS;
}

/*----------------------------------------------------------------------------
 * Main Application
 *----------------------------------------------------------------------------*/

static int send_image(axidma_t& axidma, const image_t& image,
        image_t& output_image)
{
    // Get a handle to the AXI DMA device
    XAxiDma* dma_dev = &axidma.dev;

    // Initiate the transfer to receive the output image from the FPGA
    int rc = XAxiDma_SimpleTransfer(dma_dev, (u32)output_image.buffer,
            output_image.size(), XAXIDMA_DEVICE_TO_DMA);
    if (rc != XST_SUCCESS) {
        log_err("Unable to start image transfer over AXI DMA from the FPGA.\n");
        return rc;
    }

    // Initiate the transfer to send the image to the FPGA
    rc = XAxiDma_SimpleTransfer(dma_dev, (u32)image.buffer, image.size(),
            XAXIDMA_DMA_TO_DEVICE);
    if (rc != XST_SUCCESS) {
        log_err("Unable to start image transfer over AXI DMA to the FPGA.\n");
        return rc;
    }

    /* Wait for both the transfers to complete. We really only need to wait on the
     * receive transfer, as it depends on the transmit transfer. */
    while (XAxiDma_Busy(dma_dev, XAXIDMA_DMA_TO_DEVICE) &&
            XAxiDma_Busy(dma_dev, XAXIDMA_DEVICE_TO_DMA));

    return XST_SUCCESS;
}

static int blob_detection(axidma_t axidma, const TCHAR *image_dir,
        const TCHAR *image_name, const TCHAR *output_dir)
{
    // Get handles to the global buffers for the images
    image_t& image = IMAGE;
    image_t& output_image = OUTPUT_IMAGE;

    // Open the image file, and load it into memory
    log_verbose("\tOpening the image file in '%s'...\n", image_dir);
    int rc = open_image(image_dir, image_name, image);
    if (rc != XST_SUCCESS) {
        return rc;
    }

    /* Use the hardware on the FPGA to perform an operation on the image,
     * sending it out, and receiving a new output image. */
    log_verbose("\tTransferring the image over the fabric...\n");
    rc = send_image(axidma, image, output_image);
    if (rc != XST_SUCCESS) {
        return rc;
    }

    // Save the image file
    log_verbose("\tSaving the image to file '%s'...\n", output_dir);
    rc = save_image(output_dir, image_name, output_image);
    if (rc != XST_SUCCESS) {
        return rc;
    }

    return XST_SUCCESS;
}

static int run_blob_detections(axidma_t& axidma, const TCHAR* image_dir_path,
        DIR *image_dir, const TCHAR *output_dir_path)
{
    // Read and process each image file in the directory
    while (true) {
        // Get the next file in the input image directory, stop when none left
        FILINFO file_info;
        FRESULT f_rc = f_readdir(image_dir, &file_info);
        if (f_rc != FR_OK) {
            log_err("%s: Unable to read the next file in the input image "
                    "directory.\n", image_dir_path);
            return f_rc;
        } else if (strlen(file_info.fname) == 0) {
            return XST_SUCCESS;
        }

        // Run blob detection on this image, and save it to its output file
        printf("\nRunning blob detection on file '%s' in '%s'...\n",
                file_info.fname, image_dir_path);
        int rc = blob_detection(axidma, image_dir_path, file_info.fname,
                output_dir_path);
        if (rc != XST_SUCCESS) {
            return rc;
        }
    }
}

int main()
{
    printf("\nBlob Detector Accelerator\n");
    printf("-------------------------\n");

    // Initialize the system, namely the SD card and AXI DMA
    printf("Mounting the SD card and initializing AXI DMA devices...\n");
    devices_context_t devices_context;
    int rc = init_devices(devices_context);
    if (rc != XST_SUCCESS) {
        return rc;
    }

    // Open the input directory containing the images
    FRESULT f_rc;
    DIR input_dir;
    f_rc = f_opendir(&input_dir, IMAGE_DIR_PATH);
    if (f_rc != FR_OK) {
        log_err("%s: Unable to open input image directory.\n", IMAGE_DIR_PATH);
        return f_rc;
    }

    // Create the output directory if it doesn't already exist
    f_rc = f_mkdir(OUTPUT_DIR_PATH);
    if (f_rc != FR_OK && f_rc != FR_EXIST) {
        log_err("%s: Unable to create output directory.\n", OUTPUT_DIR_PATH);
        return rc;
    }

    // Run blob detection on all the images in the input directory
    rc = run_blob_detections(devices_context.axidma, IMAGE_DIR_PATH, &input_dir,
            OUTPUT_DIR_PATH);
    if (rc != XST_SUCCESS) {
        return rc;
    }

    printf("\nBlob detection is complete. Output images can be found in %s.\n",
            OUTPUT_DIR_PATH);
    return XST_SUCCESS;
}
