#include "hist_func.h"

void hist_eq(Mat &input, Mat &equalized, G *trans_func, float *CDF);

int main() {
	Mat input = imread("input.jpg", CV_LOAD_IMAGE_COLOR);
	Mat equalized_YUV;

	cvtColor(input, equalized_YUV, CV_RGB2YUV);	// RGB -> YUV

	// split each channel(Y, U, V)
	Mat channels[3];
	split(equalized_YUV, channels);
	Mat Y = channels[0];						// U = channels[1], V = channels[2]

	// PDF or transfer function txt files
	FILE *f_PDF_R, *f_PDF_G, *f_PDF_B;
	FILE *f_equalized_PDF_YUV_R, *f_equalized_PDF_YUV_G, *f_equalized_PDF_YUV_B;
	FILE *f_trans_func_eq_YUV;

	float **PDF_RGB = cal_PDF_RGB(input);		// PDF of Input image(RGB) : [L][3]
	float *CDF_YUV = cal_CDF(Y);				// CDF of Y channel image

	fopen_s(&f_PDF_R, "PDF_R.txt", "w+"); fopen_s(&f_PDF_G, "PDF_G.txt", "w+"); fopen_s(&f_PDF_B, "PDF_B.txt", "w+");
	fopen_s(&f_equalized_PDF_YUV_R, "equalized_PDF_YUV_R.txt", "w+"); fopen_s(&f_equalized_PDF_YUV_G, "equalized_PDF_YUV_G.txt", "w+"); fopen_s(&f_equalized_PDF_YUV_B, "equalized_PDF_YUV_B.txt", "w+");
	fopen_s(&f_trans_func_eq_YUV, "trans_func_eq_YUV.txt", "w+");

	G trans_func_eq_YUV[L] = { 0 };			// transfer function

	// histogram equalization on Y channel
	hist_eq(Y, Y, trans_func_eq_YUV, CDF_YUV);

	// merge Y, U, V channels
	merge(channels, 3, equalized_YUV);

	// YUV -> RGB (use "CV_YUV2RGB" flag)
	cvtColor(equalized_YUV, equalized_YUV, CV_YUV2RGB);

	// equalized PDF (YUV)
	float **equalized_PDF_YUV = cal_PDF_RGB(equalized_YUV);

	//write files
	for (int i = 0; i < L; i++) {
		// write PDF of original image
		fprintf(f_PDF_R, "%d\t%f\n", i, PDF_RGB[i][0]); fprintf(f_PDF_G, "%d\t%f\n", i, PDF_RGB[i][1]); fprintf(f_PDF_B, "%d\t%f\n", i, PDF_RGB[i][2]);
		// write PDF of output image
		fprintf(f_equalized_PDF_YUV_R, "%d\t%f\n", i, equalized_PDF_YUV[i][0]); fprintf(f_equalized_PDF_YUV_G, "%d\t%f\n", i, equalized_PDF_YUV[i][1]); fprintf(f_equalized_PDF_YUV_B, "%d\t%f\n", i, equalized_PDF_YUV[i][2]);
		// write transfer functions
		fprintf(f_trans_func_eq_YUV, "%d\t%d\n", i, trans_func_eq_YUV[i]);
	}

	// memory release
	free(PDF_RGB);
	free(CDF_YUV);
	fclose(f_PDF_R); fclose(f_PDF_G); fclose(f_PDF_B);
 	fclose(f_equalized_PDF_YUV_R); fclose(f_equalized_PDF_YUV_G); fclose(f_equalized_PDF_YUV_B);
	fclose(f_trans_func_eq_YUV);

	////////////////////// Show each image ///////////////////////

	namedWindow("RGB", WINDOW_AUTOSIZE);
	imshow("RGB", input);

	namedWindow("Equalized_YUV", WINDOW_AUTOSIZE);
	imshow("Equalized_YUV", equalized_YUV);

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