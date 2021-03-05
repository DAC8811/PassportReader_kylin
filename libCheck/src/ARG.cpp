#include "ContentCheck.h"
#include "PatternData.h"

int  ContentCheck::Pattern_Check_ARG(Mat& src)
{
	Mat SrcImg;
	resize(src, SrcImg, Size(src.cols / 2, src.rows / 2), 0, 0, INTER_LINEAR);
	//SrcImg = src.clone();

	//Mat ResultImage1 = Pattern_UV_PROCESS2(SrcImg, 2, -90, 1);
	Mat ResultImage1 = Pattern_UV_PROCESS2(SrcImg, 2, -80, 1);

	//imshow("ResultImage1", ResultImage1);
	int check = CheckValid(ResultImage1, 2000, 0);

	if (check)
		return 1;
	else
		return 2;
}