#include "ContentCheck.h"
#include "PatternData.h"

int  ContentCheck::Pattern_Check_IDN(Mat& src)
{
	Mat SrcImg;
	resize(src, SrcImg, Size(src.cols / 2, src.rows / 2), 0, 0, INTER_LINEAR);
	//SrcImg = src.clone();
	Scalar tempVal = cv::mean(SrcImg);
	float ImgMean = tempVal.val[0];
	//cout << "ImgMean: " << ImgMean << endl;
	if (ImgMean > 180)
		return 1;


	//int(*ImgData)[7] = IDN_DATA;

	//Mat NewImg = SrcImg(Rect(150, 60, 200, 200));
	Mat NewImg = SrcImg(Rect(150, 60, 300, 300));
	//Mat NewImg = SrcImg(Rect(ImgData[0][1], ImgData[0][2], ImgData[0][3], ImgData[0][4]));
	//imshow("NewImg1", NewImg);

	//Mat ResultImage1 = Pattern_UV_PROCESS2(NewImg, ImgData[0][6], ImgData[0][0], ImgData[0][5]);
	//Mat ResultImage1 = Pattern_UV_PROCESS2(NewImg, 2, ImgData[0][0], ImgData[0][5]);
	//Mat ResultImage1 = Pattern_UV_PROCESS2(NewImg, 2, -70, 2);
	Mat ResultImage1 = Pattern_UV_PROCESS2(NewImg, 2, -60, 2);

	//imshow("ResultImage1", ResultImage1);
	int check = CheckValid(ResultImage1, 4200, 2000);

	if (check == 1)
		return 1;


	//NewImg = src(Rect(150, 30, 300, 250));
	NewImg = src(Rect(150, 20, 460, 350));
	//NewImg = SrcImg(Rect(ImgData[1][1], ImgData[1][2], ImgData[1][3], ImgData[1][4]));

	//imshow("NewImg3", NewImg);
	//ResultImage1 = Pattern_UV_PROCESS2(NewImg, ImgData[1][6], ImgData[1][0], ImgData[1][5]);
	ResultImage1 = Pattern_UV_PROCESS2(NewImg, 2, -10, 1);

	//imshow("ResultImage3", ResultImage1);
	check = CheckValid(ResultImage1, 28000, 10000);

	if (check == 1)
		return 1;
	else
		return 2;

}