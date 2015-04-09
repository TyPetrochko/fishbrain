#include "opencv2/core/core.hpp"
#include "opencv2/flann/miniflann.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/video/video.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <math.h>
#include "faceDetection2.hpp"
#include <iostream>
#include <stdio.h>


using namespace std;
using namespace cv;


/** Global variables */
String face_cascade_name = "lbpcascade_frontalface.xml";
String eyes_cascade_name = "haarcascade_eye_tree_eyeglasses.xml";
CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;
String window_name = "Capture - Face detection";
/**
 * @function main
 */



#ifdef DEBUG
int main( void )
{
    VideoCapture capture;
    Mat frame;

    //-- 1. Load the cascade
    if( !face_cascade.load( face_cascade_name ) ){ printf("--(!)Error loading face cascade\n"); return -1; };
    if( !eyes_cascade.load( eyes_cascade_name ) ){ printf("--(!)Error loading eyes cascade\n"); return -1; };

    //-- 2. Read the video stream
    capture.open( -1 );
    if ( ! capture.isOpened() ) { printf("--(!)Error opening video capture\n"); return -1; }

    while ( capture.read(frame) )
    {
        if( frame.empty() )
        {
            printf(" --(!) No captured frame -- Break!");
            break;
        }

        float x, y;
	//-- 3. Apply the classifier to the frame
        detectFaces( frame, &x, &y, true);

        //-- bail out if escape was pressed
        int c = waitKey(10);
        if( (char)c == 27 ) { break; }
    }
    return 0;
}
#endif
/**
 * @function detectAndDisplay
 */

void initFaceDetection(){
    if( !face_cascade.load( face_cascade_name ) ){ printf("--(!)Error loading face cascade\n");};
    if( !eyes_cascade.load( eyes_cascade_name ) ){ printf("--(!)Error loading eyes cascade\n");};
	
}
void detectFaces( Mat frame, float *x, float *y, int display)
{
    std::vector<Rect> faces;
    Mat frame_gray;

    cvtColor( frame, frame_gray, COLOR_BGR2GRAY );
    equalizeHist( frame_gray, frame_gray );

    //-- Detect faces
    face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0, Size(80, 80) );
    puts("Detecting faces ");
    for( size_t i = 0; i < faces.size(); i++ )
    {
	puts("Found one");
        Mat faceROI = frame_gray( faces[i] );
        std::vector<Rect> eyes;

        //-- In each face, detect eyes
        eyes_cascade.detectMultiScale( faceROI, eyes, 1.1, 2, 0 |CASCADE_SCALE_IMAGE, Size(30, 30) );
        //if( eyes.size() == 2)
        if(1)
	{
            //-- Draw the face
            Point center( faces[i].x + faces[i].width/2, faces[i].y + faces[i].height/2 );
            if (display) {ellipse( frame, center, Size( faces[i].width/2, faces[i].height/2 ), 0, 0, 360, Scalar( 255, 0, 0 ), 2, 8, 0 );}
	    float relativeX, relativeY;
	    relativeX = (center.x-(frame.cols/2.0))/(frame.cols/2.0);
	    relativeY = ((frame.rows/2.0)-center.y)/(frame.rows/2.0);
	    *x = relativeX;
	    *y = relativeY;
            for( size_t j = 0; j < eyes.size(); j++ )
            { //-- Draw the eyes
                //Point eye_center( faces[i].x + eyes[j].x + eyes[j].width/2, faces[i].y + eyes[j].y + eyes[j].height/2 );
                //int radius = cvRound( (eyes[j].width + eyes[j].height)*0.25 );
                //circle( frame, eye_center, radius, Scalar( 255, 0, 255 ), 3, 8, 0 );
            }
        }

    }
    //-- Show what you got
    if (display) {imshow( window_name, frame );}
}
