// opencv_test.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include "opencv2/highgui.hpp"
#include <iostream>

using namespace cv;


int main() {

	Mat input = imread("lena.jpg", CV_LOAD_IMAGE_COLOR);
	Mat input_gray, output;

	// check for validation
	if (!input.data) {
		printf("Could not open\n");
		return -1;
	}

	cvtColor(input, input_gray, CV_RGB2GRAY);	// convert RGB to Grayscale
//	input_gray.convertTo(input_gray, CV_64F, 1.0 / 255);	// 8-bit unsigned char -> 64-bit floating point
	
	//Fill the code using 'Canny' in OpenCV.
	Canny(input_gray, output, 50, 100);


	namedWindow("Grayscale", WINDOW_AUTOSIZE);
	imshow("Grayscale", input_gray);

	namedWindow("Canny", WINDOW_AUTOSIZE);
	imshow("Canny", output);

	waitKey(0);

	return 0;
}



