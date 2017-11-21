#include <opencv\highgui.h>
#include <opencv2/opencv.hpp>
#include "opencv2/imgproc.hpp"
#include<opencv\cv.h>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <fstream>
#include <vector>
#include <queue>
#include <string>
#define _USE_MATH_DEFINES

using namespace std;
using namespace cv;
void worck1(const string s, IplImage *im)
{
	IplImage* gray = 0;
	IplImage* dst = 0;
	cvNamedWindow("gray", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Canny1", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Canny2", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Canny3", CV_WINDOW_AUTOSIZE);
	//create one chenal image and transformation
	gray = cvCreateImage(cvGetSize(im), IPL_DEPTH_8U, 1);
	dst = cvCreateImage(cvGetSize(im), IPL_DEPTH_8U, 1);
	cvCvtColor(im, gray, CV_RGB2GRAY);
	cvShowImage("original", im);
	cvShowImage("gray", gray);
	cvCanny(gray, dst, 60, 140, 3);
	cvShowImage("Canny1", dst);
	cvSaveImage("name.bmp", dst);
	cvCanny(gray, dst, 60, 140, 3);
	cvShowImage("Canny2", dst);
	cvSaveImage("name3.bmp", dst);
	cvCanny(gray, dst, 60, 140, 3);
	cvShowImage("Canny3", dst);
	cvSaveImage("name3.bmp", dst);

	cvReleaseImage(&gray);
	cvReleaseImage(&dst);
	
}

void worck2(const string s, IplImage *im)
{
	IplImage* gray = 0;
	IplImage *smooth = 0;
	IplImage* dst = 0;
	cvNamedWindow("gray", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Canny1", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Canny2", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Canny3", CV_WINDOW_AUTOSIZE);
	//create one chenal image and transformation
	gray = cvCreateImage(cvGetSize(im), IPL_DEPTH_8U, 1);
	cvCvtColor(im, gray, CV_RGB2GRAY);
	cvShowImage("original", im);
	cvShowImage("gray", gray);
	smooth = cvCreateImage(cvGetSize(im), IPL_DEPTH_8U, 1);
	dst = cvCreateImage(cvGetSize(im), IPL_DEPTH_8U, 1);
	cvSmooth(gray, smooth, CV_GAUSSIAN, 3, 3);
	cvCanny(gray, dst, 60, 140, 3);
	cvShowImage("Canny1", dst);
	cvSaveImage("name.bmp", dst);
	cvSmooth(gray, smooth, CV_GAUSSIAN, 5, 5);
	cvCanny(gray, dst, 60, 140, 3);
	cvShowImage("Canny2", dst);
	cvSaveImage("name3.bmp", dst);
	cvSmooth(gray, smooth, CV_GAUSSIAN, 7, 7);
	cvCanny(gray, dst, 60, 140, 3);
	cvShowImage("Canny3", dst);
	cvSaveImage("name3.bmp", dst);
	//dst = cvCreateImage(cvGetSize(im), IPL_DEPTH_8U, 1);
	
	cvReleaseImage(&gray);
	cvReleaseImage(&dst);
	
}


int main()
{
	 IplImage* image = 0;
	//image = cvLoadImage("cat.bmp", 1);
	//image = cvLoadImage("lena.bmp", 1);
	  image = cvLoadImage("m.jpg", 1);
	  worck("s", image);

	return 0;
}
