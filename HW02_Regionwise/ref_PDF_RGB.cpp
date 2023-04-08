#include "hist_func.h"

int main(){
	Mat input = imread("ref.jpg", CV_LOAD_IMAGE_COLOR);
	
	// PDF, CDF txt files
	FILE *f_PDF_R, *f_PDF_G, *f_PDF_B;

	fopen_s(&f_PDF_R, "ref_PDF_R.txt", "w+"); fopen_s(&f_PDF_G, "ref_PDF_G.txt", "w+"); fopen_s(&f_PDF_B, "ref_PDF_B.txt", "w+");

	// each histogram
	float **PDF_RGB = cal_PDF_RGB(input);

	//write files
	for (int i = 0; i < L; i++) {
		fprintf(f_PDF_R, "%d\t%f\n", i, PDF_RGB[i][0]); fprintf(f_PDF_G, "%d\t%f\n", i, PDF_RGB[i][1]); fprintf(f_PDF_B, "%d\t%f\n", i, PDF_RGB[i][2]);
	}
	
	// memory release
	fclose(f_PDF_R); fclose(f_PDF_G); fclose(f_PDF_B);

	////////////////////// Show each image ///////////////////////

	//////////////////////////////////////////////////////////////

	waitKey(0);

	return 0;

}