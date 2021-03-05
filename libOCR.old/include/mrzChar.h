#pragma once

#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include "config.h"
#include "featureIO.h"

class mrzChar {
 public:
  mrzChar();

  mrzChar(cv::Mat &charImg, int x = 0, int y = 0, featureIO *fio = NULL, bool reverse = false);

//    mrzChar(std::string charImgPath, featureIO *fio = NULL, bool reverse = false);

  void show();

  std::vector<double> getFeature();

  std::vector<double> getFeatureAlternative();

  cv::Mat getCharImg();

  const char getCharacter();

  int getPosX();

  int getPosY();

  void setPos(int x, int y);

 private:

  void segment();

  void padAndResize();

  void constructRowPixelStatisticsFeature();

  void constructColPixelStatisticsFeature();

  void constructBlockPixelStatisticsFeature();

  void constructFeature();

  void constructFeatureAlternative();

  double calcSim(const std::vector<double> &feaTemplate);

  void classify();

  void subdivide();

  void subdivide_A4();

  void subdivide_0ODQ();

  void subdivide_NH();

  void subdivide_7Z2();

  void subdivide_EFP();

  void subdivide_XVW();

  void subdivide_TI();

  void compare(std::vector<std::vector<double>> &valueTable, std::vector<char> &charTable);

 private:
  cv::Mat charImg;
  double ratioHeightWidth;
  int height;
  int width;

  int TD;

  int posX;
  int posY;

  std::vector<double> rowPixelStatisticsFeature;

  std::vector<double> colPixelStatisticsFeature;

  std::vector<double> blockPixelStatisticsFeature;

  std::vector<double> feature;

  std::vector<double> featureAlternative;

  char character;

  featureIO *fio;

};

