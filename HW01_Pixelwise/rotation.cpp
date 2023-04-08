# include <opencv2/opencv.hpp>
# include <iostream>

using namespace cv;
using namespace std;

template <typename T>
Mat myrotate(const Mat input, float angle, const char* opt);
int nearest(float a);
int bilinear(const Mat input, float x, float y, int ch);

int main() {
	Mat input, rotated;

	//Read each image
	input = imread("lena.jpg");

	//Check for invalid input
	if (!input.data) {
		cout << "Could not open or find the image" << endl;
		return -1;
	}

	//original image  
	imshow("image", input);

	//rotated = myrotate<Vec3b>(input, 45, "nearest");
	rotated = myrotate<Vec3b>(input, 45, "bilinear");

	//rorated image
	imshow("rotated", rotated);

	waitKey(0);

	return 0;
}


template <typename T>
Mat myrotate(const Mat input, float angle, const char*opt) {
	int row = input.rows;
	int col = input.cols;

	float radian = angle * CV_PI / 180;

	float sq_row = ceil(row * sin(radian) + col * cos(radian));
	float sq_col = ceil(col * sin(radian) + row * cos(radian));
	
	Mat output = Mat::zeros(sq_row, sq_col, input.type());
	for (int i = 0; i < sq_row ; i++) {
		for (int j = 0; j < sq_col ; j++) {
			float x = (j - sq_col / 2) * cos(radian) - (i - sq_row / 2) * sin(radian) + col / 2;
			float y = (j - sq_col / 2) * sin(radian) + (i - sq_row / 2) * cos(radian) + row / 2;
			
			if ((y >= 0) && (y <= (row - 1)) && (x >= 0) && (x <= (col - 1))){
				if (!strcmp(opt, "nearest")) {
				//nearest method
					output.at<Vec3b>(i, j)[0] += input.at<Vec3b>(nearest(y), nearest(x))[0];
					output.at<Vec3b>(i, j)[1] += input.at<Vec3b>(nearest(y), nearest(x))[1];
					output.at<Vec3b>(i, j)[2] += input.at<Vec3b>(nearest(y), nearest(x))[2];
				}
				else if (!strcmp(opt, "bilinear")) {
				//bilinear method
					output.at<Vec3b>(i, j)[0] += bilinear(input, y, x, 0);
					output.at<Vec3b>(i, j)[1] += bilinear(input, y, x, 1);
					output.at<Vec3b>(i, j)[2] += bilinear(input, y, x, 2);
				}
			}
		}
	}
	return output;
}



int nearest(float a) {
	/*return nearest integer*/
	if (abs(a - (int)a) <= (abs(a + 1 - (int(a)))))
		return int(a);
	return int(a) + 1;
}

int bilinear(const Mat input, float x, float y, int ch) {
	/*return intensity of floating point using bilinear interpolation*/
	int x1 = int(x); int x2 = int(x) + 1;
	int y1 = int(y); int y2 = int(y) + 1;

	int temp1 = (x2 - x)*input.at<Vec3b>(x1, y2)[ch] + (x - x1)*input.at<Vec3b>(x1, y1)[ch];
	int temp2 = (x2 - x)*input.at<Vec3b>(x2, y2)[ch] + (x - x1)*input.at<Vec3b>(x2, y1)[ch];

	return (x2 - x) * temp2 + (x - x1) * temp1;
}
