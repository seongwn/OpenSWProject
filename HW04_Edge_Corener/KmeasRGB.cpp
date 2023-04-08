#include <iostream>
#include <opencv2/opencv.hpp>

#define IM_TYPE	CV_8UC3

using namespace cv;
using namespace std;


// Note that this code is for the case when an input data is a color value.
int main() {

	Mat input = imread("lena.jpg", CV_LOAD_IMAGE_COLOR);
	Mat output;


	if (!input.data)
	{
		std::cout << "Could not open" << std::endl;
		return -1;
	}

	input.convertTo(input, CV_32FC3, 1.0 / 255);
	namedWindow("Original", WINDOW_AUTOSIZE);
	imshow("Original", input);
	
	// if sigma == 0, it consider only intensity
	float sigma = 0.5;
		
	Mat samples1(input.rows * input.cols, 3, CV_32F);
	Mat samples2(input.rows * input.cols, 5, CV_32F);
	
	for (int y = 0; y < input.rows; y++) {
		for (int x = 0; x < input.cols; x++) {
			for (int z = 0; z < 3; z++) {
				samples1.at<float>(y + x*input.rows, z) = input.at<Vec3f>(y, x)[z];
				samples2.at<float>(y + x*input.rows, z) = input.at<Vec3f>(y, x)[z];
			}
			samples2.at<float>(y + x*input.rows, 3) = sigma * ((float)x / input.cols);
			samples2.at<float>(y + x*input.rows, 4) = sigma * ((float)y / input.rows);
		}
	}

	// Clustering is performed for each channel (RGB)
	// Note that the intensity value is not normalized here (0~1). You should normalize both intensity and position when using them simultaneously.
	int clusterCount = 4;
	Mat labels1, labels2;
	int attempts = 5;
	Mat centers1, centers2;
	
	kmeans(samples1, clusterCount, labels1, TermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 10000, 0.0001), attempts, KMEANS_PP_CENTERS, centers1);
	kmeans(samples2, clusterCount, labels2, TermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 10000, 0.0001), attempts, KMEANS_PP_CENTERS, centers2);
	
	Mat new_image1(input.size(), input.type());
	Mat new_image2(input.size(), input.type());

	for (int y = 0; y < input.rows; y++) {
		for (int x = 0; x < input.cols; x++)
		{
			int cluster_idx1 = labels1.at<int>(y + x*input.rows, 0);
			int cluster_idx2 = labels2.at<int>(y + x*input.rows, 0);
			//Fill code that finds for each pixel of each channel of the output image the intensity of the cluster center.
			//new_image.at<float>(y, x) = centers.at<float>(cluster_idx, 0);
			for (int z = 0; z < 3; z++) {
				new_image1.at<Vec3f>(y, x)[z] = centers1.at<float>(cluster_idx1, z);
				new_image2.at<Vec3f>(y, x)[z] = centers2.at<float>(cluster_idx2, z);
			}

		}
	}
	imshow("label map(Intensity)", new_image1);
	imshow("label map(Intensity + position)", new_image2);
	waitKey(0);

	return 0;
}

