The following libraries need installing with sudo apt install...

libopencv-viz-dev
libopencv-videostab-dev
libopencv-superres-dev
libopencv-stitching-dev
libopencv-videoio-dev
libopencv-calib3d-dev
libopencv-objdetect-dev
libopencv-photo-dev
libopencv-shape-dev

Compile with this line...

g++ -c -g -I/usr/include/opencv2 ./Video/camlib_opencv.cpp
