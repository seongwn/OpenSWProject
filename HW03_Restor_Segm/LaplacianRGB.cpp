#include <iostream>
#include <opencv2/opencv.hpp>
#include <math.h>       /* exp */
#define IM_TYPE	CV_8UC3

using namespace std;
using namespace cv;

// Image Type
// "G" for GrayScale Image, "C" for Color Image
#if (IM_TYPE == CV_8UC3)
typedef uchar G;
typedef cv::Vec3b C;
#elif (IM_TYPE == CV_16SC3)
typedef short G;
typedef Vec3s C;
#elif (IM_TYPE == CV_32SC3)
typedef int G;
typedef Vec3i C;
#elif (IM_TYPE == CV_32FC3)
typedef float G;
typedef Vec3f C;
#elif (IM_TYPE == CV_64FC3)
typedef double G;
typedef Vec3d C;
#endif

Mat sobelfilter(const Mat input);

int main() {

	Mat input = imread("lena.jpg", CV_LOAD_IMAGE_COLOR);
	Mat output;

	if (!input.data)
	{
		std::cout << "Could not open" << std::endl;
		return -1;
	}

	namedWindow("Original", WINDOW_AUTOSIZE);
	imshow("Original", input);
	output = sobelfilter(input); //Boundary process: zero-paddle, mirroring, adjustkernel

	namedWindow("LaplacianRGB", WINDOW_AUTOSIZE);
	imshow("LaplacianRGB", output);


	waitKey(0);

	return 0;
}


Mat sobelfilter(const Mat input) {

	Mat kernel;

	int row = input.rows;
	int col = input.cols;
	int n = 1; // Sobel Filter Kernel N

	int kernel_size = n * 2 + 1;
	float S[9] = { 0, 1, 0, 1, -4, 1, 0, 1, 0 };
	//float S[9] = { 1, 1, 1, 1, -8, 1, 1, 1, 1 };


	Mat kernelS(kernel_size, kernel_size, CV_32F, S);

	Mat output = Mat::zeros(row, col, input.type());
	
	int tempa = 0;
	int tempb = 0;
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			float sum1_r = 0.0, sum1_g = 0.0, sum1_b = 0.0;
			for (int a = -n; a <= n; a++) {
				for (int b = -n; b <= n; b++) {
					// Use mirroring boundary process
					// Find output M(x,y) = sqrt( input.at<G>(x, y)*Sx + input.at<G>(x, y)*Sy )
					if (i + a < 0) {
						tempa = -(i + a);
					}
					else if (i + a > row - 1) {
						tempa = i - a;

					}
					else {
						tempa = i + a;
					}
					if (j + b < 0) {
						tempb = -(j + b);
					}
					else if (j + b > col - 1) {
						tempb = j - b;
					}
					else {
						tempb = j + b;
					}
					sum1_r += float(input.at<C>(tempa, tempb)[0]) * kernelS.at<float>(a + n, b + n);
					sum1_g += float(input.at<C>(tempa, tempb)[1]) * kernelS.at<float>(a + n, b + n);
					sum1_b += float(input.at<C>(tempa, tempb)[2]) * kernelS.at<float>(a + n, b + n);

				}
			}
			output.at<C>(i, j)[0] = (G)(abs(sum1_r));
			output.at<C>(i, j)[1] = (G)(abs(sum1_g));
			output.at<C>(i, j)[2] = (G)(abs(sum1_b));
			
		}
	}
	return output;
}