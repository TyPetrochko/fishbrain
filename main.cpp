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

using namespace std;
using namespace cv;

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
	while (1){ //Create infinte loop for live streaming
		IplImage* frame = cvQueryFrame(capture); //Create image frames from capture
		Mat toPass(frame);
		imshow("Camera_Output", editimage(toPass));   //Show image frames on created window

		char key = cvWaitKey(10);     //Capture Keyboard stroke
		if (key == 27){
			break;      //If you hit ESC key loop will break.
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





