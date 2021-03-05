#include "ContentCheck.h"


#define Thread_Valid_MIN 30
#define Thread_Valid_MAX 350

int ContentCheck::Pattern_UV(const Mat& src, int VALID_MAX, int VALID_MIN, int vlaue)
{
	Mat SrcImage = src.clone();
	if (!SrcImage.data)
	{
		SrcImage.release();
		return 0;
	}

	Mat ResultImage = Pattern_UV_PROSCESS(SrcImage, vlaue);

	//imshow("Pattern", ResultImage);
	int state;
	Mat Seg;
	for (int i = 2; i > -1; i--)
	{
		Seg = ResultImage(Rect(ResultImage.cols * i / 3, 0, ResultImage.cols / 3, ResultImage.rows));
		state = Pattern_Search_UV(Seg);
		if (state == 1)
			break;
	}

	if (state == 1)
		return 3;

	//cout << "PatternSelect:" << state << endl;
	Seg = SrcImage(Rect(SrcImage.cols / 2, 0, SrcImage.cols / 2, SrcImage.rows));
	//imshow("Seg", Seg);
	state = Pattern_Thread_UV(Seg, vlaue, 0, 0);

	//cout << "state" << state << endl;
	if (state == 1)
		return 1;
	else
		return 2;

}

int ContentCheck::Pattern_Search_UV(const Mat& src)
{
	Mat SrcImage = src.clone();

	if (!SrcImage.data)
		return 0;
	//imshow("Pattern_Search_UVSrcImage", SrcImage);
	int BlockSize = 10;
	//int BlockSize = 5;

	int ColNum = SrcImage.cols / BlockSize;
	int RowNum = SrcImage.rows / BlockSize;

	int** MatBlock = CreateArray(RowNum, ColNum);

	int count = 0;
	int ValidNum = 0;
	Mat Seg;
	Mat get;
	int Num = 0;
	int PointX = 0;
	int PointY = 0;
	count = countNonZero(SrcImage);
	//cout << "有效像素点" << endl;
	//cout << count << endl;
	if (count > 10000)
		return 1;

	for (int i = 0; i < RowNum; i++)
	{
		PointY = i * BlockSize;
		for (int j = 0; j < ColNum; j++)
		{
			PointX = j * BlockSize;

			Seg = SrcImage(Rect(PointX, PointY, BlockSize, BlockSize));
			get = Seg.clone();

			ValidNum = countNonZero(get);

			if (ValidNum >(BlockSize * BlockSize * 1 / 2))
			{
				Num++;
				MatBlock[i][j] = 1;
				//cout << i;
				//cout << " ";
				//cout << j;
				//cout << endl;
			}
			else
			{
				MatBlock[i][j] = 0;
			}
		}
	}

	//cout << "Num:" << Num << endl;
	int Rad = count / (BlockSize * BlockSize);
	//cout << "Rad:" << Rad << endl;

	/*for (int i = ColNum - 1; i > -1; i--)
	{
	for (int j = 0; j < RowNum; j++)
	{
	if (MatBlock[j][i] == 1)
	{
	cout << j;
	cout << " ";
	cout << i;
	cout << endl;
	}

	count = 0;
	if (MatBlock[j][i] == 1)
	{
	int m = i - Rad / 2;
	m = (m < 0) ? 0 : m;
	for (; (m < (i + Rad / 2 + 1))&&(m < SrcImage.cols); m++)
	{
	int n = j - Rad / 2;
	n = (n < 0) ? 0 : n;
	for (; (n < (j + Rad / 2 + 1))&&(n < SrcImage.rows); n++)
	{
	if (MatBlock[n][m] == 1)
	count++;
	}
	}
	//cout << count << endl;
	if (count > (Num / 2))
	{
	state = 1;
	break;

	}
	}
	}
	}*/

	//if (Num > (Rad * 1 / 2))
	DeleteArray(MatBlock, RowNum);
	if (Num > (Rad * 2 / 3))
		return 1;
	else
		return 0;
}


