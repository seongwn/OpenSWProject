#include <iostream>
#include <opencv2/opencv.hpp>
#include <math.h>       /* exp */
# include <time.h> /*  */
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

Mat gaussianfilter_sep(const Mat input, int n, float sigmaT, float sigmaS, const char* opt);
Mat unsharpMask(const Mat input, int n, float sigmaT, float sigmaS, const char*opt, float k);

int main() {

	Mat input = imread("lena.jpg", CV_LOAD_IMAGE_COLOR);
	Mat input_gray;
	Mat output;

	cvtColor(input, input_gray, CV_RGB2GRAY);

	if (!input.data)
	{
		std::cout << "Could not open" << std::endl;
		return -1;
	}

	namedWindow("Grayscale", WINDOW_AUTOSIZE);
	imshow("Grayscale", input_gray);

	
	output = unsharpMask(input_gray, 3, 2, 2, "zero-paddle", 0.3); //Boundary process: zero-paddle, mirroring, adjustkernel
	
	namedWindow("Unsharp Masking", WINDOW_AUTOSIZE);
	imshow("Unsharp Masking", output);

	waitKey(0);


	return 0;
}

Mat gaussianfilter_sep(const Mat input, int n, float sigmaT, float sigmaS, const char* opt) {
	Mat kernelS, kernelT;

	int row = input.rows;
	int col = input.cols;
	int kernel_size = (2 * n + 1);
	int tempa, tempb;
	float denom1 = 0.0;
	float denom2 = 0.0;
	float kernelvalue;

	//Initializing kernel matrix
	kernelS = Mat::zeros(kernel_size, 1, CV_32F);
	kernelT = Mat::zeros(1, kernel_size, CV_32F);
	for (int a = -n; a <= n; a++) {
		float val1 = exp(-pow(a, 2) / (2 * pow(sigmaS, 2)));
		kernelS.at<float>(a + n, 0) = val1;
		denom1 += val1;

		float val2 = exp(-pow(a, 2) / (2 * pow(sigmaT, 2)));
		kernelT.at<float>(0, a + n) = val2;
		denom2 += val2;
	}

	for (int a = -n; a < n; a++) {
		kernelS.at<float>(a + n, 0) /= denom1;
		kernelT.at<float>(0, a + n) /= denom2;
	}

	Mat temp_output = Mat::zeros(row, col, input.type());
	Mat output = Mat::zeros(row, col, input.type());
	if (!strcmp(opt, "zero-paddle")) {
		for (int i = 0; i < row; i++) {
			for (int j = 0; j < col; j++) {
				float sum1 = 0.0;
				for (int b = -n; b < n; b++) {
					if (j + b <= col - 1 && j + b >= 0) {
						sum1 += (float)input.at<G>(i, j + b) * kernelT.at<float>(0, b + n);
					}
				}
				temp_output.at<G>(i, j) = (G)sum1;
			}
		}
		for (int i = 0; i < row; i++) {
			for (int j = 0; j < col; j++) {
				float sum1 = 0.0;
				for (int a = -n; a <= n; a++) {
					if (i + a <= row - 1 && i + a >= 0) {
						sum1 += (float)temp_output.at<G>(i + a, j) * kernelS.at<float>(a + n, 0);
					}
				}
				output.at<G>(i, j) = (G)sum1;
			}
		}
	}
	else if (!strcmp(opt, "mirroring")) {
		for (int i = 0; i < row; i++) {
			for (int j = 0; j < col; j++) {
				float sum1 = 0.0;
				for (int b = -n; b <= n; b++) {
					if (j + b < 0) {
						tempb = -(j + b);
					}
					else if (j + b > col - 1) {
						tempb = j - b;
					}
					else {
						tempb = j + b;
					}
					sum1 += (float)temp_output.at<G>(i, tempb) * kernelT.at<float>(0, b + n);
				}
				temp_output.at<G>(i, j) = (G)sum1;
			}
		}
		for (int i = 0; i < row; i++) {
			for (int j = 0; j < col; j++) {
				float sum1 = 0.0;
				for (int a = -n; a <= n; a++) {
					if (i + a > row - 1) {
						tempa = i - a;
					}
					else if (i + a < 0) {
						tempa = -(i + a);
					}
					else {
						tempa = i + a;
					}
					sum1 += (float)temp_output.at<G>(tempa, j) * kernelS.at<float>(a + n, 0);
				}
				output.at<G>(i, j) = (G)sum1;
			}
		}
	}

	else if (!strcmp(opt, "adjustkernel")) {
		for (int i = 0; i < row; i++) {
			for (int j = 0; j < col; j++) {
				float sum1 = 0.0;
				float sum2 = 0.0;
				for (int b = -n; b <= n; b++) {
					if (j + b >= 0 && j + b <= col - 1) {
						sum1 += (float)input.at<G>(i, j + b) * kernelT.at<float>(0, b + n);
						sum2 += kernelT.at<float>(0, b + n);
					}
				}
				temp_output.at<G>(i, j) = (G)(sum1 / sum2);
			}
		}
		for (int i = 0; i < row; i++) {
			for (int j = 0; j < col; j++) {
				float sum1 = 0.0;
				float sum2 = 0.0;
				for (int a = -n; a <= n; a++) {
					if (i + a >= 0 && i + a <= row - 1) {
						sum1 += (float)temp_output.at<G>(i + a, j) * kernelS.at<float>(a + n, 0);
						sum2 += kernelS.at<float>(a + n, 0);
					}
				}
				output.at<G>(i, j) = (G)(sum1 / sum2);
			}
		}
	}


	return output;
}

Mat unsharpMask(const Mat input, int n, float sigmaT, float sigmaS, const char*opt, float k) {
	int row = input.rows;
	int col = input.cols;
	// Blur with low-pass filter
	Mat L = gaussianfilter_sep(input, n, sigmaT, sigmaS, opt);
	Mat output = Mat::zeros(row, col, input.type());
	
	output = (input - k*L) / (1 - k);
	
	return output;
}