#include "ContentCheck.h"
#include "PatternData.h"

int  ContentCheck::Pattern_Check_HND(Mat& src)
{
	Mat SrcImg;
	resize(src, SrcImg, Size(src.cols / 2, src.rows / 2), 0, 0, INTER_LINEAR);
	//SrcImg = src.clone();
	//Mat Seg = SrcImg(Rect(220, 40, 160, 170));
	Mat Seg = src(Rect(10, 40, 610, 170));
	Scalar tempVal = cv::mean(Seg);
	float ImgMean = tempVal.val[0];
	cout << "ImgMean: " << tempVal.val[0] << endl;
	cout << "ImgMean: " << tempVal.val[1] << endl;
	cout << "ImgMean: " << tempVal.val[2] << endl;
	if (ImgMean > 180)
		return 1;
	else
		return 2;

}