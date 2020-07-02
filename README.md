# camlib

This repo contains the source code for the static libraries that the MiniWinCamera example projects use to drive a video camera. For the embedded examples this is an OV7670 camera module without a buffer. For the Windows/Linux examples it is the first camera found on the system. The Linux/Windows versions use OpenCV version 4.30. The camera installed on the system must be supported by this package.

The static library binaries produced by compiling the source code found here are included in the MiniWin repo. It is not necessary to obtain this repo and build the code if using the MiniWinCamera example projects. Documentation on the embedded examples' connections required and the resources used are in Appendix 4 of the main MiniWin documentation under the miniwinwm/MiniWin/docs folder found here...

https://github.com/miniwinwm/miniwinwm/tree/master/MiniWin/docs
