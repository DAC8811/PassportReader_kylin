#include "ContentCheck.h"
#include "PatternData.h"

int  ContentCheck::Pattern_Check_GRC(Mat& src)
{
	Mat SrcImg;
	resize(src, SrcImg, Size(src.cols / 2, src.rows / 2), 0, 0, INTER_LINEAR);

	//SrcImg = src.clone();

	Mat NewImg = SrcImg(Rect(330, 30, 290, 350));

	Mat ResultImage1 = Pattern_UV_PROCESS2(NewImg, 2, -30, 3);
	//Mat ResultImage1 = Pattern_UV_PROCESS2(NewImg, 2, -40, 3);
	//Mat ResultImage1 = Pattern_UV_PROCESS2(NewImg, 4, ImgData[0][0], 1);
	//int CAN_DATA[1][7] = { {-40, 260, 50, 350, 350, 1, 4 } };
	//imshow("ResultImage4", ResultImage1);
	int check = CheckValid(ResultImage1, 45000, 12000);

	if (check)
		return 1;
	else
		return 2;
}