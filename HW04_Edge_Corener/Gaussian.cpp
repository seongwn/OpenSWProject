#include <opencv2/opencv.hpp>
#include <stdio.h>
# include <math.h>

#define IM_TYPE	CV_64FC3

using namespace std;
using namespace cv;

Mat Add_Gaussian_noise(const Mat input, double mean, double sigma);
Mat Gaussianfilter_Gray(const Mat input, int n, double sigma_t, double sigma_s, const char *opt);
Mat Gaussianfilter_RGB(const Mat input, int n, double sigma_t, double sigma_s, const char *opt);

int main() {

	Mat input = imread("lena.jpg", CV_LOAD_IMAGE_COLOR);
	Mat input_gray;

	// check for validation
	if (!input.data) {
		printf("Could not open\n");
		return -1;
	}

	cvtColor(input, input_gray, CV_RGB2GRAY);	// convert RGB to Grayscale
	
	// 8-bit unsigned char -> 64-bit floating point
	input.convertTo(input, CV_64FC3, 1.0 / 255);
	input_gray.convertTo(input_gray, CV_64F, 1.0 / 255);

	// Add noise to original image
	Mat noise_Gray = Add_Gaussian_noise(input_gray, 0, 0.1);
	Mat noise_RGB = Add_Gaussian_noise(input, 0, 0.1);

	// Denoise, using gaussian filter
	Mat Denoised_Gray = Gaussianfilter_Gray(noise_Gray, 3, 10, 10, "adjustkernel");
	Mat Denoised_RGB = Gaussianfilter_RGB(noise_RGB, 3, 10, 10, "adjustkernel");

	namedWindow("Grayscale", WINDOW_AUTOSIZE);
	imshow("Grayscale", input_gray);

	namedWindow("RGB", WINDOW_AUTOSIZE);
	imshow("RGB", input);

	namedWindow("Gaussian Noise (Grayscale)", WINDOW_AUTOSIZE);
	imshow("Gaussian Noise (Grayscale)", noise_Gray);

	namedWindow("Gaussian Noise (RGB)", WINDOW_AUTOSIZE);
	imshow("Gaussian Noise (RGB)", noise_RGB);

	namedWindow("Denoised (Grayscale)", WINDOW_AUTOSIZE);
	imshow("Denoised (Grayscale)", Denoised_Gray);

	namedWindow("Denoised (RGB)", WINDOW_AUTOSIZE);
	imshow("Denoised (RGB)", Denoised_RGB);

	waitKey(0);

	return 0;
}

Mat Add_Gaussian_noise(const Mat input, double mean, double sigma) {

	Mat NoiseArr = Mat::zeros(input.rows, input.cols, input.type());
	RNG rng;
	rng.fill(NoiseArr, RNG::NORMAL, mean, sigma);

	add(input, NoiseArr, NoiseArr);

	return NoiseArr;
}

Mat Gaussianfilter_Gray(const Mat input, int n, double sigma_t, double sigma_s, const char *opt) {

	int row = input.rows;
	int col = input.cols;
	int kernel_size = (2 * n + 1);

	// Initialiazing Gaussian Kernel Matrix
	Mat kernel = Mat::zeros(kernel_size, kernel_size, CV_64F);
	// Fill code to initialize Gaussian filter kernel matrix
	double denom = 0.0;
	for (int a = -n; a <= n; a++) {  // Denominator in m(s,t)
		for (int b = -n; b <= n; b++) {
			double value1 = exp(-(pow(a, 2) / (2 * pow(sigma_s, 2))) - (pow(b, 2) / (2 * pow(sigma_t, 2))));
			kernel.at<double>(a + n, b + n) = value1;
			denom += value1;
		}
	}

	for (int a = -n; a <= n; a++) {  // Denominator in m(s,t)
		for (int b = -n; b <= n; b++) {
			kernel.at<double>(a + n, b + n) /= denom;
		}
	}
	Mat output = Mat::zeros(row, col, input.type());
	// convolution
	
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			if (!strcmp(opt, "zero-padding")) {
				double sum1 = 0.0;
				for (int x = -n; x <= n; x++) { // for each kernel window
					for (int y = -n; y <= n; y++) {
						// Gaussian filter with "zero-padding" boundary process:		
						if ((i + x <= row - 1) && (i + x >= 0) && (j + y <= col - 1) && (j + y >= 0)) {
							sum1 += kernel.at<double>(x + n, y + n) * (float)input.at<double>(i + x, j + y);
						}

					}
				}
				output.at<double>(i, j) = (double)sum1;

			}

			else if (!strcmp(opt, "mirroring")) {
				double sum1 = 0.0;
				int tempx, tempy;
				for (int x = -n; x <= n; x++) { // for each kernel window
					for (int y = -n; y <= n; y++) {
						// Gaussian filter with "mirroring" boundary process:		
						if (i + x < 0)
							tempx = -(i + x);
						else if (i + x > row - 1)
							tempx = i - x;
						else
							tempx = i + x;
						if (j + y < 0)
							tempy = -(j + y);
						else if (j + y > col - 1)
							tempy = j - y;
						else
							tempy = j + y;
						sum1 += kernel.at<double>(x + n, y + n) * (double)input.at<double>(tempx, tempy);
					}
				}
				output.at<double>(i, j) = (double)sum1;

			}

			else if (!strcmp(opt, "adjustkernel")) {
				double sum1 = 0.0, sum2 = 0.0;
				for (int x = -n; x <= n; x++) { // for each kernel window
					for (int y = -n; y <= n; y++) {
						// Gaussian filter with "adjustkernel" boundary process:		
						if ((i + x <= row - 1) && (i + x >= 0) && (j + y <= col - 1) && (j + y >= 0)) {
							sum1 += kernel.at<double>(x + n, y + n) * (double)input.at<double>(i + x, j + y);
							sum2 += kernel.at<double>(x + n, y + n);
						}
					}
				}
				output.at<double>(i, j) = (double)sum1 / sum2;
			}

		}
	}
	return output;
}

