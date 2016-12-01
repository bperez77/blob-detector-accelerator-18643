/**
 * @file blob_detector.cpp
 * @date Tuesday, November 01, 2016 at 04:44:05 PM EDT
 * @author Brandon Perez (bmperez)
 * @author Devon White (dww)
 * @author Yiyi Zhang (yiyiz)
 *
 * @bug No known bugs.
 **/

#include <cstdlib>              // C standard library
#include <cstdio>               // C standard I/O library
#include <cassert>              // Assert macro

#include <xil_cache.h>          // Cache control functions
#include <xparameters.h>        // Auto-generated params for FPGA IP
#include <xaxidma.h>            // Functions and definitions for AXI CDMA
#include <xtime_l.h>            // Cycle counter timer
#include <ff.h>                 // Xilinx FAT filesystem interface

#include "image.h"              // Image definitions and the image type

/*----------------------------------------------------------------------------
 * Internal Definitions
 *----------------------------------------------------------------------------*/

// A macro to print an error message
#define log_err(msg, ...) printf("%s: %s: %d: Error: " msg, __FILE__, \
        __func__, __LINE__, ##__VA_ARGS__)

// Shorten the clunky name for id defines for the AXI DMA devices
static const int AXIDMA_ID = XPAR_IMAGE_DMA_DEVICE_ID;

// The path to the SD card for the f_mount function
static const TCHAR *SD_CARD_PATH = "0:/";

/* The name of the image file. IMPORTANT: For any given file path, no component
 * (file/directory) of the path can exceed 8 characters (including the extension).
 * Also, the extension cannot exceed 3 characters. */
static const TCHAR *IMAGE_FILE_PATH = "im.rgb";

// A structure representing an AXI DMA device, holds information about the IP
typedef struct axidma {
    int id;                     // ID used to identify the device
    XAxiDma_Config *config;     // Config parameters for the AXI DMA
    XAxiDma dev;                // CDMA device structure
} axidma_t;

// A structure representing the context on the device
typedef struct device_context {
    FATFS sd_card_fs;           // Handle the the SD card filesystem (FAT)
    axidma_t axidma;            // The AXI DMA device
} devices_context_t;

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

extern FRESULT follow_path (DIR* dp, const TCHAR* path);

static int open_image(const char *image_path, image_t& image)
{
    // Try to open the specified image file
    FIL file;
    int rc = f_open(&file, image_path, FA_READ);
    if (rc != FR_OK) {
        log_err("%s: Unable to open image file.\n", image_path);
        return rc;
    }

    // Check that the file is the expected size
    if (file.fsize != image.size()) {
        log_err("%s: File size does not match image's. Expected %u bytes, but "
                "the file size is %lu.\n", image_path, image.size(), file.fsize);
        return XST_BUFFER_TOO_SMALL;
    }

    // Read an image's worth of data plus 1 byte to detect for too large files
    size_t bytes_read;
    rc = f_read(&file, image.buffer, image.size(), &bytes_read);
    assert(bytes_read == image.size());
    if (rc != FR_OK) {
        log_err("%s: Unable to read file.\n", image_path);
        return rc;
    }

    // Close the file
    rc = f_close(&file);
    if (rc != FR_OK) {
        log_err("%s: Unable to close file.\n", image_path);
        return rc;
    }

    return XST_SUCCESS;
}


/*----------------------------------------------------------------------------
 * Main Application
 *----------------------------------------------------------------------------*/

// Declare the image, allocating room for its buffer (stack is too small)
static image_t TEST_IMAGE;

int main()
{
    printf("Blob Detector Accelerator\n");
    printf("-------------------------\n\n");

    // Initialize the system, namely the SD card and AXI DMA
    printf("Mounting the SD card and initializing AXI DMA devices...\n");
    devices_context_t devices_context;
    int rc = init_devices(devices_context);
    if (rc != XST_SUCCESS) {
        return rc;
    }

    // Open the image file, and load it into memory
    image_t& image = TEST_IMAGE;
    rc = open_image(IMAGE_FILE_PATH, image);
    if (rc != XST_SUCCESS) {
        return rc;
    }

    return 0;
}
