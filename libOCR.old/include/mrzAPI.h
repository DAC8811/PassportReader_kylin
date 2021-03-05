#include <opencv2/core/core.hpp>
#include <string>

std::string mrzOcrAPI(cv::Mat pic, int type, std::string templatePath);

cv::Mat findMrzAPI(const cv::Mat &pic, int type, int threshold);
