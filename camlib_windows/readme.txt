Install msys64 with the OpenCV version 4.30 libraries.

Compile camlib with g++ using this command line:
	g++ -c -g -IC:\msys64\mingw64\include\opencv4 video/camlib_opencv.cpp

Compile camlib with cl from VC++ using this command line: (must be run in x64 native tools VS command prompt)
	cl -c -D_WIN32 -EHsc -IC:\msys64\mingw64\include\opencv4 video/camlib_opencv.cpp

Link for mingw64 with these libraries/objects included using g++, not gcc:
	camlib_opencv.o -lopencv_core.dll -lopencv_imgproc.dll -lopencv_videoio.dll

Link for VC++ with these libraries/objects included:
	camlib_opencv.obj lib\opencv_world430.lib
File opencv_world430.dll must be available to the executable.
