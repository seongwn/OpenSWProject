Open Software Project - Assignment1
1517034 정성원

1. rotation.cpp
-NN과 bilineaer interpolation을 이용해 이미지 회전
-lena.jpg 사용
-Mat myrotate(const Mat input, float angle, const char* opt);
  input image를 opt method(bilinear/nearest)를 이용해 angle만큼 회전시킨 이미지를 리턴하는 함수
-int nearest(float a);
  float a와 가장 가까운 정수를 리턴하는 함수
-int bilinear(const Mat input, float x, float y, int ch);
  floating point pixel이 주어졌을 때 bilinear interpolation를 이용해 해당 pixel의 intensity를 예측하는 함수

2. stitching.cpp
-affine transformation을 이용해 이미지 stitching
-Img01.jpg, Img02.jpg를 사용