Mat Gaussianfilter_RGB(const Mat input, int n, double sigma_t, double sigma_s, const char *opt) {
	
	int row = input.rows;
	int col = input.cols;
	int kernel_size = (2 * n + 1);

	// Initialiazing Gaussian Kernel Matrix
	Mat kernel = Mat::zeros(kernel_size, kernel_size, CV_64F);
	// Fill code to initialize Gaussian filter kernel matrix
	double denom = 0.0;
	for (int a = -n; a <= n; a++) {  // Denominator in m(s,t)
		for (int b = -n; b <= n; b++) {
			double value1 = exp(-(pow(a, 2) / (2 * pow(sigma_s, 2))) - (pow(b, 2) / (2 * pow(sigma_t, 2))));
			kernel.at<double>(a + n, b + n) = value1;
			denom += value1;
		}
	}

	for (int a = -n; a <= n; a++) {  // Denominator in m(s,t)
		for (int b = -n; b <= n; b++) {
			kernel.at<double>(a + n, b + n) /= denom;
		}
	}

	
	Mat output = Mat::zeros(row, col, input.type());

	// convolution
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			if (!strcmp(opt, "zero-padding")) {
				double sum1_r = 0.0, sum1_g = 0.0, sum1_b = 0.0;
				for (int x = -n; x <= n; x++) { // for each kernel window
					for (int y = -n; y <= n; y++) {
						// Gaussian filter with "zero-padding" boundary process (3-channel input):		
						if ((i + x <= row - 1) && (i + x >= 0) && (j + y <= col - 1) && (j + y >= 0)) {
							sum1_r += kernel.at<double>(x + n, y + n) * input.at<Vec3d>(i + x, j + y)[0];
							sum1_g += kernel.at<double>(x + n, y + n) * input.at<Vec3d>(i + x, j + y)[1];
							sum1_b += kernel.at<double>(x + n, y + n) * input.at<Vec3d>(i + x, j + y)[2];
						}
					}
				}
				output.at<Vec3d>(i, j)[0] = (double)sum1_r;
				output.at<Vec3d>(i, j)[1] = (double)sum1_g;
				output.at<Vec3d>(i, j)[2] = (double)sum1_b;
			}

			else if (!strcmp(opt, "mirroring")) {
				double sum1_r = 0.0, sum1_g = 0.0, sum1_b = 0.0;
				int tempx, tempy;
				for (int x = -n; x <= n; x++) { // for each kernel window
					for (int y = -n; y <= n; y++) {
						if (i + x > row - 1) {
							tempx = (i - x);
						}
						else if (i + x < 0) {
							tempx = -(i + x);
						}
						else {
							tempx = (i + x);
						}
						if (j + y > col - 1) {
							tempy = (j - y);
						}
						else if (j + y < 0) {
							tempy = -(j + y);
						}
						else {
							tempy = (j + y);
						}
						sum1_r += kernel.at<double>(x + n, y + n) * (double)(input.at<Vec3d>(tempx, tempy)[0]);
						sum1_g += kernel.at<double>(x + n, y + n) * (double)(input.at<Vec3d>(tempx, tempy)[1]);
						sum1_b += kernel.at<double>(x + n, y + n) * (double)(input.at<Vec3d>(tempx, tempy)[2]);
					}
				}
				output.at<Vec3d>(i, j)[0] = (double)sum1_r;
				output.at<Vec3d>(i, j)[1] = (double)sum1_g;
				output.at<Vec3d>(i, j)[2] = (double)sum1_b;
			}

			else if (!strcmp(opt, "adjustkernel")) {
				double sum1_r = 0.0, sum1_g = 0.0, sum1_b = 0.0;
				double sum2 = 0.0;
				for (int x = -n; x <= n; x++) { // for each kernel window
					for (int y = -n; y <= n; y++) {
						if ((i + x <= row - 1) && (i + x >= 0) && (j + y <= col - 1) && (j + y >= 0)) {
							sum1_r += kernel.at<double>(x + n, y + n) * (float)(input.at<Vec3d>(i + x, j + y)[0]);
							sum1_g += kernel.at<double>(x + n, y + n) * (float)(input.at<Vec3d>(i + x, j + y)[1]);
							sum1_b += kernel.at<double>(x + n, y + n) * (float)(input.at<Vec3d>(i + x, j + y)[2]);
							sum2 += kernel.at<double>(x + n, y + n);
						}
					}
				}
				output.at<Vec3d>(i, j)[0] = (double)(sum1_r / sum2);
				output.at<Vec3d>(i, j)[1] = (double)(sum1_g / sum2);
				output.at<Vec3d>(i, j)[2] = (double)(sum1_b / sum2);
			}

		}
	}

	return output;
}