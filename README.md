# Fisheye Project #

Using a fisheye camera and facial tracking to achieve "window-like" video chats, where either party can move his/her head to glance around the opposite party's room.

## Installation ##

1) Follow tutorial here http://docs.opencv.org/doc/tutorials/introduction/linux_install/linux_install.html

2) To compile, use 

    make fishbrain

or the following..

    g++ -I/usr/local/include/opencv -I/usr/local/include/opencv2 -L/usr/local/lib/ -g -o binary  main.cpp -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_video -lopencv_features2d -lopencv_calib3d -lopencv_objdetect -lopencv_contrib -lopencv_legacy -lopencv_stitching

In the above command, binary is the file you output to (in this case binary/fishbrain) and main.cpp is the name of the source file.

For confusion, visit:
http://answers.opencv.org/question/25642/how-to-compile-basic-opencv-program-in-c-in-ubuntu/

3) If an error regarding location of OpenCV, look here: http://stackoverflow.com/questions/19164343/opencv-libopencv-core-so-2-4-cannot-open-shared-object-file-no-such-file-or

## Running ##

To run this program, attack a fisheye camera to your webcam, navigate to binary folder, and type:

    ./fishbrain

You should see a working prototype of the algorithm.
