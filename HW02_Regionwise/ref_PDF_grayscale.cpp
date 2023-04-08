#include "hist_func.h"

int main() {
	Mat input = imread("ref.jpg", CV_LOAD_IMAGE_COLOR);
	Mat input_gray;
	cvtColor(input, input_gray, CV_RGB2GRAY);
	// PDF, CDF txt files
	FILE *f_PDF;

	fopen_s(&f_PDF, "ref_PDF.txt", "w+"); 

	// each histogram
	float *PDF_gray = cal_PDF(input_gray);

	//write files
	for (int i = 0; i < L; i++) {
		fprintf(f_PDF, "%d\t%f\n", i, PDF_gray[i]); 
	}

	// memory release
	fclose(f_PDF); 
	////////////////////// Show each image ///////////////////////

	//////////////////////////////////////////////////////////////

	waitKey(0);

	return 0;

}