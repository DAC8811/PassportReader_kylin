#pragma once
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>

std::vector<int> getDiffVec(std::vector<int> vec);

std::string randomStr();

int adaptiveBinaryzation(cv::Mat &img);

int otsuBinaryzation(cv::Mat &img);

int hsvBinaryzation(cv::Mat &img, int threshold);

int binaryImgHorizontalChange(cv::Mat &img, std::vector<int> &horizontalChangeVec);

void showVecHist(std::vector<int> vec);

int vecSum(std::vector<int> vec);

void vecHighValuePassFilter(std::vector<int> &vec, int threadhold);

class Seg {
 public:
  Seg(int head, int tail) {
      this->head = head;
      this->tail = tail;
      this->width = tail - head;
  }
  int getWidth() {
      return this->width;
  }
  int getHead() {
      return this->head;
  }
  int getTail() {
      return this->tail;
  }

//	Seg& operator=(Seg s) {
//		this->head = s.head;
//		this->tail = s.tail;
//		this->width = s.width;
//		return *this;
//	}

 private:
  int head;
  int tail;
  int width;
};

bool SegCompareByWidth(Seg a, Seg b);

bool SegCompareByHead(Seg a, Seg b);

std::vector<Seg> vec2Segs(std::vector<int> vec);

int segsFilter(std::vector<Seg> &segs, int threshold = 5);

int vecInSegPassFilter(std::vector<int> &vec, std::vector<Seg> segs);

void vecNormalization(std::vector<double> &vec, int scale);

double vecNorm(std::vector<double> vec, double N);



