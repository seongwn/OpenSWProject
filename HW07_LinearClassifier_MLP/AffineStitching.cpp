// reuse code of lab2(image stitching) and lab8(SIFT)

# include <opencv2/opencv.hpp>
# include <opencv2/nonfree/nonfree.hpp>
# include <iostream>
# include <vector>
# include <cmath>
# include <ctime>

# define RATIO_THR 0.4

using namespace std;
using namespace cv;

/*SIFT*/
double euclidDistance(Mat &vec1, Mat& vec2);
int nearestNeighbor(Mat& vec, vector<KeyPoint>& keypoints, Mat& descriptors, int nn);
void findPairs(vector<KeyPoint>& keypoints1, Mat& descriptors1,
	vector<KeyPoint>& keypoints2, Mat& descriptors2,
	vector<Point2f>& srcPoints, vector<Point2f>& dstPoints, bool crossCheck, bool ratio_threshold);

/*Affine Transformation*/
template <typename T>
Mat cal_affine(int ptl_x[], int ptl_y[], int ptr_x[], int ptr_y[], int num_points);
void blend_stitching(const Mat I1, const Mat I2, Mat &I_f, int diff_x, int diff_y, float alpha);

int main() {
	/*Load Data*/
	Mat input1 = imread("Img01.jpg", CV_LOAD_IMAGE_COLOR);
	Mat input2 = imread("Img02.jpg", CV_LOAD_IMAGE_COLOR);
	Mat input1_gray, input2_gray;

	if (!input1.data || !input2.data) {
		cout << "Could not open" << endl;
		return -1;
	}
	//resize & convert RGB > Gray
		
	resize(input1, input1, Size(input1.cols / 2, input1.rows / 2));
	resize(input2, input2, Size(input2.cols / 2, input2.rows / 2));

	
	cvtColor(input1, input1_gray, CV_RGB2GRAY);
	cvtColor(input2, input2_gray, CV_RGB2GRAY);

	input1.convertTo(input1, CV_32FC3, 1.0 / 255);
	input2.convertTo(input2, CV_32FC3, 1.0 / 255);


	/*Feature Matching*/
	FeatureDetector* detector = new SiftFeatureDetector(
		0,	// nFeatures
		4,	// nOctaveLayers
		0.04,	// contrastThreshold
		10,	// edgeThreshold
		1.6	// sigma
	);

	DescriptorExtractor * extractor = new SiftDescriptorExtractor();

	// Create Image for display matching keypoints
	Size size = input2.size();
	Size sz = Size(size.width + input1_gray.size().width, max(size.height, input1_gray.size().height));
	Mat matchingImage = Mat::zeros(sz, CV_8UC3);

	// Compute keypoints and descriptor from the source & target images
	vector<KeyPoint> keypoints1;
	Mat descriptors1;
	detector->detect(input1_gray, keypoints1);	// detect keypoints of input1_gray
	extractor->compute(input1_gray, keypoints1, descriptors1);	// get feature descriptors of input1_gray
	printf("input1: %d keypoints are found.\n", (int)keypoints1.size());
	
	vector<KeyPoint> keypoints2;
	Mat descriptors2;
	detector->detect(input2_gray, keypoints2);	// detect keypoints of input2_gray
	extractor->compute(input2_gray, keypoints2, descriptors2);	// get feature descriptors of input2_gray
	printf("input2: %d keypoints are found.\n", (int)keypoints2.size());

	/*
	for (int i = 0; i < keypoints1.size(); i++) {
		KeyPoint kp = keypoints1[i];
		kp.pt.x += size.width;
		circle(matchingImage, kp.pt, cvRound(kp.size * 0.25), Scalar(255, 255, 0), 1, 8, 0);
	}

	for (int i = 0; i < keypoints2.size(); i++) {
		KeyPoint kp = keypoints2[i];
		circle(matchingImage, kp.pt, cvRound(kp.size * 0.25), Scalar(255, 255, 0), 1, 8, 0);
	}
	*/

	/*Find nearest neighbor pairs*/
	vector<Point2f> srcPoints;
	vector<Point2f> dstPoints;
	bool crossCheck = true;
	bool ratio_threshold = true;
	findPairs(keypoints2, descriptors2, keypoints1, descriptors1, srcPoints, dstPoints, crossCheck, ratio_threshold);
	printf("%zd keypoints are matched.\n", srcPoints.size());

	/*Affine Transform Estimation*/
	const float I1_row = input1.rows;
	const float I1_col = input1.cols;
	const float I2_row = input2.rows;
	const float I2_col = input2.cols;

	// Corresponding points
	int num_points = srcPoints.size();
	int* ptl_x = (int*)malloc(sizeof(int) * num_points);
	int* ptl_y = (int*)malloc(sizeof(int) * num_points);
	int* ptr_x = (int*)malloc(sizeof(int) * num_points);
	int* ptr_y = (int*)malloc(sizeof(int) * num_points);

	for (int i = 0; i < num_points; i++) {
		ptl_x[i] = dstPoints[i].y;
		ptl_y[i] = dstPoints[i].x;
		ptr_x[i] = srcPoints[i].y;
		ptr_y[i] = srcPoints[i].x;
	}


	Mat A12, A21;

	A12 = cal_affine<float>(ptl_x, ptl_y, ptr_x, ptr_y, num_points);
	A21 = cal_affine<float>(ptr_x, ptr_y, ptl_x, ptl_y, num_points);

	
	bool ransac = true;
	if (ransac) {
		// RANSAC Parameters
		int K = 3; // 3; 
		int S = 50;
		double thres = 3.84;
		
		int bestScore = 0;
		Mat bestT;

		int* sptl_x = (int*)malloc(sizeof(int) * K);
		int* sptl_y = (int*)malloc(sizeof(int) * K);
		int* sptr_x = (int*)malloc(sizeof(int) * K);
		int* sptr_y = (int*)malloc(sizeof(int) * K);

		for (int s = 0; s < S; s++) {
			// Randomly sample k data
			srand((unsigned int)time(NULL));
			int* picked = (int*)calloc(num_points, sizeof(int));
			int k = 0;
			while (k < K) {
				int randn = rand() % num_points;
				if (!picked[randn]) {
					sptl_x[k] = ptl_x[randn];
					sptl_y[k] = ptl_y[randn];
					sptr_x[k] = ptr_x[randn];
					sptr_y[k] = ptr_y[randn];
					picked[randn] = 1;
					k ++;
				}	
			}
			// Estimate the affine transformation
			Mat A12 = cal_affine<float>(ptl_x, ptl_y, ptr_x, ptr_y, num_points);


			// Score by computing the number of inliers 
			int score = 0;
			for (int i = 0; i < num_points; i++) {
				float Tpx = A12.at<float>(0) * dstPoints[i].y + A12.at<float>(1) * dstPoints[i].x + A12.at<float>(2);
				float Tpy = A12.at<float>(3) * dstPoints[i].y + A12.at<float>(4) * dstPoints[i].x + A12.at<float>(5);
				float dist = sqrt((Tpx - srcPoints[i].y)*(Tpx - srcPoints[i].y) + (Tpy - srcPoints[i].x)*(Tpy - srcPoints[i].x));
				if (dist < thres)
					score++;
			}
			// Select the best affine transformation
			if (score > bestScore) {
				bestScore = score;
				bestT = A12;
			}
		}
	
		// Re-estimate the affine transformation by solving Mx = b with Tb's inliers
		// Get inliers
		int* iptl_x = (int*)malloc(sizeof(int)*bestScore);
		int* iptl_y = (int*)malloc(sizeof(int)*bestScore);
		int* iptr_x = (int*)malloc(sizeof(int)*bestScore);
		int* iptr_y = (int*)malloc(sizeof(int)*bestScore);
		int j = 0;
		std::cout << "BestT: " << bestT << endl;
		std::cout << "BestScore: " << bestScore << endl;
		for(int i = 0; i < num_points; i++) {
			float Tpx = bestT.at<float>(0) * dstPoints[i].y + bestT.at<float>(1) * dstPoints[i].x + bestT.at<float>(2);
			float Tpy = bestT.at<float>(3) * dstPoints[i].y + bestT.at<float>(4) * dstPoints[i].x + bestT.at<float>(5);
			float dist = sqrt((Tpx - srcPoints[i].y)*(Tpx - srcPoints[i].y) + (Tpy - srcPoints[i].x)*(Tpy - srcPoints[i].x));
			if (dist < thres) {
				iptl_x[j] = dstPoints[i].y;
				iptl_y[j] = dstPoints[i].x;
				iptr_x[j] = srcPoints[i].y;
				iptr_y[j] = srcPoints[i].x;
				j++;
			}

		}


		// Re-estimate
		//cout << "A12" << A12 << endl;
		//cout << "A21" << A21 << endl;
		A12 = cal_affine<float>(iptl_x, iptl_y, iptr_x, iptr_y, bestScore);
		A21 = cal_affine<float>(iptr_x, iptr_y, iptl_x, iptl_y, bestScore);
		//cout << "A12" << A12 << endl;
		//cout << "A21" << A21 << endl;
	}

	// Compute corners (p1, p2, p3, p4)
	Point2f p1(A21.at<float>(0) * 0 + A21.at<float>(1) * 0 + A21.at<float>(2),
		A21.at<float>(3) * 0 + A21.at<float>(4) * 0 + A21.at<float>(5));

	Point2f p2(A21.at<float>(0) * 0 + A21.at<float>(1) * I2_col + A21.at<float>(2),
		A21.at<float>(3) * 0 + A21.at<float>(4) * I2_col + A21.at<float>(5));

	Point2f p3(A21.at<float>(0) * I2_row + A21.at<float>(1) * 0 + A21.at<float>(2),
		A21.at<float>(3) * I2_row + A21.at<float>(4) * 0 + A21.at<float>(5));

	Point2f p4(A21.at<float>(0) * I2_row + A21.at<float>(1) * I2_col + A21.at<float>(2),
		A21.at<float>(3) * I2_row + A21.at<float>(4) * I2_col + A21.at<float>(5));

	// For inverse warping
	Point2f p1_(A12.at<float>(0) * 0 + A12.at<float>(1) * 0 + A12.at<float>(2),
		A12.at<float>(3) * 0 + A12.at<float>(4) * 0 + A12.at<float>(5));

	Point2f p2_(A12.at<float>(0) * 0 + A12.at<float>(1) * I1_col + A12.at<float>(2),
		A12.at<float>(3) * 0 + A12.at<float>(4) * I1_col + A12.at<float>(5));

	Point2f p3_(A12.at<float>(0) * I1_row + A12.at<float>(1) * 0 + A12.at<float>(2),
		A12.at<float>(3) * I1_row + A12.at<float>(4) * 0 + A12.at<float>(5));

	Point2f p4_(A12.at<float>(0) * I1_row + A12.at<float>(1) * I1_col + A12.at<float>(2),
		A12.at<float>(3) * I1_row + A12.at<float>(4) * I1_col + A12.at<float>(5));



	// Compute Boundary for inverse warping
	int bound_u = (int)round(min(0.0f, min(p1.x, p2.x)));
	int bound_b = (int)round(std::max(I1_row, std::max(p3.x, p4.x)));
	int bound_l = (int)round(min(0.0f, min(p1.y, p3.y)));
	int bound_r = (int)round(std::max(I1_col, std::max(p2.y, p4.y)));

	int bound_u_ = (int)round(min(0.0f, min(p1_.x, p2_.x)));
	int bound_b_ = (int)round(std::max(I2_row, std::max(p3_.x, p4_.x)));
	int bound_l_ = (int)round(min(0.0f, min(p1_.y, p3_.y)));
	int bound_r_ = (int)round(std::max(I2_col, std::max(p2_.y, p4_.y)));

	int diff_x = abs(bound_u);
	int diff_y = abs(bound_l);

	int diff_x_ = abs(bound_u_);
	int diff_y_ = abs(bound_l_);

	// Initialize merged Image


	Mat I_f(bound_b - bound_u + 1, bound_r - bound_l + 1, CV_32FC3, Scalar(0));



	// Inverse warping with bilinear interpolation
	for (int i = -diff_x_; i < I_f.rows - diff_x_; i++) {
		for (int j = -diff_y_; j < I_f.cols - diff_y_; j++) {
			float x = A12.at<float>(0) * i + A12.at<float>(1) * j + A12.at<float>(2) + diff_x_;
			float y = A12.at<float>(3) * i + A12.at<float>(4) * j + A12.at<float>(5) + diff_y_;

			int y1 = floor(y);
			int y2 = ceil(y);
			int x1 = floor(x);
			int x2 = ceil(x);

			float mu = y - y1;
			float lambda = x - x1;

			if (x1 >= 0 && x2 < I2_row && y1 >= 0 && y2 < I2_col) {
				for (int k = 0; k < 3; k++) {
					float f1 = mu * input2.at<Vec3f>(x1, y2)[k] + (1 - mu) * input2.at<Vec3f>(x1, y1)[k];
					float f2 = mu * input2.at<Vec3f>(x2, y2)[k] + (1 - mu) * input2.at<Vec3f>(x2, y1)[k];
					I_f.at<Vec3f>(i + diff_x_, j + diff_y_)[k] += lambda * f2 + (1 - lambda) * f1;
				}
			}

		}
	}
	// image stitching with blend

	blend_stitching(input1, input2, I_f, diff_x, diff_y, 0.5);

	cv::namedWindow("Left Image");
	cv::imshow("Left Image", input1);

	cv::namedWindow("Right Image");
	cv::imshow("Right Image", input2);

	cv::namedWindow("result");
	cv::imshow("result", I_f);

	cv::waitKey(0);

}

