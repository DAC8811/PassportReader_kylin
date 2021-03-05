#pragma once
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include "mrzChar.h"
#include "config.h"

class MRZ {
 public:
  MRZ(const cv::Mat &img, int type, int threshold, featureIO *fio);
  MRZ(const std::string &imgPath, int type, int threshold, featureIO *fio);
  ~MRZ();
  void showMrzImg(int windowMode);
  std::vector<mrzChar> getMrzChars();
  std::string getMrzStr();
 private:
  void mrzImgBinaryzation(int threshold);
  void mrzCharSegmentation();
  void generateMrzStr();
  void checkMrzStr(int type);
 private:
  cv::Mat mrzImg;
  std::vector<mrzChar> mrzChars;
  std::string mrzStr;
  int lineAmount;
  featureIO *fio;
};