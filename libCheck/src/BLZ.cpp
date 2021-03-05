#include "ContentCheck.h"
#include "PatternData.h"

int  ContentCheck::Pattern_Check_BLZ(Mat& src)
{
	Mat SrcImg;
	resize(src, SrcImg, Size(src.cols / 2, src.rows / 2), 0, 0, INTER_LINEAR);

	//int(*ImgData)[7] = BLZ_DATA;

	//第一次检查蓝色通道下像素点数
	Mat NewImg = SrcImg(Rect(50, 60, 280, 270));

	//imshow("NewImg", NewImg);

	Mat ResultImage4 = Pattern_UV_PROCESS2(NewImg, 8, -60, 0);
	//Mat ResultImage1 = Pattern_UV_PROCESS2(NewImg, 4, ImgData[0][0], 1);

	//imshow("ResultImage4", ResultImage4);
	int check = CheckValid(ResultImage4, 7700, 5100);

	/*if (!check)
		return 2;*/

	if (!check)
		return 2;

	//第一次检查绿色通道下像素点数
	Mat ResultImage5 = Pattern_UV_PROCESS2(NewImg, 8, -40, 1);
	//Mat ResultImage1 = Pattern_UV_PROCESS2(NewImg, 4, ImgData[0][0], 1);

	//imshow("ResultImage5", ResultImage5);
	check = CheckValid(ResultImage5, 15300, 10100);

	if (check)
		return 1;
	else
		return 2;
}