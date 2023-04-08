1. hist_match.cpp
-gray scale의 input image(input.jpg)를 reference image(ref.jpg)에 맞도록 histogram matching 수행
1) void hist_eq(Mat &input, Mat &equalized, G * trans_func, float *CDF)
-주어진 input image와 CDF를 이용해 histogram equalization를 수행하는 함수
-Parameters:
input: original image
equalized: histogram equalized result
trans_fun: transfer function for histogram equalization
CDF: CDF of input image

2) void func_inverse(G *transfer_func, G * inverse_func)
-주어진 transfer function의 역함수를 구하는 함수
-Parameters:
trans_func: 역함수를 구하고자하는 함수(input)
inverse_func: 주어진 함수의 역함수(output)
-checked[L]: Intensity i(i = 0, ..., 255)의 역함수값이 정의되었는지 확인하는 boolean array
	     만일 역함수값이 정의되지 않았을 경우, neighbor intensity의 역함수 값을 선택

2. hist_match_YUV.cpp
-color 이미지(RGB) YUV 이미지로 변환 후 Y 채널에 대해서 histogram matching 수행
1) void hist_eq(Mat &input, Mat &equalized, G * trans_func, float *CDF)
-주어진 input image와 CDF를 이용해 histogram equalization를 수행하는 함수
-Parameters:
input: original image
equalized: histogram equalized result
trans_fun: transfer function for histogram equalization
CDF: CDF of input image

2) void func_inverse(G *transfer_func, G * inverse_func)
-주어진 transfer function의 역함수를 구하는 함수
-Parameters:
trans_func: 역함수를 구하고자하는 함수(input)
inverse_func: 주어진 함수의 역함수(output)
-checked[L]: Intensity i(i = 0, ..., 255)의 역함수값이 정의되었는지 확인하는 boolean array
	     만일 역함수값이 정의되지 않았을 경우, neighbor intensity의 역함수 값을 선택