int ContentCheck::Pattern_Thread_UV(const Mat& src, int select1, int select2, int select3)
{
	Mat SrcImage = src.clone();
	if (!SrcImage.data)
	{
		SrcImage.release();
		return 0;
	}

	int en1 = Thread_UV(SrcImage, 1);
	if (en1 == 1)
		return 1;
	int en2 = Thread_UV(SrcImage, 2);
	if (en2 == 1)
		return 1;
	int en3 = Thread_UV(SrcImage, 3);
	if (en3 == 1)
		return 1;

	SrcImage.release();

	/*if ((en1 == 1) & (en2 == 1))
	return 1;

	if ((en3 == 1)&(en2 == 1))
	return 1;

	if ((en1 == 1)&(en3 == 1))
	return 1;*/

	return 2;
	/*Mat ResultImage;
	bool en;
	int value ;
	for (int m = 1; m < 3; m++)
	{
	value = m * 10 + select1;
	ResultImage = Pattern_UV_PROSCESS(SrcImage, select1);

	en = CheckValid(ResultImage, 2000, 50);
	if (en == true)
	break;
	}

	if (en == false)
	return 2;*/



	//return en;


}


int ContentCheck::Thread_UV(Mat& src, int select)
{
	Mat SrcImage = src.clone();
	if (!SrcImage.data)
	{
		SrcImage.release();
		return 0;
	}

	Mat ResultImage;
	int check = 0;
	//int en1 = Thread_UV_Process(SrcImage, ResultImage, select);
	switch (select)
	{
	case 1://red
	{
		check = RGB_Red_Process(SrcImage, ResultImage);
		break;
	}
	case 2://green
	{
		check = RGB_Green_Process(SrcImage, ResultImage);
		break;
	}
	case 3://blue
	{
		check = RGB_Blue_Process(SrcImage, ResultImage);
		break;
	}
	default:check = 0; break;

	}

	if (check != 1)
	{
		SrcImage.release();
		ResultImage.release();
		return check;
	}

	//imshow("ResultImageUV", ResultImage);
	bool en = CheckValid(ResultImage, 4000, 100);

	if (!en)
	{
		SrcImage.release();
		ResultImage.release();
		return 4;
	}

	vector<std::vector<Point>> contours;

	findContours(ResultImage, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

	if (contours.empty())
	{
		contours.clear();
		contours.shrink_to_fit();
		SrcImage.release();
		ResultImage.release();
		return 2;
	}



	vector<vector<Point>> contours1;

	for (int i = 0; i < contours.size(); ++i)
	{
		//cout << "【计算出来的第" << i << "个轮廓的长度为：】" << contours[i].size() << endl;
		//if ((contours[i].size() > 50)&(contours[i].size() < 500))
		if ((contours[i].size() > 30)&(contours[i].size() < 300))
		//if ((contours[i].size() > 15)&(contours[i].size() < 150))
		{
			contours1.push_back(contours[i]);
		}
	}
	if (contours1.size()>50)
	{
		contours.clear();
		contours.shrink_to_fit();
		contours1.clear();
		contours1.shrink_to_fit();
		SrcImage.release();
		ResultImage.release();
		return 2;
	}

	if (contours1.empty())
	{
		contours.clear();
		contours.shrink_to_fit();
		contours1.clear();
		contours1.shrink_to_fit();
		SrcImage.release();
		ResultImage.release();
		return 2;
	}




	en = false;

	for (int i = 0; i < (int)contours1.size(); i++)
	{
		double CounArea = contourArea(contours1[i], false);
		//cout << "【用轮廓面积计算函数计算出来的第" << i << "个轮廓的面积为：】" << CounArea << endl;
		if ((CounArea > Thread_Valid_MIN)&(CounArea < Thread_Valid_MAX))
		//if ((CounArea > 15)&(CounArea < 175))
		{
			en = true;
			break;
		}
	}


	contours.clear();
	contours.shrink_to_fit();
	contours1.clear();
	contours1.shrink_to_fit();
	SrcImage.release();
	ResultImage.release();

	if (en)
		return 1;
	else
		return 2;

}