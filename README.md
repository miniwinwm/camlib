# camlib

This repo contains the source code for the static libraries that the MiniWinCamera example projects use to drive a video camera. For the embedded examples this is an OV7670 camera module without a buffer. For the Windows/Linux examples it is the first camera found on the system. The Linux/Windows versions use OpenCV version 4.30. The camera installed on the system must be supported by this package.

The API to all these drivers is common. There are 3 functions:

    void camlib_init(void);             // initialise the driver, call once at start-up
    void camlib_capture(void);          // capture a QQVGA image 160 x 120 pixels
    uint16_t *camlib_get_frame(void)    // return a pointer to a static uint16_t[160][120] array in camlib containing the image
  
The image is in RGB565 format, 16 bits per pixel.

Function call camlib_init() for Windows and Linux platforms is slow and can take a few seconds to complete.
  
All the embedded examples using bit banging to read the camera. This is slow and not the best way to drive a video camera but was chosen for the MiniWinCamera embedded examples for these reasons:

1) Not all the MiniWin embedded hardware variants' processors have an on-chip camera peripheral (eg LPC54628)
2) If the processor does have a camera peripheral the pins might not be available on the development board used (eg STM32F429-DISC1 - the pins clash with the touch screen pins).
3) The code can be written to be almost common across all embedded hardware variants. Camera drivers are time consuming to write!

The static library binaries produced by compiling the source code found here are included in the MiniWin repo. It is not necessary to obtain this repo and build the code if using the MiniWinCamera example projects. 

Documentation on the embedded examples' connections required and the resources used are in Appendix 4 of the main MiniWin documentation under the miniwinwm/MiniWin/docs folder found here...

https://github.com/miniwinwm/miniwinwm/tree/master/MiniWin/docs
