Install OpenCV version 4.30.

Compile camlib with this command:
	g++ -c -g -I/usr/include/opencv4 ./Video/camlib_opencv.cpp

Link with these libraries/objects included using g++, not gcc:
	camlib_opencv.o -lopencv_core -lopencv_videoio -lopencv_imgproc

	
