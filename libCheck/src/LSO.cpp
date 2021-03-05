#include "ContentCheck.h"
#include "PatternData.h"

int  ContentCheck::Pattern_Check_LSO(Mat& src)
{
	Mat SrcImg;
	resize(src, SrcImg, Size(src.cols / 2, src.rows / 2), 0, 0, INTER_LINEAR);

	//int(*ImgData)[7] = LSO_DATA;

	Mat NewImg = SrcImg(Rect(250, 30, 270, 320));
    //imshow("NewImg", NewImg);

	//检查绿色通道下像素点数
	Mat ResultImage5 = Pattern_UV_PROCESS2(NewImg,2, -10, 1);
	//Mat ResultImage1 = Pattern_UV_PROCESS2(NewImg, 4, ImgData[0][0], 1);
	//imshow("ResultImage5", ResultImage5);
	int check = CheckValid(ResultImage5, 21700, 14400);

	if (check)
		return 1;
	else
		return 2;
}