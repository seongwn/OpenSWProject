Open Software Project
Assignment7 README
경영학부 1517034 정성원

1. AffineStitching.cpp
-Affine Transform을 이용해 Image Stitching을 구현한 소스 코드
-Feature Matching을 통해 두 이미지의 corresponding point를 찾음
-Feature Matching과 Image Stitching을 위한 코드는 이전의 코드를 재사용
-if(ransac){} 절 을 새롭게 추가하여 RANSAC을 추가 구현

2. Hough-skeleton.cpp
-Opencv의 HoughLines와 HoughLinesP를 이용해 Line fitting을 구현한 소스 코드