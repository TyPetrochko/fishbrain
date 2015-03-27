// FisheyeProject.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"
#include <iostream>
#include <fstream>	
#include "opencv2/core/core.hpp"
#include "opencv2/flann/miniflann.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/photo/photo.hpp"
#include "opencv2/video/video.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/ml/ml.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/contrib/contrib.hpp"
#include "opencv2/core/core_c.h"
#include "opencv2/highgui/highgui_c.h"
#include "opencv2/imgproc/imgproc_c.h"
#include <math.h>

#define ESCAPE_KEY (27)

#define RIGHT_ARROW (83)
#define LEFT_ARROW (81)
#define UP_ARROW (82)
#define DOWN_ARROW (84)

#define TOP_HORIZON_INITIAL (.6f)
#define BOTTOM_HORIZON_INITIAL (-.6f)
#define LEFT_HORIZON_INITIAL (-.6f)
#define RIGHT_HORIZON_INITIAL (.6f)

using namespace std;
using namespace cv;

float clamp(float n, float lower, float upper){
    return std::max(lower, std::min(n, upper));
}

Mat getcap() {
	VideoCapture cap(0);
	Mat image;
	cap >> image;
	return image;

}

void showimg(Mat image){
	if (!image.data){
		cout << "no picture";
		return;
	}

	namedWindow("Display", WINDOW_AUTOSIZE);
	imshow("Display", image);
	waitKey(0);
}

Mat editimage(Mat image){
	

	float height = image.rows;
	float width = image.cols;
	float xradius = height / 2;
	//clone image to "toreturn" so we can modify them separately
	Mat toreturn = image.clone();

	for (int x = 0; x < width - 1; x++){
		for (int y = 0; y < height - 1; y++){
			//******Adustment to image*************
			//calculate pixel's distance from center
			float centdistx = abs((width / 2) - x);
			float centdisty = abs((height / 2) - y);
			//calculate  if sqrt(x^2 + y^2) <= r (multiply x or y to skew accordingly)
			float xbounds = sqrt(pow(xradius, 2) - pow(centdisty, 2));

			if (centdistx <= xbounds){
				//do something to make effect visible
				//image.at<Vec3b>(y, x).val[0] = 0;
				//image.at<Vec3b>(y, x).val[1] = 127;
			}

			//******Adustment to toreturn*************
			//calculate the positive or negative x value
			float relxcoord = x - (width / 2);
			//calculate it's equivalent on the filled-in circle in the middle
			float equivx = (relxcoord / (width / 2))*xbounds + (width / 2);

			//making sure it's inbounds
			if (equivx > 0 && equivx <= width){
				toreturn.at<Vec3b>(y, x) = image.at<Vec3b>(y, equivx);
			}
		}
	}
	return toreturn;
}

Mat drawequators(Mat image, float horizontalSpan, float verticalSpan){
    // When horizontalSpan is 0, it draws a vertical line
    // When horizontalSpan is some value between 0 and 1, it draws
    // an equator somewhere between a vertical line and a full circle
    // (essentially a "smushed" circle in the x-direction)

    float height = image.rows;
    float width = image.cols;
    int rad = (int) (height / 2.0); // Assume circle fills to top of screen
    int topOfScreen = (int) (height / 2.0);
    //clone image to "toreturn" so we can modify them separately
    Mat toreturn = image.clone();
    for (int x = -rad; x < rad; x++){

	float xcoord = (width/2.0)+x;
	float ycoord = topOfScreen-sqrt(rad*rad-(x*x));
	
	float modifiedXcoord = (width/2.0)+horizontalSpan*x;
	float modifiedYcoord = topOfScreen-verticalSpan*sqrt(rad*rad-(x*x));

	image.at<Vec3b>(modifiedYcoord, xcoord).val[0] = 0;
	image.at<Vec3b>(modifiedYcoord, xcoord).val[1] = 0;
	image.at<Vec3b>(modifiedYcoord, xcoord).val[2] = 0;

	image.at<Vec3b>(ycoord, modifiedXcoord).val[0] = 0;
	image.at<Vec3b>(ycoord, modifiedXcoord).val[1] = 0;
	image.at<Vec3b>(ycoord, modifiedXcoord).val[2] = 0;

	image.at<Vec3b>(height-modifiedYcoord, xcoord).val[0] = 0;
	image.at<Vec3b>(height-modifiedYcoord, xcoord).val[1] = 0;
	image.at<Vec3b>(height-modifiedYcoord, xcoord).val[2] = 0;
	
	image.at<Vec3b>(height-ycoord, modifiedXcoord).val[0] = 0;
	image.at<Vec3b>(height-ycoord, modifiedXcoord).val[1] = 0;
	image.at<Vec3b>(height-ycoord, modifiedXcoord).val[2] = 0;

    }
    return toreturn;
}

