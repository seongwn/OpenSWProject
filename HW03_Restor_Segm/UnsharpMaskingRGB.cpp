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
Mat unsharp_mask(const Mat input, int n, float sigmaT, float sigmaS, const char* opt, float k);

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

	
	output = unsharp_mask(input, 10, 2, 2, "mirroring", 0.1); //Boundary process: zero-paddle, mirroring, adjustkernel
													

	namedWindow("Unsharp Masking", WINDOW_AUTOSIZE);
	imshow("Unsharp Masking", output);

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
				float sum1_r = 0.0;
				float sum1_g = 0.0;
				float sum1_b = 0.0;
				for (int a = -n; a <= n; a++) {
					for (int b = -n; b <= n; b++) {
						/* Gaussian filter with Zero-paddle boundary process:
						Fill the code:
						*/
						if ((i + a <= row - 1) && (i + a >= 0) && (j + b <= col - 1) && (j + b >= 0)) {
							sum1_r += kernel.at<float>(a + n, b + n) * (float)(input.at<C>(i + a, j + b))[0];
							sum1_g += kernel.at<float>(a + n, b + n) * (float)(input.at<C>(i + a, j + b))[1];
							sum1_b += kernel.at<float>(a + n, b + n) * (float)(input.at<C>(i + a, j + b))[2];
						}
					}
				}
				output.at<C>(i, j)[0] = (G)sum1_r;
				output.at<C>(i, j)[1] = (G)sum1_g;
				output.at<C>(i, j)[2] = (G)sum1_b;
			}

			else if (!strcmp(opt, "mirroring")) {
				float sum1_r = 0.0;
				float sum1_g = 0.0;
				float sum1_b = 0.0;
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
						sum1_r += kernel.at<float>(a + n, b + n) * (float)(input.at<C>(tempa, tempb))[0];
						sum1_g += kernel.at<float>(a + n, b + n) * (float)(input.at<C>(tempa, tempb))[1];
						sum1_b += kernel.at<float>(a + n, b + n) * (float)(input.at<C>(tempa, tempb))[2];
					}
				}
				output.at<C>(i, j)[0] = (G)sum1_r;
				output.at<C>(i, j)[1] = (G)sum1_g;
				output.at<C>(i, j)[2] = (G)sum1_b;
			}


			else if (!strcmp(opt, "adjustkernel")) {
				float sum1_r = 0.0;
				float sum1_g = 0.0;
				float sum1_b = 0.0;
				float sum2 = 0.0;
				for (int a = -n; a <= n; a++) {
					for (int b = -n; b <= n; b++) {
						/* Gaussian filter with "adjustkernel" process:
						Fill the code:
						*/
						if ((i + a <= row - 1) && (i + a >= 0) && (j + b <= col - 1) && (j + b >= 0)) {
							sum1_r += kernel.at<float>(a + n, b + n) * (float)(input.at<C>(i + a, j + b))[0];
							sum1_g += kernel.at<float>(a + n, b + n) * (float)(input.at<C>(i + a, j + b))[1];
							sum1_b += kernel.at<float>(a + n, b + n) * (float)(input.at<C>(i + a, j + b))[2];
							sum2 += kernel.at<float>(a + n, b + n);
						}
					}
				}
				output.at<C>(i, j)[0] = (G)(sum1_r / sum2);
				output.at<C>(i, j)[1] = (G)(sum1_g / sum2);
				output.at<C>(i, j)[2] = (G)(sum1_b / sum2);
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
				float sum1_r = 0.0;
				float sum1_g = 0.0;
				float sum1_b = 0.0;
				for (int b = -n; b < n; b++) {
					if (j + b <= col - 1 && j + b >= 0) {
						sum1_r += (float)input.at<C>(i, j + b)[0] * kernelT.at<float>(0, b + n);
						sum1_g += (float)input.at<C>(i, j + b)[1] * kernelT.at<float>(0, b + n);
						sum1_b += (float)input.at<C>(i, j + b)[2] * kernelT.at<float>(0, b + n);
					}
				}
				temp_output.at<C>(i, j)[0] = (G)sum1_r;
				temp_output.at<C>(i, j)[1] = (G)sum1_g;
				temp_output.at<C>(i, j)[2] = (G)sum1_b;
			}
		}
		for (int i = 0; i < row; i++) {
			for (int j = 0; j < col; j++) {
				float sum1_r = 0.0;
				float sum1_g = 0.0;
				float sum1_b = 0.0;
				for (int a = -n; a <= n; a++) {
					if (i + a <= row - 1 && i + a >= 0) {
						sum1_r += (float)temp_output.at<C>(i + a, j)[0] * kernelS.at<float>(a + n, 0);
						sum1_g += (float)temp_output.at<C>(i + a, j)[1] * kernelS.at<float>(a + n, 0);
						sum1_b += (float)temp_output.at<C>(i + a, j)[2] * kernelS.at<float>(a + n, 0);
					}
				}
				output.at<C>(i, j)[0] = (G)sum1_r;
				output.at<C>(i, j)[1] = (G)sum1_g;
				output.at<C>(i, j)[2] = (G)sum1_b;
			}
		}
	}
	else if (!strcmp(opt, "mirroring")) {
		for (int i = 0; i < row; i++) {
			for (int j = 0; j < col; j++) {
				float sum1_r = 0.0;
				float sum1_g = 0.0;
				float sum1_b = 0.0;
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
					sum1_r += kernelT.at<float>(0, b + n) * (float)input.at<C>(i, tempb)[0];
					sum1_g += kernelT.at<float>(0, b + n) * (float)input.at<C>(i, tempb)[1];
					sum1_b += kernelT.at<float>(0, b + n) * (float)input.at<C>(i, tempb)[2];
				}
				temp_output.at<C>(i, j)[0] = (G)sum1_r;
				temp_output.at<C>(i, j)[1] = (G)sum1_g;
				temp_output.at<C>(i, j)[2] = (G)sum1_b;
			}
		}
		for (int i = 0; i < row; i++) {
			for (int j = 0; j < col; j++) {
				float sum1_r = 0.0;
				float sum1_g = 0.0;
				float sum1_b = 0.0;
				for (int a = -n; a <= n; a++) {
					if (i + a < 0) {
						tempa = -(i + a);
					}
					else if (i + a > row - 1) {
						tempa = i - a;
					}
					else {
						tempa = i + a;
					}
					sum1_r += kernelS.at<float>(a + n, 0) * (float)temp_output.at<C>(tempa, j)[0];
					sum1_g += kernelS.at<float>(a + n, 0) * (float)temp_output.at<C>(tempa, j)[1];
					sum1_b += kernelS.at<float>(a + n, 0) * (float)temp_output.at<C>(tempa, j)[2];
				}
				output.at<C>(i, j)[0] = (G)sum1_r;
				output.at<C>(i, j)[1] = (G)sum1_g;
				output.at<C>(i, j)[2] = (G)sum1_b;
			}
		}
	}

	else if (!strcmp(opt, "adjustkernel")) {
		for (int i = 0; i < row; i++) {
			for (int j = 0; j < col; j++) {
				float sum1_r = 0.0;
				float sum1_g = 0.0;
				float sum1_b = 0.0;
				float sum2 = 0.0;
				for (int b = -n; b <= n; b++) {
					if (j + b >= 0 && j + b <= col - 1) {
						sum1_r += (float)input.at<C>(i, j + b)[0] * kernelT.at<float>(0, b + n);
						sum1_g += (float)input.at<C>(i, j + b)[1] * kernelT.at<float>(0, b + n);
						sum1_b += (float)input.at<C>(i, j + b)[2] * kernelT.at<float>(0, b + n);
						sum2 += kernelT.at<float>(0, b + n);
					}
				}
				temp_output.at<C>(i, j)[0] = (G)(sum1_r / sum2);
				temp_output.at<C>(i, j)[1] = (G)(sum1_g / sum2);
				temp_output.at<C>(i, j)[2] = (G)(sum1_b / sum2);
			}
		}
		for (int i = 0; i < row; i++) {
			for (int j = 0; j < col; j++) {
				float sum1_r = 0.0;
				float sum1_g = 0.0;
				float sum1_b = 0.0;
				float sum2 = 0.0;
				for (int a = -n; a <= n; a++) {
					if (i + a >= 0 && i + a <= row - 1) {
						sum1_r += (float)temp_output.at<C>(i + a, j)[0] * kernelS.at<float>(a + n, 0);
						sum1_g += (float)temp_output.at<C>(i + a, j)[1] * kernelS.at<float>(a + n, 0);
						sum1_b += (float)temp_output.at<C>(i + a, j)[2] * kernelS.at<float>(a + n, 0);
						sum2 += kernelS.at<float>(a + n, 0);
					}
				}
				output.at<C>(i, j)[0] = (G)(sum1_r / sum2);
				output.at<C>(i, j)[1] = (G)(sum1_g / sum2);
				output.at<C>(i, j)[2] = (G)(sum1_b / sum2);
			}
		}
	}


	return output;
}

Mat unsharp_mask(const Mat input, int n, float sigmaT, float sigmaS, const char* opt, float k) {
	int row = input.rows;
	int col = input.cols;
	Mat output = Mat::zeros(row, col, input.type());
	// Blur with low-pass filter(input -> L)
	Mat L = gaussianfilter_sep(input, n, sigmaT, sigmaS, opt);
	// output = (input - k * L) / (1 - k)
	output = (input - k * L ) / (1 - k);
	
	return output;
}