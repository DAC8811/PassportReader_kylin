#pragma once
#include <opencv2/opencv.hpp>
#include <string>
#include "utils.h"


cv::Mat findMrz(const cv::Mat& img, int type);
int findMRZ(cv::Mat image,std::vector<cv::Mat>& result);