# Fisheye Project #

Using a fisheye camera and facial tracking to achieve "window-like" video chats, where either party can move his/her head to glance around the opposite party's room.

Check out [Fishbrain Section](#Fishbrain)

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

To run this program, attack a fisheye camera to your webcam (make sure it's centered), navigate to binary folder, and type:

    ./fishbrain

You should see a working prototype of the algorithm.

## Fishbrain ##

The fishbrain algorithm uses intersecting equatorial lines to determine a confined area of the fisheye image, which it resizes to a rectangle. This resized rectangle represents the viewframe of a traditional rectilinear webcam operating in the 180-degree domain captured by the fisheye camera.

As an example, consider the following image from Stanford University:

![Equatorial lines diagram]
(http://solar-center.stanford.edu/images/sungrid-0.gif)

Consider the shape formed by the intersection of the longitudinal 30, longitudinal 45, latitudinal 15, and latitudinal 30 curves. They almost form a perfect rectangle - but not quite, because of the curvature of the left and right sides.

If you "cut out" this slice from the page, and then "stretched" it by pulling the corners - you could probably manipulate it into the general shape of a rectangle (pretend it's made out of latex rather than paper).

This is almost exactly what fishbrain does *once every frame*.

However, notice that, while longitudinal lines form a complete equator going around the globe, i.e. dividing the sphere in half perfectly, while the latitudinal lines do *not* perfectly halve the globe. In our algorithm, the latitudinal lines are equatorial in nature, meaning that they converge on an "east" and "west" pole located at the far left and far right of the circle formed by the fisheye image.

If a fisheye image is overlaid over this equatorial grid, these slices accurately represent warped traditional rectilinear photographs because that's exactly what they are. You can think of a fisheye lens as a *whole bunch of rectilinear photographs "smushed" together*. Thus - resizing them "pans" a virtual webcam to that position in the fisheye image.

As you can imagine, you're "scaling up" an image in size, so some degradation in quality will occur. This is natural, and the most effective solution is to use a better camera and fisheye lens.

But which equatorial slices of the fisheye image do we reposition? We can interpret this question as "where do we initially pan the virtual rectilinear webcam to."

At first, the one smack dab in the middle of the fisheye image. This isn't very interesting, since it's essentially just reproducing a regular webcam in lower quality.

But this is also where facial tracking comes in. We continually run a [Haar Cascade Classifier](http://docs.opencv.org/modules/objdetect/doc/cascade_classification.html) trained to detect human faces in the fisheye image (with a little bit of stretching to account for the warping around the edges). When a face is detected at point (x, y), the equatorial slice with (-x, -y) is targeted by the repositioning algorithm. This pans the viewframe of the virtual webcam to the "mirror" orientation of the line drawn between the user's face and the webcam. Thus - when the user moves his head to the left - it well show the *left* side of the room. This mimics a mirror!

## Moving Forward ##

Here are my next few goals:

1) Refractor/comment algorithm.
2) Speed up the algorithm. Since it uses a whole bunch of matrix transformations, [cache optimization](http://www.cc.gatech.edu/~bader/COURSES/UNM/ece637-Fall2003/papers/KW03.pdf) might make a huge difference.
3) Build [networking portion](#Networking).

## Networking ##

The ultimate goal of fishbrain is to allow videochats that simulate a spatial conversation in real-time between users A and B. When user A moves his head to the left, he can see the right side of user B's room. Thus - both users can imagine that the plane of their computer screens are actually windows into the opposite user's room.
