1. LoG-skeleton.cpp
LOG filter를 구현한 소스코드 파일
grayscale 이미지의 경우 Gaussianfilter -> Laplacianfilter
RGB 이미지의 경우 GaussianfilterRGB -> LaplacianfilterRGB 필터를 이용

2. Canny-skeleton.cpp
OpenCV의 Canny를 이용한 소스코드 파일

3. Harris_corner-skeleton.cpp
OpenCV의 cornerHarris를 이용한 소스코드 파일
Non Maximym Suppression은 Mat nonMaximum_Suppression을 이용해 구현함