/*SIFT Functions*/
// Caculate euclidian distance between vec1 and vec2
double euclidDistance(Mat &vec1, Mat& vec2) {
	double sum = 0.0;
	int dim = vec1.cols;
	for (int i = 0; i < dim; i++) 
		sum += (vec1.at<float>(0, i) - vec2.at<float>(0, i)) * (vec1.at<float>(0, i) - vec2.at<float>(0, i));
	
	return sqrt(sum);
}

// Find the index of nearest neighbor point from keypoints
int nearestNeighbor(Mat& vec, vector<KeyPoint>& keypoints, Mat& descriptors, int nn) {
	int neighbor = -1;
	double minDist = 1e6;
	for (int i = 0; i < descriptors.rows; i++) {
		Mat v = descriptors.row(i);
		double dist = euclidDistance(v, vec);
		if (dist < minDist && i != nn) {
			minDist = dist;
			neighbor = i;
		}
	}
	return neighbor;
}

// Find pairs of points with the smallest distance between them
void findPairs(vector<KeyPoint>& keypoints1, Mat& descriptors1,
	vector<KeyPoint>& keypoints2, Mat& descriptors2,
	vector<Point2f>& srcPoints, vector<Point2f>& dstPoints, bool crossCheck, bool ratio_threshold) {
	for (int i = 0; i < descriptors1.rows; i++) {
		KeyPoint pt1 = keypoints1[i];
		Mat desc1 = descriptors1.row(i);
		int nn = nearestNeighbor(desc1, keypoints2, descriptors2, -1);
		// Refine matching points using ratio_based thresholding
		if (ratio_threshold) {
			int nn2 = nearestNeighbor(desc1, keypoints2, descriptors2, nn);
			double ratio = (euclidDistance(desc1, descriptors2.row(nn)) / euclidDistance(desc1, descriptors2.row(nn2)));
			if (ratio >= RATIO_THR)
				continue;
		}
		// Refine matching points using cross-checking
		if (crossCheck) 
			if (nearestNeighbor(descriptors2.row(nn), keypoints1, descriptors1, -1) != i)
				continue;

		KeyPoint pt2 = keypoints2[nn];
		srcPoints.push_back(pt1.pt);
		dstPoints.push_back(pt2.pt);
	}
	
}

