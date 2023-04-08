Open Software Project Assignment3
경영학과 1517034 정성원

HW1. Gaussian Filter
0. Separable Gaussian Filter는 인풋 이미지의 종류에 따라 GaussianFilterGray.cpp 과 GaussianFilterRGB.cpp에 각각 나누어 구현했다.
1. main function
-Mat naive_output과 output은 각각 original gaussian 함수의 결과, separable gaussian 함수의 결과를 저장하기 위해 선언했다.
-original gaussian 함수와 separable gaussian 함수의 연산 속도를 비교하기 위해 clock_t start, end 변수를 이용했다. 연산의 결과는 각각 float type 변수인 res1, res2에 저장했다.

2. Mat naive_gaussianfilter(const Mat input, int n, float sigmaT, float sigmaS, const char* opt)
-Input:
const Mat input: 가우시안 필터를 적용할 입력 이미지
int n: 가우시안 필터의 크기(2*n + 1)
float sigmaT, sigmaS: 가우시안 필터에 이용할 표준편차
const char* opt: Image boundary processing 방식('zero-paddle', 'mirroring', 'adjustkernel')
-Output:
입력받은 입력이미지에 2 * n + 1 가우시안 필터를 적용한 결과(Mat type)

3. Mat gaussianfilter_sep(const Mat input, int n, float sigmaT, float sigmaS, const char* opt)
-Input:
const Mat input: 가우시안 필터를 적용할 입력 이미지
int n: 가우시안 필터의 크기(2*n + 1)
float sigmaT, sigmaS: 가우시안 필터에 이용할 표준편차
const char* opt: Image boundary processing 방식('zero-paddle', 'mirroring', 'adjustkernel')
-Output:
입력받은 입력이미지에 2 * n + 1 가우시안 필터를 적용한 결과(Mat type)


HW2. Unsharp Masking
0. Unsharp Masking은 인풋 이미지의 종류에 따라 UnsharpMaskingGray.cpp와 UnsharpMaskingRGB.cpp에 각각 나누어 구현했다.
1. Mat gaussianfilter_sep(const Mat input, int n, float sigmaT, float sigmaS, const char* opt)
-앞서 구현한 함수와 동일

2. Mat unsharpMask(const Mat input, int n, float sigmaT, float sigmaS, const char*opt, float k)
-Input:
앞 5가지 파라미터는 가우스안 필터 함수의 인자로 다시 넘겨줌
float k: scale value
-Mat L: Low-pass 필터링 결과
-Output:
입력받은 입력이미지에 Unsharp mask를 적용한 결과(Mat type)

HW3. Convolution
컨볼루션 계산 풀이를 Technical Report.docx 3.6에 첨부했습니다.