Mat sectionWarp(Mat src, float bottomHorizon, float topHorizon, float leftHorizon, float rightHorizon){
    // Think of each horizon as being one of the lines in drawequators

    float height = src.rows;
    float width = src.cols;
    float rad = (height / 2.0f); // Assume circle fills to top of screen
    
    
    
    float topOfScreen = (height / 2.0f);
    Mat toreturn = src.clone();
    for (int x = -(width/2.0f); x < (width/2.0f); x++){
	for (int y = -(height/2.0f); y<(height/2.0f);y++){
		topHorizon = clamp(topHorizon, -1.0f, 1.0f);
		bottomHorizon = clamp(bottomHorizon, -1.0f, 1.0f);
		leftHorizon = clamp(leftHorizon, -1.0f, 1.0f);
		rightHorizon = clamp(rightHorizon, -1.0f, 1.0f);
		float relY = ((float) y)/(height/2.0f);
		float relX = ((float) x)/(width/2.0f);
		// Let's find the "relative" radius, i.e. if we drew a line from
		// (0,0) to our current point, the continued drawing the line until
		// you hit the edge of the viewing panel (rectangle), what percentage 
		// would our point be at along that line?

		// Let's figure out how we're going to "stretch" it
		float yWeighted = (topHorizon + bottomHorizon)/2.0f - relY*(abs(topHorizon-bottomHorizon)/2.0f); 
		float xWeighted = (leftHorizon + rightHorizon)/2.0f + relX*(abs(leftHorizon-rightHorizon)/2.0f); 
		float relativeRad;
		
		if(abs(relY)>abs(relX)){
		relativeRad = abs(relY);
		}else{
		relativeRad = abs(relX);
		}
		// Let's find x and y coords on a 0 to 1 scale, so IGNORING the radius
		//float xNew = (((float) relativeRad)*xWeighted)/(sqrt((relY*relY)+(relX*relX)));
		//float yNew = (((float) relativeRad)*relY)/(sqrt((relX*relX + relY*relY)));

		float yNew = (abs(yWeighted)/yWeighted)*sqrt(abs((yWeighted*yWeighted-yWeighted*yWeighted*xWeighted*xWeighted)/(1-xWeighted*xWeighted*yWeighted*yWeighted)));
		float xNew = xWeighted*(sqrt(abs(1-yNew*yNew)));


		// Now multiply by radius
		xNew *= rad;
		yNew *= rad;

		// Now go to all positive coordinates (what we really use in OpenCV)
		float yPix = (height/2.0f)-yNew;
		float xPix = xNew+(width/2.0f); 
		//yPix = clamp(yPix, 0.0f, height/2.0f-1);
		//xPix = clamp(xPix, 0.0f, width/2.0f-1);
		int xPos = (int)((width/2.0f)+x);
		int yPos = (int)((height/2.0f)+y);
		//cout << xWeighted << " " << yWeighted << "\n";
		if(yPix == yPix && xPix == xPix && xPix >0 && yPix >0 && xPix <= width && yPix <= height) {
			//toreturn.at<Vec3b>(yPos, xPos) = src.at<Vec3b>(yPix, xPix);
			toreturn.at<Vec3b>(yPix, xPix).val[1] = 0;
		}
// TODO make YMax, YMin the appropriate values, then use percentage along those points

		}
	}
    	return toreturn;
}

