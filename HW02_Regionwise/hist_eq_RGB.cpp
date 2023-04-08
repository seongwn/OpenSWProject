# include "hist_func.h"
# include <iostream>

using namespace std;

void hist_eq_Color(Mat &input, Mat &equalized, G(*trans_func)[3], float **CDF);

int main() {

	Mat input = imread("input.jpg", CV_LOAD_IMAGE_COLOR);
	Mat equalized_RGB = input.clone();

	// PDF or transfer function txt files
	FILE *f_PDF_R, *f_PDF_G, *f_PDF_B;
	FILE *f_equalized_PDF_R, *f_equalized_PDF_G, *f_equalized_PDF_B;
	FILE *f_trans_func_eq_R, *f_trans_func_eq_G, *f_trans_func_eq_B;
	
	fopen_s(&f_PDF_R, "PDF_R.txt", "w+"); fopen_s(&f_PDF_G, "PDF_G.txt", "w+"); fopen_s(&f_PDF_B, "PDF_B.txt", "w+");
	fopen_s(&f_equalized_PDF_R, "equalized_PDF_R.txt", "w+"); fopen_s(&f_equalized_PDF_G, "equalized_PDF_G.txt", "w+"); fopen_s(&f_equalized_PDF_B, "equalized_PDF_B.txt", "w+");
	fopen_s(&f_trans_func_eq_R, "trans_func_eq_R.txt", "w+"); fopen_s(&f_trans_func_eq_G, "trans_func_eq_G.txt", "w+"); fopen_s(&f_trans_func_eq_B, "trans_func_eq_B.txt", "w+");

	float **PDF_RGB = cal_PDF_RGB(input);	// PDF of Input image(RGB) : [L][3]
	float **CDF_RGB = cal_CDF_RGB(input);	// CDF of Input image(RGB) : [L][3]

	G trans_func_eq_RGB[L][3] = { 0 };		// transfer function

	// histogram equalization on RGB image
	hist_eq_Color(input, equalized_RGB, trans_func_eq_RGB, CDF_RGB);

	// equalized PDF (RGB)
	float **equalized_PDF_RGB = cal_PDF_RGB(equalized_RGB);
	
	//write files
	for (int i = 0; i < L; i++) {
		// write PDF of original image
		fprintf(f_PDF_R, "%d\t%f\n", i, PDF_RGB[i][0]); fprintf(f_PDF_G, "%d\t%f\n", i, PDF_RGB[i][1]); fprintf(f_PDF_B, "%d\t%f\n", i, PDF_RGB[i][2]);
		// write PDF of output image
		fprintf(f_equalized_PDF_R, "%d\t%f\n", i, equalized_PDF_RGB[i][0]); fprintf(f_equalized_PDF_G, "%d\t%f\n", i, equalized_PDF_RGB[i][1]); fprintf(f_equalized_PDF_B, "%d\t%f\n", i, equalized_PDF_RGB[i][2]);
		// write transfer functions
		fprintf(f_trans_func_eq_R, "%d\t%d\n", i, trans_func_eq_RGB[i][0]); fprintf(f_trans_func_eq_G, "%d\t%d\n", i, trans_func_eq_RGB[i][1]); fprintf(f_trans_func_eq_B, "%d\t%d\n", i, trans_func_eq_RGB[i][2]);
	}

	// memory release
	free(PDF_RGB);
	free(CDF_RGB);
	fclose(f_PDF_R); fclose(f_PDF_G); fclose(f_PDF_B);
	fclose(f_equalized_PDF_R); fclose(f_equalized_PDF_G); fclose(f_equalized_PDF_B);
	fclose(f_trans_func_eq_R); fclose(f_trans_func_eq_G); fclose(f_trans_func_eq_B);

	////////////////////// Show each image ///////////////////////

	namedWindow("RGB", WINDOW_AUTOSIZE);
	imshow("RGB", input);

	namedWindow("Equalized_RGB", WINDOW_AUTOSIZE);
	imshow("Equalized_RGB", equalized_RGB);

	//////////////////////////////////////////////////////////////

	waitKey(0);

	return 0;
}

// histogram equalization on 3 channel image
void hist_eq_Color(Mat &input, Mat &equalized, G(*trans_func)[3], float **CDF) {
	////////////////////////////////////////////////
	//											  //
	// How to access multi channel matrix element //
	//											  //
	// if matrix A is CV_8UC3 type,				  //
	// A(i, j, k) -> A.at<Vec3b>(i, j)[k]		  //
	//											  //
	////////////////////////////////////////////////
	// compute transfer function
	for (int i = 0; i < L; i++)
		for(int k = 0; k < 3 ; k++)
			trans_func[i][k] = (G)((L - 1) * CDF[i][k]);

	// perform the transfer function
	for (int i = 0; i < input.rows; i++)
		for (int j = 0; j < input.cols; j++)
			for (int k =0; k < 3; k++)
				equalized.at<C>(i, j)[k] = trans_func[input.at<C>(i, j)[k]][k];
}