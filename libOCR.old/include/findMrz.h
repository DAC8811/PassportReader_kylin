#pragma once
#include <opencv2/opencv.hpp>
#include <string>
#include "utils.h"
#include "config.h"

cv::Mat removeOutlierContours(cv::Mat &img);

cv::Mat findMrz(const cv::Mat &img, int type = 0, int threshold = 20);

cv::Mat findMrz(std::string imgPath, int type = 0, int threshold = 20);