void streamWebcam(){
	cvNamedWindow("Camera_Output", 1);    //Create window
	CvCapture* capture = cvCaptureFromCAM(CV_CAP_ANY);  //Capture using any camera connected to your system
	while (1){ //Create infinte loop for live streaming
		IplImage* frame = cvQueryFrame(capture); //Create image frames from capture
		cvShowImage("Camera_Output", frame);   //Show image frames on created window

		char key = cvWaitKey(10);     //Capture Keyboard stroke
		if (key == 27){
			break;      //If you hit ESC key loop will break.
		}
	}
	cvReleaseCapture(&capture); //Release capture.
	cvDestroyWindow("Camera_Output"); //Destroy Window
}

void streamFisheyeConversion(){
	cvNamedWindow("Camera_Output", 1);    //Create window
	CvCapture* capture = cvCaptureFromCAM(CV_CAP_ANY);  //Capture using any camera connected to your system
	float bottomHorizon, topHorizon, leftHorizon, rightHorizon;
	bottomHorizon = -.6f;
	topHorizon = .6f;
	leftHorizon = -.6f;
	rightHorizon = .6f;

	while (1){ //Create infinte loop for live streaming
		IplImage* frame = cvQueryFrame(capture); //Create image frames from capture
		Mat toPass(frame);
		//Mat edited = editimage(toPass);
		//drawequators(toPass, .5, .75);
		Size s = toPass.size();
		if(s.width >0 && s.height > 0){
		    imshow("Camera_Output", sectionWarp(toPass, bottomHorizon, topHorizon, leftHorizon, rightHorizon));   //Show image frames on created window
		}
		
		char key = cvWaitKey(10);     //Capture Keyboard stroke

		if (key == ESCAPE_KEY){
			break;
		}else if (key == 'r') {
			leftHorizon = LEFT_HORIZON_INITIAL;
			rightHorizon = RIGHT_HORIZON_INITIAL;
			topHorizon = TOP_HORIZON_INITIAL;
			bottomHorizon = BOTTOM_HORIZON_INITIAL;
		}else if (key == LEFT_ARROW || key == RIGHT_ARROW || key == UP_ARROW || key == DOWN_ARROW){
			// Weight how far they move based upon how far they are from the center of the screen
			// i.e. when you're farther away, you only move a little...
			float amtToMoveL = 1.0f - abs(leftHorizon);
			float amtToMoveR = 1.0f - abs(rightHorizon);
			float amtToMoveU = 1.0f - abs(topHorizon);
			float amtToMoveD = 1.0f - abs(bottomHorizon);

			switch  (key) {
				case LEFT_ARROW: leftHorizon -= amtToMoveL/8.0f;
						 rightHorizon -= amtToMoveR/8.0f;
						 break; // TODO FINISH THIS
				case RIGHT_ARROW: leftHorizon += amtToMoveL/8.0f;
						  rightHorizon += amtToMoveR/8.0f;
						  break;
				case UP_ARROW: topHorizon += amtToMoveU/8.0f;
					       bottomHorizon += amtToMoveD/8.0f;
					       break;
				case DOWN_ARROW: topHorizon -= amtToMoveU/8.0f;
						 bottomHorizon -= amtToMoveD/8.0f;
						 break;
			}
			//bottomHorizon += amtToMoveD/8.0f;
			//topHorizon += amtToMoveU/8.0f;
			//leftHorizon += amtToMoveL/8.0f;
			//rightHorizon += amtToMoveR/8.0f;
			cout << "MOVING\n";
		}else {
			cout << (int)key << '\n';
		}
	}
	cvReleaseCapture(&capture); //Release capture.
	cvDestroyWindow("Camera_Output"); //Destroy Window
}

int main(int argc, char** argv)
{
	//showimg(editimage(getcap()));
	streamFisheyeConversion();
	//streamWebcam();
	return 0;
}
