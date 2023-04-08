/*
Need to check mirroring & adjust kernel
*/

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

Mat naive_gaussianfilter(const Mat input, int n, float sigmaT, float sigmaS, const char* opt);
Mat gaussianfilter_sep(const Mat input, int n, float sigmaT, float sigmaS, const char* opt);

int main() {

	Mat input = imread("lena.jpg", CV_LOAD_IMAGE_COLOR);
	Mat input_gray;
	Mat naive_output, output;

	cvtColor(input, input_gray, CV_RGB2GRAY);

	if (!input.data)
	{
		std::cout << "Could not open" << std::endl;
		return -1;
	}

	namedWindow("Grayscale", WINDOW_AUTOSIZE);
	imshow("Grayscale", input_gray);

	// using naive gaussian filter
	clock_t start = clock();
	naive_output = naive_gaussianfilter(input_gray, 10, 2, 2, "zero-paddle"); //Boundary process: zero-paddle, mirroring, adjustkernel
	clock_t end = clock();
	float res1 = (end - start) / CLOCKS_PER_SEC;
	cout << "Time consumption of naive gaussian filter: " << res1 << "sec" << endl;

	// using gaussian filter implemented in seperable manner
	start = clock();
	output = gaussianfilter_sep(input_gray, 10, 2, 2, "zero-paddle"); //Boundary process: zero-paddle, mirroring, adjustkernel
	end = clock();
	float res2 = (end - start) / CLOCKS_PER_SEC;
	cout << "Time consumption of seperable gaussian filter: " << res2 << "sec" << endl;
	cout << "Seperable gaussian filter is " << res1 / res2 << "x faster." << endl;


	namedWindow("Gaussian Filter", WINDOW_AUTOSIZE);
	imshow("Gaussian Filter", output);
	
	waitKey(0);


	return 0;
}


Mat naive_gaussianfilter(const Mat input, int n, float sigmaT, float sigmaS, const char* opt) {

	Mat kernel;

	int row = input.rows;
	int col = input.cols;
	int kernel_size = (2 * n + 1);
	int tempa;
	int tempb;
	float denom;
	float kernelvalue;

	// Initialiazing Kernel Matrix 
	kernel = Mat::zeros(kernel_size, kernel_size, CV_32F);


	denom = 0.0;
	for (int a = -n; a <= n; a++) {  // Denominator in m(s,t)
		for (int b = -n; b <= n; b++) {
			float value1 = exp(-(pow(a, 2) / (2 * pow(sigmaS, 2))) - (pow(b, 2) / (2 * pow(sigmaT, 2))));
			kernel.at<float>(a + n, b + n) = value1;
			denom += value1;
		}
	}

	for (int a = -n; a <= n; a++) {  // Denominator in m(s,t)
		for (int b = -n; b <= n; b++) {
			kernel.at<float>(a + n, b + n) /= denom;
		}
	}

	Mat output = Mat::zeros(row, col, input.type());


	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			if (!strcmp(opt, "zero-paddle")) {
				float sum1 = 0.0;
				for (int a = -n; a <= n; a++) {
					for (int b = -n; b <= n; b++) {
						/* Gaussian filter with Zero-paddle boundary process:
						Fill the code:
						*/
						if ((i + a <= row - 1) && (i + a >= 0) && (j + b <= col - 1) && (j + b >= 0)) {
							sum1 += kernel.at<float>(a + n, b + n) * (float)(input.at<G>(i + a, j + b));
						}
					}
				}
				output.at<G>(i, j) = (G)sum1;
			}

			else if (!strcmp(opt, "mirroring")) {
				float sum1 = 0.0;
				for (int a = -n; a <= n; a++) {
					for (int b = -n; b <= n; b++) {
						/* Gaussian filter with "mirroring" process:
						Fill the code:
						*/
						if (i + a > row - 1) {
							tempa = (i - a);
						}
						else if (i + a < 0) {
							tempa = -(i + a);
						}
						else {
							tempa = (i + a);
						}
						if (j + b > col - 1) {
							tempb = (j - b);
						}
						else if (j + b < 0) {
							tempb = -(j + b);
						}
						else {
							tempb = (j + b);
						}
						sum1 += kernel.at<float>(a + n, b + n) * (float)(input.at<G>(tempa, tempb));
					}
				}
				output.at<G>(i, j) = (G)sum1;
			}


			else if (!strcmp(opt, "adjustkernel")) {
				float sum1 = 0.0;
				float sum2 = 0.0;
				for (int a = -n; a <= n; a++) {
					for (int b = -n; b <= n; b++) {
						/* Gaussian filter with "adjustkernel" process:
						Fill the code:
						*/
						if ((i + a <= row - 1) && (i + a >= 0) && (j + b <= col - 1) && (j + b >= 0)) {
							sum1 += kernel.at<float>(a + n, b + n) * (float)(input.at<G>(i + a, j + b));
							sum2 += kernel.at<float>(a + n, b + n);
						}
					}
				}
				output.at<G>(i, j) = (G)(sum1 / sum2);
			}
		}
	}
	return output;
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

