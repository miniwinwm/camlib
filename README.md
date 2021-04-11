# camlib

This repo contains the source code for the static libraries that the MiniWinCamera Windows, Linux and Raspberry Pi OS example projects use to drive a video camera. It is the first camera found on the system that is chosen. This library uses OpenCV. The camera installed on the system must be supported by this package.

The API to all these drivers is common. There are 3 functions:

    void camlib_init(void);             // initialise the driver, call once at start-up
    void camlib_capture(void);          // capture a QQVGA image 160 x 120 pixels
    uint16_t *camlib_get_frame(void)    // return a pointer to a static uint16_t[160 * 120] array in camlib containing the image
  
The image is in RGB565 format, 16 bits per pixel.

Function call camlib_init() is slow and can take a few seconds to complete.
  
The static library binaries produced by compiling the source code found here are included in the MiniWin repo. It is not necessary to obtain this repo and build the code if using the MiniWinCamera example projects. 

