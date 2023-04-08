#include "hist_func.h"
#include <iostream>

using namespace std;

void hist_eq(Mat &input, Mat &equalized, G *trans_func, float *CDF);
void func_inverse(G *trans_func, G *inverse_func);

int main() {
	/*
	Input: Original Image, Reference Image
	Output:
	-histogram-matched image
	-1 transfer function(txt file)
	-1 histogram of the original image(txt file)
	-1 histogram of the ouput image(txt file)
	*/
	Mat input = imread("input.jpg", CV_LOAD_IMAGE_COLOR);
	Mat input_matched_YUV;
	Mat ref = imread("ref.jpg", CV_LOAD_IMAGE_COLOR);
	Mat ref_matched_YUV;

	//1. Convert the color image RGB to YUV
	cvtColor(input, input_matched_YUV, CV_RGB2YUV);
	cvtColor(ref, ref_matched_YUV, CV_RGB2YUV);

	//Split each channel(Y, U, V);
	Mat input_channels[3];	Mat ref_channels[3];
	split(input_matched_YUV, input_channels);	split(ref_matched_YUV, ref_channels);
	Mat input_Y = input_channels[0]; Mat ref_Y = ref_channels[0];

	//PDF or transfer function txt files
	FILE *f_PDF_R, *f_PDF_G, *f_PDF_B;
	FILE *f_matched_PDF_R, *f_matched_PDF_G, *f_matched_PDF_B;
	FILE *f_trans_func_match_YUV;

	fopen_s(&f_PDF_R, "PDF_R(match).txt", "w+"); fopen_s(&f_PDF_G, "PDF_G(match).txt", "w+"); fopen_s(&f_PDF_B, "PDF_B(match).txt", "w+");
	fopen_s(&f_matched_PDF_R, "matched_PDF_R.txt", "w+"); fopen_s(&f_matched_PDF_G, "matched_PDF_G.txt", "w+"); fopen_s(&f_matched_PDF_B, "matched_PDF_B.txt", "w+");
	fopen_s(&f_trans_func_match_YUV, "trans_func_match_YUV.txt", "w+");


	// 2. Apply the histogram matching for Y channel only
	// Compute the transfer functions s = T(r) for histogram equalization of input's Y channel, and perform histogram equalization
	float **PDF_input_RGB = cal_PDF_RGB(input);
	float *CDF_input_YUV = cal_CDF(input_Y);

	G Tr_YUV[L] = { 0 }; //transfer function s = T(r)
	hist_eq(input_Y, input_Y, Tr_YUV, CDF_input_YUV);

	// Compute the transfer functions s = G(z) for histogram equalization of ref's Y channel
	//float **PDF_ref_RGB = cal_PDF_RGB(ref);
	float *CDF_ref_YUV = cal_CDF(ref_Y);

	G Gz_YUV[L] = { 0 }; //transfer function s = T(r)
	hist_eq(ref_Y, ref_Y, Gz_YUV, CDF_ref_YUV);

	// Apply the intensity mapping from r to z
	G inverseGz[L] = { 0 };
	func_inverse(Gz_YUV, inverseGz);

	// perform the transfer function
	for (int i = 0; i < input.rows; i++)
		for (int j = 0; j < input.cols; j++)
			input_Y.at<G>(i, j) = inverseGz[Tr_YUV[input_Y.at<G>(i, j)]];


	//3. Convert the histogram-matched Y and UV to the color image(RGB)
	// merge Y. U, V channels
	merge(input_channels, 3, input_matched_YUV);
	// YUV -> RGB
	cvtColor(input_matched_YUV, input_matched_YUV, CV_YUV2RGB);
	// output PDF
	float **PDF_output_RGB = cal_PDF_RGB(input_matched_YUV);

	//write files
	for (int i = 0; i < L; i++) {
		// write PDF of original image
		fprintf(f_PDF_R, "%d\t%f\n", i, PDF_input_RGB[i][0]); fprintf(f_PDF_G, "%d\t%f\n", i, PDF_input_RGB[i][1]); fprintf(f_PDF_B, "%d\t%f\n", i, PDF_input_RGB[i][2]);
		// write PDF of output image
		fprintf(f_matched_PDF_R, "%d\t%f\n", i, PDF_output_RGB[i][0]); fprintf(f_matched_PDF_G, "%d\t%f\n", i, PDF_output_RGB[i][1]); fprintf(f_matched_PDF_B, "%d\t%f\n", i, PDF_output_RGB[i][2]);
		// write transfer functions
		fprintf(f_trans_func_match_YUV, "%d\t%d\n", i, inverseGz[Tr_YUV[i]]);
	}

	//file close
	fclose(f_PDF_R); fclose(f_PDF_G); fclose(f_PDF_B);
	fclose(f_matched_PDF_R); fclose(f_matched_PDF_G); fclose(f_matched_PDF_B);
	fclose(f_trans_func_match_YUV);


	////////////////////// Show each image ///////////////////////
	namedWindow("RGB", WINDOW_AUTOSIZE);
	imshow("RGB", input);

	namedWindow("Equalized_YUV", WINDOW_AUTOSIZE);
	imshow("Equalized_YUV", input_matched_YUV);
	//////////////////////////////////////////////////////////////
	waitKey(0);
	return 0;

}

// histogram equalization
void hist_eq(Mat &input, Mat &equalized, G *trans_func, float *CDF) {

	// compute transfer function
	for (int i = 0; i < L; i++)
		trans_func[i] = (G)((L - 1) * CDF[i]);

	// perform the transfer function
	for (int i = 0; i < input.rows; i++)
		for (int j = 0; j < input.cols; j++)
			equalized.at<G>(i, j) = trans_func[input.at<G>(i, j)];

}

//get inverse function G-1
void func_inverse(G *trans_func, G *inverse_func) {
	/* Return z = G-1(s) */
	bool checked[L] = { 0 };
	for (int i = 0; i < L; i++) {
		if (checked[trans_func[i]])
			continue;
		inverse_func[trans_func[i]] = (G)i;
		checked[trans_func[i]] = 1;
	}

	for (int i = 0; i < L; i++)
		if (!checked[i])
			inverse_func[i] = (G)inverse_func[i - 1];
}