/*Affine Transformation Functions*/
template <typename T>
Mat cal_affine(int ptl_x[], int ptl_y[], int ptr_x[], int ptr_y[], int num_points){
	Mat M(2 * num_points, 6, CV_32F, Scalar(0));
	Mat b(2 * num_points, 1, CV_32F, Scalar(0));
	Mat M_trans, temp, affineM;

	// Initialize matrix
	for (int i = 0; i < num_points; i++) {
		M.at<float>(i * 2, 0) = M.at<float>(i * 2 + 1, 3) = ptl_x[i];
		M.at<float>(i * 2, 1) = M.at<float>(i * 2 + 1, 4) = ptl_y[i];
		M.at<float>(i * 2, 2) = M.at<float>(i * 2 + 1, 5) = 1;
		b.at<float>(i * 2, 0) = ptr_x[i];
		b.at<float>(i * 2 + 1, 0) = ptr_y[i];
	}

	M_trans = M.t();
	affineM = (M_trans * M).inv() * M_trans * b;

	return affineM;
}

void blend_stitching(const Mat I1, const Mat I2, Mat &I_f, int diff_x, int diff_y, float alpha){
	int bound_x = I1.rows + diff_x;
	int bound_y = I1.cols + diff_y;
	int col = I_f.cols;
	int row = I_f.rows;

	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			// For check validation of I1, I2
			bool cond1 = (i < bound_x && i > diff_x) && (j < bound_y && j > diff_y) ? true : false;
			bool cond2 = I_f.at<Vec3f>(i, j) != Vec3f(0, 0, 0) ? true : false;
			// I2 is already in I_f by inverse warping
			// So, it is not necessary to check that only I2 is valid
			// If both are valid
			if (cond1 && cond2) {
				I_f.at<Vec3f>(i, j) = alpha * I1.at<Vec3f>(i - diff_x, j - diff_y) + (1 - alpha) * I_f.at<Vec3f>(i, j);
			}
			// Only I1 is valid
			else if (cond1) {
				I_f.at<Vec3f>(i, j) = I1.at<Vec3f>(i - diff_x, j - diff_y);
			}

		}
	}
}
