#include<opencv\highgui.h>
#include <opencv2/opencv.hpp>
#include "opencv2/imgproc.hpp"
#include<opencv\cv.h>
#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include "opencv2/imgproc.hpp"
#include <vector>
#include<fstream>
#include <clocale>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>

#define ANGLE 30
#define RSIZE 2
#define Black 0
#define MashtabOgrÑircle 70 
#define Uchar unsigned char

using namespace std;
using namespace cv;

Mat rotate(Mat src) {
	Point2f center(src.cols / 2.0, src.rows / 2.0);
	Mat rot = getRotationMatrix2D(center, ANGLE, 1);
	Mat dst;
	warpAffine(src, dst, rot, dst.size());
	return dst;
}

Mat del(Mat image)
{
	int counter = 0;
	for (int i = image.cols - 1; i > 0; i--)
	{
		if (image.at<Uchar>(0, i) == Black)
			counter++;
		else
			break;
	}
	counter = image.cols - counter - 1;
	image = image(Rect(0, 0, counter, image.rows));
	cout << image.rows << "  " << image.cols << endl;
	return image;
}
int main()
{
	setlocale(LC_CTYPE, "rus");
	Mat source = imread("main_s111.bmp", CV_LOAD_IMAGE_GRAYSCALE);
	Mat cloneSource,res;
	cloneSource = source.clone();
	resize(cloneSource, cloneSource, Size(), RSIZE, RSIZE, INTER_LINEAR);
	cloneSource = rotate(cloneSource);
	imwrite("res1.bmp",cloneSource);

	logPolar(source, source, Point(source.rows / 2, source.cols / 2), MashtabOgrÑircle, INTER_LINEAR | WARP_FILL_OUTLIERS);//Point(source.rows / 2, source.cols / 2) - ýòî öåíòð ïðåîáðàçîâàíèÿ.
	logPolar(cloneSource, cloneSource, Point(cloneSource.rows / 2, cloneSource.cols / 2), MashtabOgrÑircle, INTER_LINEAR | WARP_FILL_OUTLIERS);
	imwrite("res2.bmp", cloneSource);
	cout << cloneSource.rows << "  " << cloneSource.cols << endl;

	source = del(source);
	cloneSource = del(cloneSource);
	imwrite("res3.bmp", cloneSource);

	Mat temp (360, (100 * log(source.rows / 2)), 0);
	resize(source, source, temp.size(),360, (100 * log(source.rows / 2)),CV_INTER_LINEAR);
	Mat temp2(360, (100 * log(cloneSource.rows / 2)), 0);
	resize(cloneSource, cloneSource, temp2.size(), 360, (100 * log(cloneSource.rows / 2)), CV_INTER_LINEAR);
	imwrite("res4.bmp", source);
	imwrite("res5.bmp", cloneSource);


	Mat temp3(cloneSource.rows * 2, cloneSource.cols, 0);
	cloneSource.copyTo(temp3(Rect(0,0,cloneSource.cols,cloneSource.rows)));
	cloneSource.copyTo(temp3(Rect(0, cloneSource.rows, cloneSource.cols, cloneSource.rows)));
	imwrite("res6.bmp", temp3);



	int result_cols = temp3.cols;
	int result_rows = temp3.rows;
	res.create(result_rows, result_cols, 0);
	matchTemplate(temp3, source, res, CV_TM_CCORR_NORMED);
	double minVal; double maxVal; Point minLoc; Point maxLoc;
	minMaxLoc(res, &minVal, &maxVal, &minLoc, &maxLoc, Mat());
	cout << "Óãîë ïîâîðîòà êàðòèíêè: " << 360 - maxLoc.y << endl;
	cout << "Ðàçìåð ìàñøòàáèðîâàíèÿ êàðòèíêè: " << round(exp((float)maxLoc.x / 100))<<endl;
	cout << "source : " << source.cols <<" "<< source.rows << endl;
	cout << "sourceClone : " << cloneSource.cols << " " << cloneSource.rows << endl;
	return 0;
}



