
#pragma once
#include <opencv2/opencv.hpp>
#include <string>
#include "utils.h"
#include "config.h"

double getTheta(const cv::Mat &img);

double getTheta(std::string imgPath);

cv::Mat rotate(const cv::Mat &img, double theta);

cv::Mat rotate(std::string imgPath, double theta);