#include "ContentCheck.h"
#include "PatternData.h"

int  ContentCheck::Pattern_Check_AUS(Mat& src)
{
	//Mat SrcImg = src.clone();
	Mat SrcImg;
	resize(src, SrcImg, Size(src.cols / 2, src.rows / 2), 0, 0, INTER_LINEAR);

	Mat ResultImage1 = Pattern_UV_PROCESS2(SrcImg, 2, -50, 2);
	//Mat ResultImage1 = Pattern_UV_PROCESS2(SrcImg, 2, -60, 2);

	//imshow("ResultImage3", ResultImage1);
	int check = CheckValid(ResultImage1, 6000, 0);

	if (!check)
		return 2;

	Mat NewImg = SrcImg(Rect(0, 0, 300, 100));

	ResultImage1 = Pattern_UV_PROCESS2(NewImg, 4, -20, 2);
	//ResultImage1 = Pattern_UV_PROCESS2(NewImg, 4, -30, 2);
	//imshow("ResultImage4", ResultImage1);
	check = CheckValid(ResultImage1, 6000, 2000);

	if (check)
		return 1;
	else
		return 2;
}