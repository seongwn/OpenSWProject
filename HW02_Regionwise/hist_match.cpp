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
	Mat input_gray;
	Mat ref = imread("ref.jpg", CV_LOAD_IMAGE_COLOR);
	Mat ref_gray;

	cvtColor(input, input_gray, CV_RGB2GRAY);	// Convert RGB to Gray
	cvtColor(ref, ref_gray, CV_RGB2GRAY);	// Convert RGB to Gray

	Mat equalized = input_gray.clone();
	Mat equalized_ref = ref_gray.clone();
	Mat matched = input_gray.clone();

	//PDF or Transfer function txt
	FILE *f_PDF;
	//FILE *f_ref_PDF_gray;
	FILE *f_matched_PDF_gray;
	FILE *f_trans_func_match_gray;
	
	fopen_s(&f_PDF, "PDF.txt", "w+");
	//fopen_s(&f_ref_PDF_gray, "ref_PDF.txt", "w+");
	fopen_s(&f_matched_PDF_gray, "matched_PDF_gray.txt", "w+");
	fopen_s(&f_trans_func_match_gray, "trans_func_match_gray.txt", "w+");

	//1. Compute the transfer functions s = T(r) for histogram equalization of an input image, and perform histogram equalization
	float *PDF_input = cal_PDF(input_gray);	// PDF of Input image(Grayscale) : [L]
	float *CDF_input = cal_CDF(input_gray);	// CDF of Input image(Grayscale) : [L]

	G Tr[L] = { 0 };	// transfer function
	hist_eq(input_gray, equalized, Tr, CDF_input);	// histogram equalization on grayscale image
	
	//2. Compute the transfer functions s = G(z) for histogram equalization of the specified PDF
	float *PDF_ref = cal_PDF(ref_gray);	// equalized PDF (grayscale)
	float *CDF_ref = cal_CDF(ref_gray);
	
	G Gz[L] = { 0 };	// transfer function
	hist_eq(ref_gray, equalized_ref, Gz, CDF_ref);	
	
	float *z = cal_PDF(equalized_ref);
	//3. Apply the intensity mapping from r to z
	G inverseGz[L] = { 0 };
	func_inverse(Gz, inverseGz);

	// perform the transfer function
	for (int i = 0; i < input.rows; i++)
		for (int j = 0; j < input.cols; j++)
			matched.at<G>(i, j) = inverseGz[Tr[input_gray.at<G>(i, j)]];
	
	float *matched_PDF_gray = cal_PDF(matched);

	//write files
	for (int i = 0; i < L; i++) {
		// write PDF
		fprintf(f_PDF, "%d\t%f\n", i, PDF_input[i]);
		//fprintf(f_ref_PDF_gray, "%d\t%f\n", i, PDF_ref[i]);
		fprintf(f_matched_PDF_gray, "%d\t%f\n", i, matched_PDF_gray[i]);
		// write transfer functions
		fprintf(f_trans_func_match_gray, "%d\t%d\n", i, inverseGz[Tr[i]]);
	}
	
	// memory release
	fclose(f_PDF);
	//fclose(f_ref_PDF);
	fclose(f_matched_PDF_gray);
	fclose(f_trans_func_match_gray);
	
	////////////////////// Show each image ///////////////////////
	namedWindow("Grayscale", WINDOW_AUTOSIZE);
	imshow("Grayscale", input_gray);

	namedWindow("Matched", WINDOW_AUTOSIZE);
	imshow("Matched", matched);
	//////////////////////////////////////////////////////////////
	waitKey(0);
	
	return 0;
	
}

// histogram equalization
void hist_eq(Mat &input, Mat &equalized, G *trans_func, float *CDF) {
	// compute transfer function
	for (int i = 0; i < L; i++) {
		trans_func[i] = (G)((L - 1) * CDF[i]);
	}

	// perform the transfer function
	for (int i = 0; i < input.rows; i++)
		for (int j = 0; j < input.cols; j++)
			equalized.at<G>(i, j) = trans_func[input.at<G>(i, j)];
	
}

//get inverse function G-1
void func_inverse(G *trans_func, G *inverse_func){
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
