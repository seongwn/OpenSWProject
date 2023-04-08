// opencv_test.cpp : Defines the entry point for the console application.
//

#include <opencv2/opencv.hpp>
#include "opencv2/highgui.hpp"
#include <iostream>
#include <string>

using namespace cv;
using namespace std;

int main() {
	Mat src = imread("building.jpg", CV_LOAD_IMAGE_COLOR);
	Mat dst, color_dst;

	// check for validation
	if (!src.data) {
		printf("Could not open\n");
		return -1;
	}

	Canny(src, dst, 50, 200, 3);
	cvtColor(dst, color_dst, COLOR_GRAY2BGR);

	//Standard Hough transform (using 'HoughLines')
#if 1
	vector<Vec2f> lines;
	cv::HoughLines(dst, lines, 1, CV_PI / 180, 150, 0, 0);

	for (size_t i = 0; i < lines.size(); i++)
	{
		float rho = lines[i][0];
		float theta = lines[i][1];
		double a = cos(theta), b = sin(theta);
		double x0 = a*rho, y0 = b*rho;
		Point pt1(cvRound(x0 + 1000 * (-b)), cvRound(y0 + 1000 * (a)));
		Point pt2(cvRound(x0 - 1000 * (-b)), cvRound(y0 - 1000 * (a)));
		line(color_dst, pt1, pt2, Scalar(0, 0, 255), 3, 8);
	}

	//Probabilistic Hough transform (using 'HoughLinesP')
#else
	vector<Vec4i> lines;
	cv::HoughLinesP(dst, lines, 1, CV_PI / 180, 50, 50, 10);

	for (size_t i = 0; i < lines.size(); i++)
	{
		line(color_dst, Point(lines[i][0], lines[i][1]),
			Point(lines[i][2], lines[i][3]), Scalar(0, 0, 255), 3, 8);
	}

#endif 1
	namedWindow("Source", 1);
	imshow("Source", src);
	namedWindow("Detected Lines", 1);
	imshow("Detected Lines", color_dst);
	waitKey(0);

	return 0;
}

